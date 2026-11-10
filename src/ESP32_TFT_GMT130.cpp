// ============================================================================
//  ESP32_TFT_GMT130 - Testprogramm fuer das 1.3" ST7789 Display (240x240)
//  Board:   ELEGOO ESP32 (30 Pins, ESP-WROOM-32)
//  Library: TFT_eSPI (Konfiguration in platformio.ini -> build_flags)
//
//  Verdrahtung:
//    Display  ESP32
//    GND      GND
//    VCC      3V3
//    SCL      D18  (SPI SCK)
//    SDA      D23  (SPI MOSI)
//    RES      D26
//    DC       D27
//    BLK      D25  (Hintergrundbeleuchtung)
//
//  Ablauf: Farbtest -> Texttest -> Grafiktest -> Animation (Endlosschleife)
//  Debug-Ausgaben auf dem seriellen Monitor (115200 Baud).
// ============================================================================

#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Falls die Farben invertiert erscheinen (Weiss wird schwarz, Rot wird cyan),
// diesen Wert umschalten (true <-> false).
static const bool INVERT_COLORS = true;

// ---------------------------------------------------------------------------
// Hilfsfunktionen
// ---------------------------------------------------------------------------

static void centerText(const char *text, int y, uint16_t fg, uint16_t bg, uint8_t font)
{
  tft.setTextColor(fg, bg);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(text, tft.width() / 2, y, font);
}

// Fuellt den Bildschirm mit einer Farbe und schreibt deren Namen in die Mitte
static void colorScreen(uint16_t color, const char *name, uint16_t textColor)
{
  tft.fillScreen(color);
  centerText(name, tft.height() / 2, textColor, color, 4);
  Serial.printf("  Farbe: %s\n", name);
  delay(800);
}

// ---------------------------------------------------------------------------
// Test 1: Vollflaechige Farben + Farbverlauf
// ---------------------------------------------------------------------------

static void testColors()
{
  Serial.println("Test 1: Farben");
  colorScreen(TFT_RED,   "ROT",     TFT_WHITE);
  colorScreen(TFT_GREEN, "GRUEN",   TFT_BLACK);
  colorScreen(TFT_BLUE,  "BLAU",    TFT_WHITE);
  colorScreen(TFT_WHITE, "WEISS",   TFT_BLACK);
  colorScreen(TFT_BLACK, "SCHWARZ", TFT_WHITE);

  // Regenbogen-Verlauf ueber die volle Breite (Farbrad)
  for (int x = 0; x < tft.width(); x++) {
    uint8_t h = (x * 255) / tft.width();
    uint8_t region = h / 43;
    uint8_t rem = (h - region * 43) * 6;
    uint8_t q = 255 - rem, t = rem;
    uint8_t r, g, b;
    switch (region) {
      case 0:  r = 255; g = t;   b = 0;   break;
      case 1:  r = q;   g = 255; b = 0;   break;
      case 2:  r = 0;   g = 255; b = t;   break;
      case 3:  r = 0;   g = q;   b = 255; break;
      case 4:  r = t;   g = 0;   b = 255; break;
      default: r = 255; g = 0;   b = q;   break;
    }
    tft.drawFastVLine(x, 0, tft.height(), tft.color565(r, g, b));
  }
  centerText("Regenbogen", tft.height() / 2, TFT_WHITE, TFT_BLACK, 4);
  delay(1500);
}

// ---------------------------------------------------------------------------
// Test 2: Text in verschiedenen Schriften
// ---------------------------------------------------------------------------

static void testText()
{
  Serial.println("Test 2: Text");
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Font 1: GLCD 6x8", 5, 5, 1);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Font 2: 16px", 5, 20, 2);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("Font 4: 26px", 5, 42, 4);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("12:34", 5, 75, 7);   // 7-Segment-Schrift (nur Ziffern)

  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString("FreeSans 12pt", 5, 135);
  tft.setTextFont(1);

  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("GMT130  ST7789  240x240", 5, 170, 2);
  tft.drawString("ESP32 + TFT_eSPI", 5, 190, 2);

  delay(3000);
}

// ---------------------------------------------------------------------------
// Test 3: Grafikprimitive
// ---------------------------------------------------------------------------

