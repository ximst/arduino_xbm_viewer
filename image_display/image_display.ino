#include "TimerOne.h" 
#include "vector.h"
#include <avr/pgmspace.h>

#define DAC_PORT PORTD
#define DAC_SEL_CHA_A PORTB &= ~0x02;
#define DAC_SEL_CHA_B PORTB |= 0x02;
#define DAC_WR_LO PORTB &= ~0x01;
#define DAC_WR_HI PORTB |= 0x01;

static uint8_t value1 = 0;
static uint8_t value2 = 0;
static uint8_t value3 = 0;

void setup() 
{

Serial.begin(9600);   

 Serial.println("Hello world!");

#if 0 
 Timer1.initialize(1);
 Timer1.setPeriod(1);
 Timer1.attachInterrupt(blankScreen);
#endif
 
  // set port direction to output
  DDRD |= B11111111;
  DDRB |= B00000011;
  DAC_SEL_CHA_A;
  DAC_WR_HI;

  // clear both DAC channels
  PORTD = 0;
  DAC_SEL_CHA_A;
  DAC_WR_LO;
  DAC_WR_HI;
  
  DAC_SEL_CHA_B;
  DAC_WR_LO;
  DAC_WR_HI;
}

static void
moveto(
  uint8_t x,
  uint8_t y
)
{
    PORTD = x;
    DAC_SEL_CHA_A;
    DAC_WR_LO;
    DAC_WR_HI;

    PORTD = y;
    DAC_SEL_CHA_B;
    DAC_WR_LO;
    DAC_WR_HI;
}

void
line_vert(
  uint8_t x0,
  uint8_t y0,
  uint8_t w
)
{
  moveto(x0, y0);
  for (uint8_t i = 0 ; i < w ; i++)
  {
    PORTD = y0++;
    DAC_SEL_CHA_B;
    DAC_WR_LO;
    DAC_WR_HI;
    
  }
}

void
line_horiz(
  uint8_t x0,
  uint8_t y0,
  uint8_t h
)
{
  moveto(x0, y0);
  for (uint8_t i = 0 ; i < h ; i++)
  {
    PORTD = x0++;
    DAC_SEL_CHA_A;
    DAC_WR_LO;
    DAC_WR_HI;
  }
}


void
line(
  uint8_t x0,
  uint8_t y0,
  uint8_t x1,
  uint8_t y1
)
{
  int dx;
  int dy;
  int sx;
  int sy;

  if (x0 == x1)
  {
    if (y0 < y1)
      line_vert(x0, y0, y1 - y0);
    else
      line_vert(x0, y1, y0 - y1);
    return;
  }

  if (y0 == y1)
  {
    if (x0 < x1)
      line_horiz(x0, y0, x1 - x0);
    else
      line_horiz(x1, y0, x0 - x1);
    return;
  }

  if (x0 <= x1)
  {
    dx = x1 - x0;
    sx = 1;
  } else {
    dx = x0 - x1;
    sx = -1;
  }

  if (y0 <= y1)
  {
    dy = y1 - y0;
    sy = 1;
  } else {
    dy = y0 - y1;
    sy = -1;
  }

  int err = dx - dy;

  moveto(x0, y0);

  while (1)
  {
    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 > -dy)
    {
      err = err - dy;
      PORTD = (x0 += sx);
      DAC_SEL_CHA_A;
      DAC_WR_LO;
      DAC_WR_HI;

    }
    if (e2 < dx)
    {
      err = err + dx;
      PORTD = (y0 += sy);
      DAC_SEL_CHA_B;
      DAC_WR_LO;
      DAC_WR_HI;
    }

  }
}


void blankScreen() 
{
  // write channel A

  value1 += 2;
  
  PORTD = value1;
  DAC_SEL_CHA_A;
  DAC_WR_LO;
  DAC_WR_HI;
  
  if (!value1)
  {
    // write channel B
    value2+=2;
    
    PORTD = value2;
    DAC_SEL_CHA_B;
    DAC_WR_LO;
    DAC_WR_HI;
  }
}

#define MAX_ROWS 3
#define MAX_COLS 11

static char text[MAX_ROWS][MAX_COLS] = {
  { "STAR" },
  { "WARS" },
  { "7" }
/*
  { "012345678901234" },
  { "0123456789-=" },
  { "~!@#$%^&*\\|_" },
  { "+`[]{}()<>/?" },
*/
};


static void draw_text(void)
{
  const uint8_t height = 40;

  uint8_t y = 256 - height;
  for (uint8_t row = 0 ; row < MAX_ROWS ; row++)
  {
    uint8_t x = 0;
    for (uint8_t col = 0 ; col < MAX_COLS ; col++)
    {
      draw_char_med(x, y, text[row][col]);
      x += 32;
    }

    y -= height;
  }
}

void loop() {
  static uint8_t hight = 20;

  //line(0, 255 - hight, 255, hight);  hight++;

  //draw_char_big(100,100,76);

  //line(0,20,0,120);

   draw_text();

 // delay(1000);
}
