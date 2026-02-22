import os

from dotenv import load_dotenv

load_dotenv()

VIAM_API_KEY = os.getenv("VIAM_API_KEY")
VIAM_API_KEY_ID = os.getenv("VIAM_API_KEY_ID")

SERVO_PINS = {
    1: int(os.getenv("SERVO_1_PIN", 18)),
    2: int(os.getenv("SERVO_2_PIN", 19)),
    3: int(os.getenv("SERVO_3_PIN", 20)),
}

SENSOR_PINS = {
    1: int(os.getenv("SENSOR_1_PIN", "23")),
    2: int(os.getenv("SENSOR_2_PIN", "24")),
    3: int(os.getenv("SENSOR_3_PIN", "25")),
}

CAT_FEEDER_MAPPING = {}
for env_var in os.environ:
    if env_var.startswith("CAT_FEEDER_"):
        cat_name = env_var.replace("CAT_FEEDER_", "").lower()
        feeder_id_str = os.getenv(env_var, "0")
        feeder_id = int(feeder_id_str) if feeder_id_str else 0
        CAT_FEEDER_MAPPING[cat_name] = feeder_id

FEEDER_OPEN_TIMEOUT = int(os.getenv("FEEDER_OPEN_TIMEOUT", 300))
PRESENCE_CHECK_INTERVAL = float(os.getenv("PRESENCE_CHECK_INTERVAL", 0.5))
CLASSIFICATION_INTERVAL = float(os.getenv("CLASSIFICATION_INTERVAL", 1.0))
CONFIDENCE_THRESHOLD = float(os.getenv("CONFIDENCE_THRESHOLD", 0.7))

SERVO_OPEN_ANGLE = 90
SERVO_CLOSED_ANGLE = 0