static void testGraphics()
{
  Serial.println("Test 3: Grafik");
  const int w = tft.width(), h = tft.height();

  // Linienfaecher aus allen vier Ecken
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < w; i += 12) {
    tft.drawLine(0, 0, i, h - 1, TFT_YELLOW);
    tft.drawLine(w - 1, 0, i, h - 1, TFT_CYAN);
    tft.drawLine(0, h - 1, i, 0, TFT_MAGENTA);
    tft.drawLine(w - 1, h - 1, i, 0, TFT_GREEN);
  }
  delay(1200);

  // Rahmen am Rand -> zeigt, ob das ganze Panel genutzt wird (kein Versatz)
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, w, h, TFT_RED);
  tft.drawRect(2, 2, w - 4, h - 4, TFT_WHITE);
  centerText("Randtest", h / 2 - 15, TFT_WHITE, TFT_BLACK, 4);
  centerText("roter Rahmen ringsum", h / 2 + 15, TFT_LIGHTGREY, TFT_BLACK, 2);
  delay(2000);

  // Gefuellte und leere Formen
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(10, 10, 60, 60, TFT_RED);
  tft.drawRect(90, 10, 60, 60, TFT_GREEN);
  tft.fillRoundRect(170, 10, 60, 60, 12, TFT_BLUE);
  tft.fillCircle(40, 120, 30, TFT_YELLOW);
  tft.drawCircle(120, 120, 30, TFT_CYAN);
  tft.fillEllipse(200, 120, 30, 18, TFT_MAGENTA);
  tft.fillTriangle(10, 230, 70, 230, 40, 170, TFT_ORANGE);
  tft.drawTriangle(90, 230, 150, 230, 120, 170, TFT_WHITE);
  tft.fillTriangle(170, 230, 230, 230, 200, 170, TFT_GREENYELLOW);
  delay(2500);
}

// ---------------------------------------------------------------------------
// Test 4: Animation (springender Ball + FPS-Anzeige), laeuft in loop()
// ---------------------------------------------------------------------------

static const int BALL_R = 12;
static const int INFO_H = 24;   // Hoehe der Infozeile oben

static float    ballX = 120, ballY = 120, velX = 2.7f, velY = 1.9f;
static int      lastX = 120, lastY = 120;
static uint32_t frames = 0, fpsTimer = 0;
static uint16_t ballColor = TFT_RED;

static void startAnimation()
{
  Serial.println("Test 4: Animation (Endlosschleife)");
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, tft.width(), INFO_H, TFT_NAVY);
  tft.drawFastHLine(0, INFO_H, tft.width(), TFT_WHITE);
  fpsTimer = millis();
}

static void animationStep()
{
  const int w = tft.width(), h = tft.height();

  ballX += velX;
  ballY += velY;

  bool bounced = false;
  if (ballX < BALL_R)              { ballX = BALL_R;              velX = -velX; bounced = true; }
  if (ballX > w - 1 - BALL_R)      { ballX = w - 1 - BALL_R;      velX = -velX; bounced = true; }
  if (ballY < INFO_H + 1 + BALL_R) { ballY = INFO_H + 1 + BALL_R; velY = -velY; bounced = true; }
  if (ballY > h - 1 - BALL_R)      { ballY = h - 1 - BALL_R;      velY = -velY; bounced = true; }
  if (bounced) ballColor = tft.color565(random(64, 256), random(64, 256), random(64, 256));

  // Alten Ball loeschen, neuen zeichnen
  tft.fillCircle(lastX, lastY, BALL_R, TFT_BLACK);
  lastX = (int)ballX;
  lastY = (int)ballY;
  tft.fillCircle(lastX, lastY, BALL_R, ballColor);

  // FPS einmal pro Sekunde anzeigen
  frames++;
  uint32_t now = millis();
  if (now - fpsTimer >= 1000) {
    char buf[32];
    snprintf(buf, sizeof(buf), "FPS: %lu   Up: %lus  ",
             (unsigned long)frames, (unsigned long)(now / 1000));
    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_NAVY);
    tft.drawString(buf, 6, INFO_H / 2, 2);
    Serial.println(buf);
    frames = 0;
    fpsTimer = now;
  }
}

// ---------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("=== ESP32 + GMT130 (ST7789 240x240) Test ===");

  tft.init();                 // schaltet auch die Hintergrundbeleuchtung (TFT_BL) ein
  tft.setRotation(0);         // 0..3 -> 0/90/180/270 Grad
  tft.invertDisplay(INVERT_COLORS);
  tft.fillScreen(TFT_BLACK);

  Serial.printf("Display initialisiert: %d x %d\n", tft.width(), tft.height());

  testColors();
  testText();
  testGraphics();
  startAnimation();
}

void loop()
{
  animationStep();
  delay(10);
}
