# Fedorentos Pet Feeder 🐈👃

## Overview

This is an automation project for feeding my 5 cats. I have 3 automatic feeders, but access to them needs to be regulated - each feeder can be accessed by a specific group of cats.

### How It Works

A Raspberry Pi controls 3 servo motors that open the protective cover for each feeder individually. The system uses:

- **Image Recognition**: Identifies which cat is approaching and opens the correct feeder cover
- **Presence Sensors**: One sensor per feeder prevents the motor from closing the cover while the cat is eating

This ensures each cat (or group of cats) can only access their designated feeder, solving the problem of dietary restrictions or special meals for specific cats.

---

## Original Idea (Deprecated)

Initially, the plan was that each cat would have an RFID tag on its collar and the pet feeder would open only to the "owner" of that feeder. This was replaced by image recognition for a more seamless experience.

![cat loading](https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExZmZ4bnQ1OGk2MGx2OTdnM2I3ajEwbjkxNnhkdTdreHpwN2ExOHprbCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/pY8jLmZw0ElqvVeRH4/giphy.gif)

## Action Plan – Smart Feeder with Image Recognition (Viam and Raspberry Pi)

### 1. Prepare the Raspberry Pi
- Confirm model and install Raspberry Pi OS 64-bit with Wi-Fi configured.
- Create Viam account, register Pi as *machine* and install `viam-server` via **CONFIGURE** tab assistant.

### 2. Configure Hardware in Viam
- In machine's **Config** tab:
  - Create `board` component model `pi` (GPIO for motor/relay).
  - Create `camera` component pointing to feeder.
  - Create motor component (servo/stepper) using `rpi-servo` module or equivalent.
- Test camera and motor in **Control** tab.

### 3. Collect Images for Dataset
- Capture feeder photos only when cat is present.
- For each cat: save ~100–200 images in different positions, times, and lighting.
- In Viam Data Manager:
  - Create **dataset**.
  - Label images in 6 classes: `cat1`, `cat2`, `cat3`, `cat4`, `cat5`, `others`.
- Cost optimization:
  - Use moderate resolution (e.g., 640×480).
  - Upload only necessary (total ~0.2–0.5 GB).

### 4. Train Model in Viam
- In dataset, click **Train model**.
- Select image classification model with *built-in training*.
- Wait for completion and test model in panel with new images.

### 5. Optimize and Clean Cloud Data
- Download local dataset copy (optional).
- In Data Manager, delete old/redundant images, keeping ~0.1–0.2 GB.
- Reduces monthly storage cost to cents (typically free tier covered).

### 6. Deploy Model on Raspberry Pi
- In **CONFIGURE** tab:
  - Add **ML model** service type `TFLite CPU`.
  - Select trained model and check "Deploy on machine".
  - Add `mlmodel vision` service using model + feeder camera.

### 7. Local Control Script (Offline)
- Create Python script on Pi using Viam SDK:
  - Connect via `localhost`.
  - Read classifications (`get_classifications_from_camera`) every X seconds.
  - If cat identified with confidence ≥ threshold → trigger motor and log locally.
- Configure auto-start (systemd).

### 8. Maintenance and Retraining
- Periodically (1–2 months):
  - Collect/upload new images only for error cases.
  - Update dataset and retrain.
  - Delete old sets to keep cloud data < 1 GB.

## Architecture (High Level)
```
              +-----------------------------+
              |        Viam Cloud           |
              |  - Data Manager / Dataset  |
              |  - Model Training (ML)     |
              |  - Configuration Panel     |
              +-------------+--------------+
                            ^
    Image Upload (occasional)|  Trained TFLite Model
        (minimal volume)    v
+-------------------------------+
|         Internet (optional)   |
+-------------------------------+

+-------------------------------+
|       Raspberry Pi (2017)     |
|  - viam-server                |
|  - TFLite ML Model Service    |
|  - Vision Service             |
|  - Python Control Script      |
+--------+--------------+-------+
         |              |
         |              |
   +-----v----+   +-----v------+
   |  Camera  |   |  Motor     |
   |  (USB)   |   | (servo/    |
   |          |   |  stepper)  |
   +----------+   +------------+
```
Runtime Flow (with or without internet):
1. Camera captures feeder image.
2. Vision service runs local TFLite model → class (cat1..cat5/others).
3. Python script reads classification:
   - If cat X with high confidence → triggers motor to dispense food.
   - Logs event to local file.

## Viam Setup

More details [here](docs/viam-setup.md)

---

## Installation

### 1. Clone the repository

```bash
git clone <repo-url>
cd fedorentos-pet-feeder
```

### 2. Install dependencies

```bash
uv sync
```

### 3. Configure environment variables

```bash
cp config/.env.example config/.env
# Edit config/.env with your values
```

### 4. Run the application

```bash
uv run python -m src.main
```

---

## Systemd Service (Auto-start on boot)

### Install the service

```bash
sudo cp deploy/fedorentos.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable fedorentos
sudo systemctl start fedorentos
```

### Manage the service

```bash
sudo systemctl status fedorentos   # Check status
sudo systemctl restart fedorrentos # Restart
sudo systemctl stop fedorrentos    # Stop
journalctl -u fedorrentos -f       # View logs
```

---

## Project Structure

```
src/
├── main.py              # Entry point
├── config.py            # Environment configuration
├── viam_connection.py   # Viam SDK connection
├── hardware/
│   ├── servo_controller.py   # Servo motor control
│   └── presence_sensor.py    # PIR sensor reading
├── vision/
│   └── classifier.py    # Cat image classification
├── feeder/
│   ├── feeder.py        # Individual feeder logic
│   └── feeder_manager.py # Orchestrates all feeders
└── utils/
    └── logger.py        # Logging configuration
```
