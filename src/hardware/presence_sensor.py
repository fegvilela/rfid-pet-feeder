import asyncio

from viam.components.board import Board

from src.utils.logger import logger


class PresenceSensor:
    def __init__(self, board: Board, feeder_id: int, pin: int):
        self.board = board
        self.feeder_id = feeder_id
        self.pin = pin
        self._gpio_pin: Board.GPIOPin | None = None

    async def setup(self) -> None:
        self._gpio_pin = await self.board.gpio_pin_by_name(name=str(self.pin))

    async def is_present(self) -> bool:
        if self._gpio_pin:
            return await self._gpio_pin.get()
        return False

    async def wait_for_presence(self, timeout: float = 30.0) -> bool:
        logger.info(f"Feeder {self.feeder_id}: Waiting for presence (timeout: {timeout}s)")
        start_time = asyncio.get_event_loop().time()

        while (asyncio.get_event_loop().time() - start_time) < timeout:
            if await self.is_present():
                logger.info(f"Feeder {self.feeder_id}: Presence detected")
                return True
            await asyncio.sleep(0.1)

        logger.info(f"Feeder {self.feeder_id}: No presence detected within timeout")
        return False

    async def wait_for_absence(self, check_interval: float = 0.5) -> None:
        logger.info(f"Feeder {self.feeder_id}: Waiting for absence")

        while await self.is_present():
            await asyncio.sleep(check_interval)

        logger.info(f"Feeder {self.feeder_id}: Absence detected")
