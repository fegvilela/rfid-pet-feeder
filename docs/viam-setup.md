[1] Create Viam account
    |
    v
[2] Add new "machine" in dashboard
    |
    v
[3] Follow viam-server installation instructions on Raspberry Pi
    - Run installation commands in Pi terminal
    - Confirm machine shows "Live"
    |
    v
[4] In CONFIG tab:
    - Add "board" component (pi)
    - Add "camera" component (webcam)
    - Add "motor" component (servo/stepper)
    |
    v
[5] Test in CONTROL tab
    - View camera video
    - Test motor movement
    |
    v
[6] In DATA section:
    - Create DATASET
    - Upload/label images (cat1..cat5/others)
    |
    v
[7] Train model
    - DATASET → "Train model"
    - Wait for completion and test model
    |
    v
[8] Deploy model
    - CONFIG → add "ML model / TFLite CPU" service
    - Select trained model and check "Deploy on machine"
    - Add "mlmodel vision" service using model + camera
    |
    v
[9] Code
    - Download Python example from panel (Code Sample)
    - Adapt logic for:
        * Call vision.get_classifications_from_camera()
        * If cat identified → trigger motor
