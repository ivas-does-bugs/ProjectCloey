# Project Overview

This project consists of two main components:

- A **Flask** server that handles image generation and upload requests.
- An **ESP32** client that requests images and displays them on a TFT screen.

---

# Libraries Used

- **TFT_eSPI** – for controlling the TFT display.  
- **JpegDecoder** – for decoding JPEG images to be displayed.  
- **Custom Configuration Files** – tailored for **TFT_eSPI** to support the specific display and pinout used.

---

# Pin Configuration

## TFT Display (SPI)

| Function       | GPIO Pin |
|----------------|----------|
| **SDO (MISO)** | GPIO 12  |
| **LED**        | GPIO 21  |
| **SCK**        | GPIO 14  |
| **SDI (MOSI)** | GPIO 13  |
| **D/C**        | GPIO 2   |
| **RESET**      | EN/RESET |
| **CS**         | GPIO 15  |
| **GND**        | GND      |
| **VCC**        | 3.3V     |

## SD Card (SPI)

| Function     | GPIO Pin |
|--------------|----------|
| **SD_CS**    | GPIO 5   |
| **SD_MOSI**  | GPIO 23  |
| **SD_MISO**  | GPIO 19  |
| **SD_SCK**   | GPIO 18  |

