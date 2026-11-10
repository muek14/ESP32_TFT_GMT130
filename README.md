# ESP32_TFT_GMT130

Testprogramm für das **1.3" TFT-Display GMT130 V1.0** (Treiber ST7789, 240×240 Pixel, SPI)
an einem **ELEGOO ESP32** (30 Pins, ESP-WROOM-32).

Das Programm zeigt nacheinander einen Farbtest, einen Texttest und einen Grafiktest
und startet danach eine Animation: einen springenden Ball mit FPS-Anzeige.

![Plattform](https://img.shields.io/badge/Plattform-PlatformIO-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino-blue)
![Library](https://img.shields.io/badge/Library-TFT__eSPI-green)

---

## Hardware

| Komponente | Beschreibung |
|---|---|
| Mikrocontroller | ELEGOO ESP32 Board, 30 Pins (15 pro Seite), ESP-WROOM-32 |
| Display | Golden Morning [GMT130 V1.0](https://goldenmorninglcd.com/de/TFT-Anzeigemodul/1.3-Zoll-240-x-240-st7789-gmt130-v1.0/), 1.3", 240×240, ST7789, IPS |
| Schnittstelle | SPI (nur Schreiben), **ohne CS-Pin** |

### Verdrahtung

| Display-Pin | Funktion | ESP32 |
|---|---|---|
| GND | Masse | GND |
| VCC | Versorgung | **3V3** (nicht 5V/VIN!) |
| SCL | SPI-Takt (SCK) | D18 |
| SDA | SPI-Daten (MOSI) | D23 |
| RES | Reset | D26 |
| DC | Daten/Befehl | D27 |
| BLK | Hintergrundbeleuchtung | D25 (oder fest an 3V3) |

```
ESP32 (ELEGOO, 30 Pins)          GMT130 (1.3" ST7789)
┌──────────────┐                 ┌──────────────┐
│          GND ├─────────────────┤ GND          │
│          3V3 ├─────────────────┤ VCC          │
│          D18 ├─────────────────┤ SCL          │
│          D23 ├─────────────────┤ SDA          │
│          D26 ├─────────────────┤ RES          │
│          D27 ├─────────────────┤ DC           │
│          D25 ├─────────────────┤ BLK          │
└──────────────┘                 └──────────────┘
```

---

## Software

- [Visual Studio Code](https://code.visualstudio.com/) mit der Erweiterung [PlatformIO IDE](https://platformio.org/)
- Plattform `espressif32`, Framework `arduino`
- Bibliothek [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) von Bodmer (wird automatisch über `lib_deps` installiert)

### Konfiguration

TFT_eSPI wird vollständig über die `build_flags` in der [platformio.ini](platformio.ini) konfiguriert
(`USER_SETUP_LOADED`). Die `User_Setup.h` der Bibliothek muss daher **nicht** bearbeitet werden.

Wichtige Einstellungen:

| Flag | Wert | Bedeutung |
|---|---|---|
| `ST7789_DRIVER` | 1 | Display-Treiber |
| `TFT_WIDTH` / `TFT_HEIGHT` | 240 / 240 | Auflösung |
| `TFT_CS` | -1 | Kein Chip-Select vorhanden |
| `TFT_SPI_MODE` | `SPI_MODE3` | **Nötig, weil CS fehlt** – mit Modus 0 bleibt das Display schwarz |
| `SPI_FREQUENCY` | 40000000 | 40 MHz SPI-Takt |
| `TFT_BACKLIGHT_ON` | HIGH | Backlight leuchtet bei HIGH an BLK |

Im Quellcode ([src/ESP32_TFT_GMT130.cpp](src/ESP32_TFT_GMT130.cpp)) wird zusätzlich
`invertDisplay(true)` gesetzt (Konstante `INVERT_COLORS`), damit die Farben korrekt dargestellt werden.

---

## Bauen und Hochladen

In VS Code über die PlatformIO-Leiste:

- **✓ Build** – kompilieren
- **→ Upload** – auf den ESP32 flashen
- **🔌 Monitor** – serielle Ausgabe (115200 Baud)

Oder im Terminal:

```bash
pio run
pio run -t upload
pio device monitor
```

> Bleibt der Upload bei `Connecting...` hängen: die **BOOT**-Taste am Board gedrückt halten,
> bis das Schreiben beginnt.

---

## Programmablauf

1. **Farbtest** – Rot, Grün, Blau, Weiß, Schwarz, danach ein Regenbogen-Verlauf
2. **Texttest** – GLCD-, 16px-, 26px-, 7-Segment- und FreeSans-Schrift
3. **Grafiktest** – Linienfächer, Randtest (roter Rahmen ringsum), gefüllte und leere Formen
4. **Animation** – springender Ball mit Farbwechsel, oben FPS- und Laufzeitanzeige (Endlosschleife)

Serielle Ausgabe:

```
=== ESP32 + GMT130 (ST7789 240x240) Test ===
Display initialisiert: 240 x 240
Test 1: Farben
  Farbe: ROT
  ...
Test 4: Animation (Endlosschleife)
FPS: 91   Up: 12s
```

Gemessen wurden etwa **91 FPS** bei 40 MHz SPI-Takt.

---

## Fehlersuche

| Problem | Lösung |
|---|---|
| Display bleibt schwarz, Backlight aus | BLK-Verdrahtung prüfen bzw. BLK testweise an 3V3 legen |
| Display bleibt schwarz, Backlight an | SCL/SDA/DC/RES prüfen; `TFT_SPI_MODE=SPI_MODE3` gesetzt? `SPI_FREQUENCY` testweise auf 20000000 senken |
| Farben invertiert (Weiß ↔ Schwarz) | `INVERT_COLORS` im Quellcode umschalten |
| Rot und Blau vertauscht | In `build_flags` ergänzen: `-D TFT_RGB_ORDER=TFT_BGR` |
| Bild verschoben / Rand fehlt | Rotation (`setRotation`) bzw. Offsets prüfen |

---

## Projektstruktur

```
ESP32_TFT_GMT130/
├── include/                 Header-Dateien (leer)
├── lib/                     Lokale Bibliotheken (leer)
├── src/
│   └── ESP32_TFT_GMT130.cpp Testprogramm
├── test/                    Unit-Tests (leer)
├── platformio.ini           Projekt- und Display-Konfiguration
├── LICENSE                  MIT-Lizenz
└── README.md
```

---

## Lizenz

Dieses Projekt steht unter der [MIT-Lizenz](LICENSE).
