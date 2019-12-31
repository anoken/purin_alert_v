//## Copyright (c) 2019 aNoken
// Licensed under the MIT license
// Based on https://github.com/meganetaaan/m5stack-avatar

#include "avator.h"

float m5_stack_rd_global = 0.0;

void rotation(int16_t x0, int16_t y0, int16_t &x1, int16_t &y1) {
  x0 = x0 - 160;
  y0 = y0 - 120;
  x1 = x0 * cos(m5_stack_rd_global) - y0 * sin(m5_stack_rd_global);
  y1 = x0 * sin(m5_stack_rd_global) + y0 * cos(m5_stack_rd_global);
  x1 = x1 + 160;
  y1 = y1 + 120;

  if (x1 < 0)x1 = 0;
  if (y1 < 0)y1 = 0;
  if (x1 > 320)x1 = 320;
  if (y1 > 240)y1 = 240;
}

void M5_Lcd_fillRect2(TFT_eSPI *spi, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int16_t x0 = x;
  int16_t y0 = y;
  int16_t x1 = x + w;
  int16_t y1 = y;
  int16_t x2 = x;
  int16_t y2 = y + h;
  int16_t x3 = x + w;
  int16_t y3 = y + h;

  int16_t xx0;
  int16_t yy0;
  int16_t xx1;
  int16_t yy1;
  int16_t xx2;
  int16_t yy2;
  int16_t xx3;
  int16_t yy3;

  rotation(x0, y0, xx0, yy0);
  rotation(x1, y1, xx1, yy1);
  rotation(x2, y2, xx2, yy2);
  rotation(x3, y3, xx3, yy3);

  spi->fillTriangle( xx0, yy0,  xx1,  yy1,  xx3,  yy3,  color);
  spi->fillTriangle( xx0, yy0,  xx2,  yy2,  xx3,  yy3,  color);
}
DrawContext::DrawContext() {
}
DrawContext::DrawContext(Expression expression, float breath) {
  this->expression = expression;
  this->breath = breath;
}

Expression DrawContext::getExpression() {
  return expression;
}

float DrawContext::getBreath() {
  return breath;
}

Mouth::Mouth(void) {
}

Mouth::Mouth(int x, int y, int minWidth, int maxWidth, int minHeight, int maxHeight, uint32_t primaryColor, uint32_t secondaryColor) {
  this->x = x;
  this->y = y;
  this->minWidth = minWidth;
  this->maxWidth = maxWidth;
  this->minHeight = minHeight;
  this->maxHeight = maxHeight;
  this->primaryColor = primaryColor;
  this->secondaryColor = secondaryColor;
  this->openRatio = 0;
}
void Mouth::_draw(TFT_eSPI *spi, int x, int y, int w, int h) {
  int16_t xx0, yy0;
  int16_t xx1, yy1;
  int16_t xx2, yy2;
  int16_t xx3, yy3;
  int16_t x0 = x;
  int16_t y0 = y;
  int16_t x1 = x + w;
  int16_t y1 = y;
  int16_t x2 = x;
  int16_t y2 = y + h;
  int16_t x3 = x + w;
  int16_t y3 = y + h;

  rotation(x0, y0, xx0, yy0);
  rotation(x1, y1, xx1, yy1);
  rotation(x2, y2, xx2, yy2);
  rotation(x3, y3, xx3, yy3);
  spi->fillTriangle(xx0, yy0, xx1, yy1, xx3, yy3, primaryColor);
  spi->fillTriangle(xx0, yy0, xx2, yy2, xx3, yy3, primaryColor);
  int plus = 0;
  int xx = random(5);
  int yy = random(10);
  x0 = 190;
  y0 = 70;
  x1 = 190;
  y1 = 50;
  x2 = 280 + xx;
  y2 = 25 + yy;
  if (!anger_flg) {
    y0 = 50;
    y1 = 30;
  }
  rotation(x0, y0, xx0, yy0);
  rotation(x1, y1, xx1, yy1);
  rotation(x2, y2, xx2, yy2);
  spi->fillTriangle(xx0, yy0, xx1, yy1, xx2, yy2, primaryColor);

  x0 = 130;
  y0 = 70;
  x1 = 130;
  y1 = 50;
  x2 = 40 + xx;
  y2 = 25 + yy;

  if (!anger_flg) {
    y0 = 50;
    y1 = 30;
  }

  rotation(x0, y0, xx0, yy0);
  rotation(x1, y1, xx1, yy1);
  rotation(x2, y2, xx2, yy2);
  spi->fillTriangle(xx0, yy0, xx1, yy1, xx2, yy2, primaryColor);
}
void Mouth::_drawCircle(TFT_eSPI *spi, int x, int y, int w, int h) {
  spi->fillRect(x, y, w, h, primaryColor);
}

