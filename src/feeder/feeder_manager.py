import asyncio

from viam.components.board import Board

from src.config import CAT_FEEDER_MAPPING, SENSOR_PINS, SERVO_PINS
from src.feeder.feeder import Feeder
from src.hardware.presence_sensor import PresenceSensor
from src.hardware.servo_controller import ServoController
from src.utils.logger import logger


class FeederManager:
    def __init__(self, board: Board):
        self.board = board
        self.feeders: dict[int, Feeder] = {}
        self._monitoring_tasks: dict[int, asyncio.Task] = {}
        self._build_feeders()

    def _build_feeders(self):
        feeder_cats: dict[int, list[str]] = {1: [], 2: [], 3: []}

        for cat_name, feeder_id in CAT_FEEDER_MAPPING.items():
            if feeder_id in feeder_cats:
                feeder_cats[feeder_id].append(cat_name)

        for feeder_id in [1, 2, 3]:
            servo = ServoController(
                board=self.board, feeder_id=feeder_id, pin=SERVO_PINS[feeder_id]
            )
            sensor = PresenceSensor(
                board=self.board, feeder_id=feeder_id, pin=SENSOR_PINS[feeder_id]
            )
            self.feeders[feeder_id] = Feeder(
                feeder_id=feeder_id, servo=servo, sensor=sensor, allowed_cats=feeder_cats[feeder_id]
            )

            logger.info(f"Feeder {feeder_id}: Created with allowed cats: {feeder_cats[feeder_id]}")

    async def setup(self):
        for feeder in self.feeders.values():
            await feeder.servo.setup()
            await feeder.sensor.setup()

    async def handle_cat_detected(self, cat_name: str) -> bool:
        feeder_id = CAT_FEEDER_MAPPING.get(cat_name.lower())

        if not feeder_id:
            logger.warning(f"Unknown cat: {cat_name}")
            return False

        feeder = self.feeders.get(feeder_id)
        if not feeder:
            logger.error(f"Feeder {feeder_id} not found")
            return False

        if feeder.is_busy:
            logger.info(f"Feeder {feeder_id} is busy, skipping")
            return False

        success = await feeder.open_for_cat(cat_name)
        if success:
            task = asyncio.create_task(feeder.monitor_and_close())
            self._monitoring_tasks[feeder_id] = task
            return True

        return False

    async def close_all(self) -> None:
        logger.info("Closing all feeders")
        for task in self._monitoring_tasks.values():
            task.cancel()

        await asyncio.gather(*self._monitoring_tasks.values(), return_exceptions=True)

        for feeder in self.feeders.values():
            await feeder._safe_close()

        self._monitoring_tasks.clear()
        logger.info("All feeders closed")
