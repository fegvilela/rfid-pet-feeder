import asyncio
from datetime import datetime

from src.config import FEEDER_OPEN_TIMEOUT, PRESENCE_CHECK_INTERVAL
from src.hardware.presence_sensor import PresenceSensor
from src.hardware.servo_controller import ServoController
from src.utils.logger import logger


class Feeder:
    def __init__(
        self,
        feeder_id: int,
        servo: ServoController,
        sensor: PresenceSensor,
        allowed_cats: list[str],
    ):
        self.feeder_id = feeder_id
        self.servo = servo
        self.sensor = sensor
        self.allowed_cats = allowed_cats
        self.is_busy = False
        self.last_opened: datetime | None = None

    def can_access(self, cat_name: str) -> bool:
        return cat_name.lower() in [c.lower() for c in self.allowed_cats]

    async def open_for_cat(self, cat_name: str) -> bool:
        if self.is_busy:
            logger.warning(f"Feeder {self.feeder_id}: Already busy, cannot open for {cat_name}")
            return False

        if not self.can_access(cat_name):
            logger.warning(f"Feeder {self.feeder_id}: Cat '{cat_name}' not allowed")
            return False

        logger.info(f"Feeder {self.feeder_id}: Opening for cat '{cat_name}'")
        self.is_busy = True
        self.last_opened = datetime.now()

        await self.servo.open()
        return True

    async def monitor_and_close(self) -> None:
        try:
            await self._wait_and_close()
        except asyncio.CancelledError:
            logger.info(f"Feeder {self.feeder_id}: Monitoring cancelled")
        finally:
            await self._safe_close()
            self.is_busy = False

    async def _wait_and_close(self) -> None:
        start_time = asyncio.get_event_loop().time()

        while True:
            elapsed = asyncio.get_event_loop().time() - start_time

            if elapsed >= FEEDER_OPEN_TIMEOUT:
                logger.warning(f"Feeder {self.feeder_id}: Timeout reached, closing")
                break

            if not await self.sensor.is_present():
                logger.info(f"Feeder {self.feeder_id}: Cat left after {elapsed:.1f}s")
                await asyncio.sleep(2)
                if not await self.sensor.is_present():
                    break

            await asyncio.sleep(PRESENCE_CHECK_INTERVAL)

    async def _safe_close(self) -> None:
        try:
            await self.servo.close()
        except Exception as e:
            logger.error(f"Feeder {self.feeder_id}: Error closing servo: {e}")
