# Tank Turret Code

This project creates a remote-controlled turret that shoots Nerf Rival rounds. The turret is built on a tank-like wheelbase and uses two brushless motors to launch rounds and two servo motors to control turret orientation. The system is powered by two ESP32 microcontrollers, enabling wireless communication between the remote and the turret.

## Features

- **High-speed launch system**: Two brushless motors spin rapidly to shoot Nerf balls at high speeds.
- **Remote control**: Separate remote control unit communicates with the turret over radio, offering flexible operation.
- **360° movement**: Dual servos allow full control over the turret's left-right and up-down movement.
- **Tank-style mobility (future goal)**: Mounted on a wheeled base for full maneuverability.

## Components

- **ESP32 Microcontrollers** (2): One on the turret and one in the remote, communicating wirelessly.
- **Brushless Motors** (2): Spin to launch Nerf balls.
- **Servo Motors** (2): Control the turret’s angle and direction.
- **Wheeled Base (optional)**: Planned feature for tank-like movement.

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/your-username/nerf-turret-tank.git
   cd nerf-turret-tank
   ```

2. **Install dependencies**:
   Ensure your environment is set up for ESP32 development:
   - [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
   - Python packages for ESP32:
     ```bash
     pip install esptool
     ```

3. **Flash firmware**:
   ```bash
   idf.py flash
   ```

## Usage

1. **Power up the turret and remote ESP32 units**.
2. **Connect to the turret's Wi-Fi** (if set to a local network mode) or pair through Bluetooth if enabled.
3. **Control the turret**: Use joystick or button commands on the remote to aim and shoot.

## Future Improvements

- **Mobile Base**: Attach a motorized wheeled base to allow tank movement.
- **Advanced targeting**: Integrate camera for computer-vision-based targeting.
- **Battery Power System**: Upgrade for increased operational time.

## Contributing

Feel free to fork the repository and submit pull requests for new features, optimizations, or bug fixes!

## License

This project is licensed under the MIT License.

---

Let me know if you'd like any additional details!
