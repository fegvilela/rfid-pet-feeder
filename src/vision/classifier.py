from viam.components.camera import Camera
from viam.services.vision import VisionClient

from src.config import CONFIDENCE_THRESHOLD
from src.utils.logger import logger


class CatClassifier:
    def __init__(self, vision_service: VisionClient, camera: Camera):
        self.vision = vision_service
        self.camera = camera

    async def classify(self) -> tuple[str | None, float]:
        try:
            classifications = await self.vision.get_classifications_from_camera(
                self.camera.name, count=1
            )

            if not classifications:
                logger.debug("No classifications returned")
                return None, 0.0

            top_classification = classifications[0]
            cat_name = top_classification.class_name
            confidence = top_classification.confidence

            logger.debug(f"Classification: {cat_name} (confidence: {confidence:.2f})")

            if confidence < CONFIDENCE_THRESHOLD:
                logger.debug(
                    f"Confidence below threshold: {confidence:.2f} < {CONFIDENCE_THRESHOLD}"
                )
                return None, confidence

            return cat_name, confidence

        except Exception as e:
            logger.error(f"Error during classification: {e}")
            return None, 0.0

    async def get_camera_frame(self) -> bytes:
        image = await self.camera.get_image()  # type: ignore[attr-defined]
        return image.data
