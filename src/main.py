import asyncio
import signal

from src.config import CLASSIFICATION_INTERVAL
from src.feeder.feeder_manager import FeederManager
from src.utils.logger import logger
from src.viam_connection import ViamConnection
from src.vision.classifier import CatClassifier


class PetFeederApp:
    def __init__(self):
        self.connection = ViamConnection()
        self.classifier: CatClassifier | None = None
        self.feeder_manager: FeederManager | None = None
        self.running = False

    async def start(self):
        logger.info("Starting Pet Feeder App")

        await self.connection.connect()
        logger.info("Connected to Viam")

        board = self.connection.get_board()
        vision = self.connection.get_vision_service()
        camera = self.connection.robot.resource_by_name("camera")

        self.classifier = CatClassifier(vision, camera)
        self.feeder_manager = FeederManager(board)
        await self.feeder_manager.setup()

        self.running = True
        logger.info("Pet Feeder App started successfully")

    async def stop(self):
        logger.info("Stopping Pet Feeder App")
        self.running = False

        if self.feeder_manager:
            await self.feeder_manager.close_all()

        await self.connection.disconnect()
        logger.info("Pet Feeder App stopped")

    async def run(self):
        await self.start()

        loop = asyncio.get_event_loop()
        for sig in (signal.SIGINT, signal.SIGTERM):
            loop.add_signal_handler(sig, lambda: asyncio.create_task(self.stop()))

        try:
            while self.running:
                await self._process_frame()
                await asyncio.sleep(CLASSIFICATION_INTERVAL)
        except asyncio.CancelledError:
            pass
        finally:
            await self.stop()

    async def _process_frame(self):
        if not self.classifier or not self.feeder_manager:
            return

        cat_name, confidence = await self.classifier.classify()

        if cat_name and cat_name.lower() != "others":
            logger.info(f"Cat detected: {cat_name} (confidence: {confidence:.2f})")
            await self.feeder_manager.handle_cat_detected(cat_name)


async def main():
    app = PetFeederApp()
    await app.run()


if __name__ == "__main__":
    asyncio.run(main())
