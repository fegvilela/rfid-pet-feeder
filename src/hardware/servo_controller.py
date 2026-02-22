from viam.components.board import Board

from src.config import SERVO_CLOSED_ANGLE, SERVO_OPEN_ANGLE
from src.utils.logger import logger


class ServoController:
    def __init__(self, board: Board, feeder_id: int, pin: int):
        self.board = board
        self.feeder_id = feeder_id
        self.pin = pin
        self.is_open = False
        self._gpio_pin: Board.GPIOPin | None = None

    async def setup(self) -> None:
        self._gpio_pin = await self.board.gpio_pin_by_name(name=str(self.pin))

    async def open(self) -> None:
        logger.info(f"Feeder {self.feeder_id}: Opening servo on pin {self.pin}")
        if self._gpio_pin:
            await self._gpio_pin.set(high=True)
        await self._set_angle(SERVO_OPEN_ANGLE)
        self.is_open = True
        logger.info(f"Feeder {self.feeder_id}: Servo opened")

    async def close(self) -> None:
        logger.info(f"Feeder {self.feeder_id}: Closing servo on pin {self.pin}")
        await self._set_angle(SERVO_CLOSED_ANGLE)
        if self._gpio_pin:
            await self._gpio_pin.set(high=False)
        self.is_open = False
        logger.info(f"Feeder {self.feeder_id}: Servo closed")

    async def _set_angle(self, angle: int) -> None:
        duty_cycle = self._angle_to_duty_cycle(angle)
        if self._gpio_pin:
            await self._gpio_pin.set_pwm(duty_cycle=duty_cycle)

    def _angle_to_duty_cycle(self, angle: int) -> float:
        min_duty = 0.05
        max_duty = 0.10
        return min_duty + (angle / 180.0) * (max_duty - min_duty)
