from viam.app.viam_client import ViamClient
from viam.components.board import Board
from viam.components.servo import Servo
from viam.rpc.dial import DialOptions
from viam.services.vision import VisionClient

from src.config import VIAM_API_KEY, VIAM_API_KEY_ID


class ViamConnection:
    def __init__(self):
        self.client: ViamClient = None
        self.robot = None

    async def connect(self):
        dial_options = DialOptions(api_key=VIAM_API_KEY, api_key_id=VIAM_API_KEY_ID)
        self.client = await ViamClient.create_from_dial_options(dial_options)
        self.robot = self.client.get_robot("fedorentos-pet-feeder")

    async def disconnect(self):
        if self.client:
            await self.client.close()

    def get_board(self, name: str = "board") -> Board:
        return Board.from_robot(self.robot, name)

    def get_servo(self, name: str) -> Servo:
        return Servo.from_robot(self.robot, name)

    def get_vision_service(self, name: str = "vision") -> VisionClient:
        return VisionClient.from_robot(self.robot, name)
