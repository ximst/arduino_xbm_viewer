#include "vector.h"
#include "dcf77.h"
#include <avr/pgmspace.h>

#define DAC_PORT PORTD
#define DAC_SEL_CHA_A PORTB &= ~0x02;
#define DAC_SEL_CHA_B PORTB |= 0x02;
#define DAC_WR_LO PORTB &= ~0x01;
#define DAC_WR_HI PORTB |= 0x01;
#define DAC_WRITE(value, channel) DAC_PORT = value; DAC_SEL_CHA_##channel; DAC_WR_LO; DAC_WR_HI;

#define MAX_ROWS 5
#define MAX_COLS 11

#define USE_DCF77 0
static char text[MAX_ROWS][MAX_COLS] = {
  { "BAKER" },
  { "HUGHES" },
  { "MAKER" },
  { "FAIRE" },
  { "   " }
/*
  { "012345678901234" },
  { "0123456789-=" },
  { "~!@#$%^&*\\|_" },
  { "+`[]{}()<>/?" },
*/
};

static String timeString = "00:00:00";

void setup() 
{

 Serial.begin(9600);   
 Serial.println("Hello world!");
 // set port direction to output
  DDRD |= B11111111;
  DDRB |= B00000011;

  DAC_WRITE(0, A);
  DAC_WRITE(0, B);  

#if USE_DCF77
  DCF77Init();
  timeString.toCharArray(text[4] , 9);
#endif
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
static unsigned char offset = 0;
static void draw_text(void)
{
  const uint8_t height = 40;

  uint8_t y = 256 - height;
  for (uint8_t row = 0 ; row < MAX_ROWS ; row++)
  {
    uint8_t x = 0;
    for (uint8_t col = 0 ; col < MAX_COLS ; col++)
    {
      draw_char_med(x + (offset % 32), y - (offset % 32), text[row][col]);
      x += 32;
    }
    y -= height;
  }
}


void loop() {

  static unsigned char throttle = 0;
  draw_text();
  throttle++;
  if ((throttle % 16) == 0)
  {
      offset++;
  }

#if USE_DCF77
  if ((throttle % 16) == 0)
  {
    // hh
    if (hh < 10)
    {
      timeString = "0";
    }
    else
    {
      timeString = "";
    }
    timeString += hh;

    // mm
    if (mm < 10)
    {
      timeString += ":0";
    }
    else
    {
      timeString += ":";
    }
    timeString += mm;

    // ss
    if (bufferPosition < 10)
    {
      timeString += ":0";
    }
    else
    {
      timeString += ":";
    }
    timeString += bufferPosition;

    timeString.toCharArray(text[4] , 9);
    
  if (bufferPosition != previousSecond) {
  //  Serial.print("Time: ");
  //  Serial.println(timeString);
  //  serialDumpTime();
    previousSecond = bufferPosition;
  }
  if (DCFSignalState != previousSignalState) {
    scanSignal();
    if (DCFSignalState) {
      digitalWrite(BLINKPIN, HIGH);
    } else {
      digitalWrite(BLINKPIN, LOW);
    }
    previousSignalState = DCFSignalState;
  }
  }
#endif
}
