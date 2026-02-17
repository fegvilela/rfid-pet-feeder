# Fedorentos Pet Feeder

This is a project for my cats. Currently, I have 5 smelly felines and two of them need to eat a separate meal, so we need at least three separate pet feeders.

The idea here is that each cat will have a RFID tag on its collar and the pet feeder will open only to the "owner" of that feeder.

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

Runtime Flow (with or without internet):
1. Camera captures feeder image.
2. Vision service runs local TFLite model → class (cat1..cat5/others).
3. Python script reads classification:
   - If cat X with high confidence → triggers motor to dispense food.
   - Logs event to local file.

## Viam Setup

More details [here](docs/viam-setup.md)
