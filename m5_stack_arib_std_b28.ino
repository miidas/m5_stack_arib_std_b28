#include "M5Stack.h"

typedef struct {
  uint8_t y, pb, pr;
} YPbPr;

void setup()
{
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.setBrightness(255);

  // ARIB STD-B28
  // 320 x 240 -> 320 x 180
  // a = 320
  // b = 180
  // (3/4)a = 240
  // (1/12)b = 15
  // (3/12)b = 45
  // (7/12)b = 105
  // d = 40

  M5.Lcd.startWrite();
  M5.Lcd.fillScreen(TFT_BLACK);

  uint16_t offset_x, offset_y;

  // Pattern 1
  uint8_t pl_w[9] = {40, 35, 34, 34, 34, 34, 34, 35, 40};
  uint8_t p1_h = 105; // (7/12)b
  YPbPr p1_color[9] = {
    {104, 128, 128}, // Gray  40%
    {180, 128, 128}, // White 75%
    {168,  44, 136}, // Yellow
    {145, 147,  44}, // Cyan
    {133,  63,  52}, // Green
    { 63, 193, 204}, // Magenta
    { 51, 109, 212}, // Red
    { 28, 212, 120}, // Blue
    {104, 128, 128}, // Gray  40%
  };

  offset_x = 0;
  offset_y = 0;

  for (auto i = 0; i < 9; i++)
  {
    M5.Lcd.fillRect(offset_x, offset_y, pl_w[i], p1_h, YPbPrToColor565(p1_color[i]));
    offset_x += pl_w[i];
  }


  // Pattern 2
  uint8_t p2_w[4] = {40, 35, 205, 40};
  uint8_t p2_h = 15; // (1/12)b
  YPbPr p2_color[4] = {
    {188, 154,  16}, // Cyan  100%
    {235, 128, 128}, // White 100%
    {180, 128, 128}, // White  75%
    { 32, 240, 118}, // Blue  100%
  };

  offset_x = 0;
  offset_y += p1_h;

  for (auto i = 0; i < 4; i++)
  {
    M5.Lcd.fillRect(offset_x, offset_y, p2_w[i], p2_h, YPbPrToColor565(p2_color[i]));
    offset_x += p2_w[i];
  }

  // Pattern 3
  uint8_t p3_w = 40;
  uint8_t p3_h = 15; // (1/12)b
  YPbPr p3_color[4] = {
    {219,  16, 138}, // Yellow 100%
    { 16, 128, 128}, // Ramp     0%
    {235, 128, 128}, // Ramp   100%
    { 63, 102, 240}, // Red    100%
  };

  offset_y += p2_h;

  M5.Lcd.fillRect(0, offset_y, p3_w, p3_h, YPbPrToColor565(p3_color[0]));
  M5.Lcd.fillRect(40, offset_y, p3_w, p3_h, YPbPrToColor565(p3_color[1]));
  M5.Lcd.fillRect(240, offset_y, p3_w, p3_h, YPbPrToColor565(p3_color[2]));
  M5.Lcd.fillRect(280, offset_y, p3_w, p3_h, YPbPrToColor565(p3_color[3]));

  // Ramp
  // 適当なので多分変
  YPbPr p3_ramp = {16, 128, 128};
  float p3_ramp_y = 16;
  for (auto i = 80; i < 240; i++)
  {
    M5.Lcd.fillRect(i, offset_y, 1, p2_h, YPbPrToColor565(p3_ramp));
    p3_ramp_y += 1.375;
    p3_ramp.y = p3_ramp_y;
  }

  // Pattern 4 2 tarinai
  uint8_t p4_w[11] = {40, 52, 68, 29, 11, 11, 11, 12, 11, 35, 40};
  uint8_t p4_h = 45; // (3/12)b
  YPbPr p4_color[11] = {
    { 49, 128, 128}, // Gray   15%
    { 16, 128, 128}, // Black   0%
    {235, 128, 128}, // White 100%
    { 16, 128, 128}, // Black   0%
    { 12, 128, 128}, //        -2%
    { 16, 128, 128}, //         0
    { 20, 128, 128}, //        +2%
    { 16, 128, 128}, //         0
    { 25, 128, 128}, //        +4%
    { 16, 128, 128}, // Black   0%
    { 49, 128, 128}, // Gray   15%
  };

  offset_x = 0;
  offset_y += p3_h;

  for (auto i = 0; i < 4; i++)
  {
    M5.Lcd.fillRect(offset_x, offset_y, p4_w[i], p4_h, YPbPrToColor565(p4_color[i]));
    offset_x += p4_w[i];
  }

  M5.Lcd.endWrite();
}

void loop()
{
  M5.update();
  if (M5.BtnB.wasPressed()) {
    ESP.restart();
  }
  delay(100);
}

uint16_t YPbPrToColor565(YPbPr color)
{
  // BT.709 HDTV
  // R = 1.164(Y-16)                 + 1.793(Cr-128)
  // G = 1.164(Y-16) - 0.213(Cb-128) - 0.533(Cr-128)
  // B = 1.164(Y-16) + 2.112(Cb-128)
  // http://licheng.sakura.ne.jp/hatena6/rgbyc.html
  uint8_t r, g, b;
  r = constrain(1.164 * (color.y - 16) + 1.793 * (color.pr - 128), 0, 255);
  g = constrain(1.164 * (color.y - 16) - 0.213 * (color.pb - 128) - 0.533 * (color.pr - 128), 0, 255);
  b = constrain(1.164 * (color.y - 16) + 2.112 * (color.pb - 128), 0, 255);
  return M5.Lcd.color565(r, g, b);
}
