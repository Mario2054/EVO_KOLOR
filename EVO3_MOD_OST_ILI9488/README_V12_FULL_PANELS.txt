# EVO3 MOD OST ILI9488 PlatformIO V12 FULL PANELS

Projekt zrobiony na bazie:
- `EVO_Radio_Arduino_ILI9488_V7_VU_FIXED_27MHz_RECOMMENDED.ino` jako baza ILI9488/LovyanGFX,
- `EVO 3 mod ost.zip` jako pełna logika funkcji EVO3.

## Nowe panele

Dodana jest osobna warstwa UI:

```text
include/EvoV12Panels.h
src/EvoV12Panels.cpp
```

Panele są rysowane natywnie w 480x320 przez LovyanGFX, w stylu z przesłanych grafik.

Zawarte panele:
01 Boot / Start
02 Radio główny
03 Info stacji
04 VU Boomerang
05 VU Analog
06 VU Crystal / Blue
07 VU Wing
08 Zegar
09 Volume overlay
10 Lista stacji
11 Wybór banku
12 Info stacji
13 Equalizer
14 FFT
15 Zaawansowany analizator
16 Bluetooth player
17 Bluetooth settings
18 SD player
19 SD recorder
20 SD file browser
21 WiFi / WebUI
22 OTA
23 Sleep timer
24 Station search
25 Favorites
26 Settings
27 System monitor
28 Alert
29 Power off
30 Recovery / safe mode
31 About

## Warstwa zgodności

```text
include/EvoDisplayCompat.h
src/EvoDisplayCompat.cpp
```

Zawiera:
- klasę LGFX dla ILI9488,
- globalny `display`,
- wrapper `u8g2`,
- alias `U8G2`, żeby stare pliki EVO3 dalej się kompilowały.

## Połączenia ILI9488

```text
MOSI -> GPIO39
SCK  -> GPIO38
DC   -> GPIO40
RST  -> GPIO41
CS   -> GND fizycznie
BL   -> 3.3V
GND  -> GND
```

## Wydajność audio

W UI ustawiono ograniczenie odświeżania:
- tryby dynamiczne ok. 90 ms,
- tryby statyczne ok. 300 ms.

Długie `delay(50)` po `audio.loop()` zostały zamienione na `delay(1)`.

## Uruchomienie

Otwórz folder w VS Code z PlatformIO i uruchom Build.

Jeśli masz inny moduł ESP32-S3, zmień w `platformio.ini`:

```ini
board = esp32-s3-devkitc-1
```
