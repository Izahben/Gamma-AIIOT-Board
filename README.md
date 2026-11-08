# Gamma-AIIoT-Board

Gamma AI + IoT is a custom embedded system I designed and built from scratch to combine artificial intelligence and IoT in a single, easy-to-use platform.

<img width="600" alt="GAMMA AI" src="https://github.com/user-attachments/assets/4e927772-7d85-4c07-9fe2-76124bc2326c" />

## What is Gamma?
Gamma is an AI-powered IoT platform designed to let users interact with connected devices through voice, text, and a web dashboard. Its current prototype supports GPIO configuration and LED control through a locally hosted interface.

<img src="https://github.com/user-attachments/assets/39d08154-0a0b-4c88-b4ec-112006c5e951" alt="Gamma PCB, 3D render" width="700">

*Gamma PCB design, 3D render. The PCB and enclosure are in production.*


## Hardware

- ESP32-S3
- INMP441 microphone and an amplifier for speech in and out
- OLED display
- GPIO expansion header for external devices
- Volume buttons and other IO interfaces

## Status

Early stage, built step by step.

- [x] Wi-Fi access point and web dashboard served from the board
- [x] Pick any usable GPIO, name it, assign it as an LED or relay, and toggle it
- [x] Devices are saved on the board and survive power loss
- [x] Dashboard works on phone, tablet and desktop
- [x] Servo, push button and switch
- [ ] Remote access from anywhere (cloud link)
- [ ] AI chat that controls devices by name
- [ ] I2C sensors, such as temperature
- [ ] Voice with the on-board microphone and OLED face

## Quick start

1. Install [PlatformIO](https://platformio.org/) in VS Code.
2. Copy `include/secrets.example.h` to `include/secrets.h` and set your own Wi-Fi password.
3. Connect the board and upload: `pio run -t upload`
4. Join the **Gamma** Wi-Fi network and open `http://192.168.4.1`.

## Project layout

```
src/        firmware (C++, Arduino framework)
include/    config.h and the secrets template
web/        dashboard (HTML, CSS, JavaScript) packed into the firmware at build time
scripts/    build helper that embeds web/ into the firmware
docs/       images
```

Built with PlatformIO, ESPAsyncWebServer and AsyncTCP. The dashboard fonts are Syne and JetBrains Mono, under the SIL Open Font License.