void Mouth::_drawRect(TFT_eSPI *spi, int x, int y, int w, int h) {
  int16_t xx0;
  int16_t yy0;
  rotation(x, y, xx0, yy0);
  M5_Lcd_fillRect2(spi, xx0, yy0, w, h, primaryColor);
}

void Mouth::_drawTriangle(TFT_eSPI *spi, int x0, int y0, int x1, int y1, int x2, int y2) {
  spi->fillTriangle(x0, y0, x1, y1, x2, y2, primaryColor);
}

void Mouth::setOpenRatio(float ratio){
  openRatio = ratio;
}

void Mouth::draw(TFT_eSprite *spi, DrawContext ctx){
  float breath = min(1.0, ctx.getBreath());
  int h = minHeight + (maxHeight - minHeight) * openRatio;
  int w = minWidth + (maxWidth - minWidth) * (1 - openRatio);
  int x = this->x - w / 2;
  int y = this->y - h / 2 + breath * 2;
  _draw(spi, x, y, w, h);
}

Eye::Eye(void){
}
Eye::Eye(int x, int y, int r, bool isLeft, uint32_t primaryColor, uint32_t secondaryColor){
  this->openRatio = 1;
  this->x = x;
  this->y = y;
  this->r = r;
  this->isLeft = isLeft;
  this->offsetX = 0;
  this->offsetY = 0;
  this->primaryColor = primaryColor;
  this->secondaryColor = secondaryColor;
}
void Eye::drawCircle(TFT_eSPI *spi, int x, int y, int r) {
  int16_t xx0;
  int16_t yy0;
  rotation(x, y, xx0, yy0);
  spi->fillCircle(xx0, yy0, r, primaryColor);
}

void Eye::drawRect(TFT_eSPI *spi, int x, int y, int w, int h) {
  int16_t xx0;
  int16_t yy0;
  rotation(x, y, xx0, yy0);
  M5_Lcd_fillRect2(spi, xx0, yy0, w, h, primaryColor);
}

void Eye::draw(TFT_eSprite *spi, DrawContext ctx) {
  Expression exp = ctx.getExpression();
  float breath = min(1.0, ctx.getBreath());
  if (openRatio > 0)  {
    drawCircle(spi, x + offsetX, y + offsetY + breath * 3, r);
    // TODO: Refactor
    if (exp == Angry || exp == Sad) {
      int x0, y0, x1, y1, x2, y2;
      x0 = x + offsetX - r * 1.1;
      y0 = y + offsetY - r * 1.1 + breath * 3;
      x1 = x0 + r * 2 * 1.1;
      y1 = y0;
      x2 = !isLeft != !(exp == Sad) ? x0 : x1;
      y2 = y0 + r * 1.1;

      int16_t xx0, yy0;
      int16_t xx1, yy1;
      int16_t xx2, yy2;
      rotation(x0, y0, xx0, yy0);
      rotation(x1, y1, xx1, yy1);
      rotation(x2, y2, xx2, yy2);
      spi->fillTriangle(xx0, yy0, xx1, yy1, xx2, yy2, secondaryColor);
    }
  }
  else  {
    int x1 = x + offsetX;
    int y1 = y + offsetY + breath * 3 ;
    int w = r * 2;
    int h = 4;
    M5_Lcd_fillRect2(spi, x1 - w / 2, y1, w, h, primaryColor);
  }
}

