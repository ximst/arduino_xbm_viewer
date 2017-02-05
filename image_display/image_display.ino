#include "TimerOne.h" 
#include "vector.h"
#include <avr/pgmspace.h>

#define DAC_PORT PORTD
#define DAC_SEL_CHA_A PORTB &= ~0x02;
#define DAC_SEL_CHA_B PORTB |= 0x02;
#define DAC_WR_LO PORTB &= ~0x01;
#define DAC_WR_HI PORTB |= 0x01;
#define DAC_WRITE(value, channel) DAC_PORT = value; DAC_SEL_CHA_##channel; DAC_WR_LO; DAC_WR_HI;


void setup() 
{

 Serial.begin(9600);   
 Serial.println("Hello world!");

 // set port direction to output
  DDRD |= B11111111;
  DDRB |= B00000011;

  DAC_WRITE(0, A);
  DAC_WRITE(0, B);  
}

static void
moveto(
  uint8_t x,
  uint8_t y
)
{
  DAC_WRITE(x, A); 
  DAC_WRITE(y, B); 
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
    DAC_WRITE(y0++, B); 
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
    DAC_WRITE(x0++, A); 
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
      DAC_WRITE((x0 += sx), A); 
    }
    if (e2 < dx)
    {
      err = err + dx;
      DAC_WRITE((y0 += sy), B); 
    }
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
   draw_text();
}
