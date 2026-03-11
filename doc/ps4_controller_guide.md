# Flying with a PS4/PS5 Controller in Paparazzi

## Setup

1. Connect the controller via USB
2. Activate the Paparazzi virtual environment:
   ```bash
   source ~/marnix/pprzEnv/bin/activate
   ```
3. Launch Paparazzi:
   ```bash
   cd ~/marnix && ./paparazzi
   ```
4. In the **Operation** tab, select one of these sessions:
   - **Simulation - Gazebo + PS Controller** — for practice in the simulator
   - **Flight UDP + PS Controller** — for the real drone

---

## Stick layout

```
        L1/L2                        R1/R2
    ___________                   ___________
   |           |                 |           |
   |  L-stick  |  Share  Options |  R-stick  |
   |  YAW      |    [T] [O]      |  ROLL     |
   |  THROTTLE |       [PS]      |  PITCH    |
   |     [   ] |  [D-pad]  [X]  [S][T][O]   |
   |___________|                 |___________|
```

### Left stick
| Direction | Control |
|-----------|---------|
| Left / Right | **Yaw** (rotate drone) |
| Up | **Decrease throttle** |
| Down | **Increase throttle** |

> Note: throttle is inverted — push the stick **down** to go up.

### Right stick
| Direction | Control |
|-----------|---------|
| Left / Right | **Roll** (strafe left/right) |
| Up / Down | **Pitch** (fly forward/backward) |

---

## Autopilot mode buttons

| Button | Mode | What it means |
|--------|------|---------------|
| **X** | ATT (manual) | You control the drone manually with the sticks |
| **Square** | AUTO1 | Reserved / intermediate mode |
| **Triangle** | NAV (autonomous) | Drone follows the flight plan |

Always check the **GCS status bar** — it shows the current mode (KILL / ATT / NAV).

---

## Flight plan buttons (only work in NAV mode)

| Button | Action |
|--------|--------|
| **Options** | Jump to "Start Engine" block |
| **D-pad Up** | Jump to "Takeoff" block |
| **D-pad Down** | Jump to "Land here" block |

---

## Safety buttons

| Button | Action | Condition |
|--------|--------|-----------|
| **L1** | **Kill throttle** — motors off immediately | Any time |
| **R1** | **Resurrect throttle** — re-enable motors | Only works when left stick is at zero throttle (fully down) |

---

## Step-by-step: first flight

### In the simulator (practice first!)

1. Select **"Simulation - Gazebo + PS Controller"** and click **Start Session**
2. In the GCS, confirm **RC: OK** appears in the status bar
3. Press **X** to enter ATT (manual) mode
4. **Arm the motors:** push the left stick to the bottom-right corner and hold until the throttle indicator turns orange
5. Slowly increase throttle (push left stick down) until the drone lifts off
6. Use the right stick to move around, left stick to yaw and climb/descend
7. To stop: lower throttle fully, then press **L1** to kill the motors

### On the real drone

Follow the pre-flight checklist from the crash course manual first, then use the same steps as the simulator. Additionally:

- Make sure **GPS fix = 3D** (green in GCS) before takeoff
- Make sure **Link** indicator is green
- Keep a safety pilot ready to take over at any time

---

## Safety rules reminder

- **Never take off without a GPS fix (3D) on the real drone**
- **Do not disable the safety exceptions** in the flight plan (geofence, datalink loss, GPS loss, battery low)
- If the drone behaves unexpectedly: switch to **ATT mode (X)** and land manually
- If you need to land immediately: press **D-pad Down** (in NAV mode) or take over manually in ATT mode
- Emergency stop: press **L1** to kill motors

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `Could not initialize SDL` error | Run `source ~/marnix/pprzEnv/bin/activate` before starting Paparazzi |
| RC: not OK in GCS | Check controller is on `/dev/input/js0` — if not, change `-d 0` to the correct number in the session |
| Throttle can't arm | Make sure left stick is fully at zero before arming (stick all the way down) |
| Drone not responding to sticks | Check you are in **ATT** mode (X button), not KILL mode |
| Motors killed mid-flight | Move left stick to zero throttle, then press **R1** to resurrect |