void Eye::setOpenRatio(float ratio){
  this->openRatio = ratio;
}
void Eye::setOffset(int offsetX, int offsetY){
  this->offsetX = offsetX;
  this->offsetY = offsetY;
}
Avator::Avator() {
  PRIMARY_COLOR = TFT_BLACK;
  SECONDARY_COLOR = TFT_CYAN;
  M5Stack_rad = 0;
  this->mouth = Mouth(163, 188 , 50, 100, 4, 60, PRIMARY_COLOR, SECONDARY_COLOR);
  this->eyeR = Eye(90, 93 , 25, false, PRIMARY_COLOR, SECONDARY_COLOR);
  this->eyeL = Eye(230, 96 , 25, true, PRIMARY_COLOR, SECONDARY_COLOR);

  this->drawContext = DrawContext(expression, breath);
  this->avatorSprite = new TFT_eSprite(&M5.Lcd);
  avatorSprite->setColorDepth(8);
  avatorSprite->createSprite(320, 240);
  avatorSprite->setBitmapColor(PRIMARY_COLOR, SECONDARY_COLOR);
  expression = Neutral;
  breath = 0.0;
}
void Avator::openMouth(int percent) {
  float f = percent / 10.0;
  mouth.setOpenRatio(f);
  draw();
}

void Avator::setMouthOpen(float f) {
  mouth.setOpenRatio(f);
}

void Avator::openEye(boolean isOpen){
  float ratio = isOpen ? 1 : 0;
  eyeR.setOpenRatio(ratio);
  eyeL.setOpenRatio(ratio);
  draw();
}

void Avator::setEyeOpen(float f){
  eyeR.setOpenRatio(f);
  eyeL.setOpenRatio(f);
}

void Avator::setExpression(Expression expression){
  this->expression = expression;
}

void Avator::init(){
  anger_flg = 0;
  area_h = 120;
  value = 0;
  thresh = 0;
}

void Avator::setBreath(float breath){
  this->breath = breath;
}

void Avator::setGaze(float vertical, float horizontal){
  int v = floor(4 * vertical);
  int h = floor(4 * horizontal);
  eyeL.setOffset(v, h);
  eyeR.setOffset(v, h);
}


void Avator::draw() {

  mouth.anger_flg = anger_flg;

  if (abs(m5_stack_rd_global * 1000 - M5Stack_rad * 1000) < 100) {
    m5_stack_rd_global = M5Stack_rad;
  }
  else if (m5_stack_rd_global < M5Stack_rad) m5_stack_rd_global += 0.1;
  else m5_stack_rd_global -= 0.1;

  avatorSprite->fillSprite(SECONDARY_COLOR);
  avatorSprite->fillRect(0, 0, 320, area_h, TFT_PINK);

  mouth.primaryColor = PRIMARY_COLOR;
  mouth.secondaryColor = SECONDARY_COLOR;
  eyeR.primaryColor = PRIMARY_COLOR;
  eyeR.secondaryColor = SECONDARY_COLOR;
  eyeL.primaryColor = PRIMARY_COLOR;
  eyeL.secondaryColor = SECONDARY_COLOR;

  this->drawContext = DrawContext(expression, breath);
  mouth.draw(avatorSprite, drawContext);
  eyeR.draw(avatorSprite, drawContext);
  eyeL.draw(avatorSprite, drawContext);
  if (speak_flg)  avatorSprite->fillCircle(30, 10, 10, BLUE);

  if (anger_flg) {
    int x = random(3);
    int y = random(5) - 10;
    if (y < -10) y = -10;
    avatorSprite->fillRect(266 + x, 10 + y, 5, 24, RED);
    avatorSprite->fillRect(242 + x, 34 + y, 29, 5, RED);
    avatorSprite->fillRect(266 + 15 + x, 10 + y, 5, 24, RED);
    avatorSprite->fillRect(266 + 15 + x, 34 + y, 24, 5, RED);
    avatorSprite->fillRect(266 + 15 + x, 34 + 15 + y, 24, 5, RED);
    avatorSprite->fillRect(266 + 15 + x, 34 + 15 + y, 5, 20, RED);
    avatorSprite->fillRect(242 + x, 34 + 15 + y, 24, 5, RED);
    avatorSprite->fillRect(266 + x, 34 + 15 + y, 5, 20, RED);
  }
  String value_str = String(value);
  String thresh_str = String(thresh);
  avatorSprite->setTextColor(TFT_WHITE, TFT_BLACK);
  String str_str = value_str + "/" + thresh_str;
  avatorSprite->drawString(str_str, 200, 220, 4);
  avatorSprite->pushSprite(0, 0);
}
