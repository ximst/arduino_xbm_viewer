#define CONFIG_HERSHEY

/*void
vector_rot_init(
  vector_rot_t * r,
  uint8_t theta
)
{
  r->theta = theta;
  r->sin_t = sin_lookup(theta);
  r->cos_t = cos_lookup(theta);
}
*/

uint8_t
vector_rot_x(
  const vector_rot_t * r,
  int8_t x,
  int8_t y
)
{
  int32_t x2 = x;
  int32_t w;
  if (r->theta != 0)
  {
    int32_t y2 = y;

    w = (r->scale * (x2 * r->cos_t + y2 * r->sin_t)) / (32 * 256);
  } else {
    w = (r->scale * x2) / 64;
  }
  
  return w + r->cx;
}

uint8_t
vector_rot_y(
  const vector_rot_t * r,
  int8_t x,
  int8_t y
)
{
  int32_t x2 = x;
  int32_t y2 = y;
  int32_t z;

  if (r->theta != 0)
    z = (r->scale * (y2 * r->cos_t - x2 * r->sin_t)) / (32 * 256);
  else
    z = (r->scale * y2) / 64;

  return z + r->cy;
}
  

static inline int8_t
scaling(
  int8_t d,
  uint8_t scale
)
{
  if (scale == 0)
    return d / 4;
  if (scale == 1)
    return (d * 2) / 3;
  if (scale == 2)
    return d;
  if (scale == 3)
    return (d * 3) / 2;
  if (scale == 4)
    return d * 2;
  return d;
}



static inline uint8_t
_draw_char(
  const uint8_t x,
  const uint8_t y,
  uint8_t c,
  const uint8_t scale
)
{
  uint8_t ox = x;
  uint8_t oy = y;
  uint8_t pen_down = 0;

  if (c < 0x20)
    return 0;

#ifdef CONFIG_HERSHEY
  const hershey_char_t * p = &hershey_simplex[c - 0x20];
  //const hershey_char_t * p = pgm_get_far_address(hershey_simplex[c - 0x20]);
  const uint8_t count = pgm_read_byte(&p->count);

 // Serial.println((unsigned long) p,HEX);  Serial.println((unsigned long) &hershey_simplex,HEX);

  for (uint8_t i = 0 ; i < count ; i++)
  {
    const int8_t px = pgm_read_byte(&p->points[i*2+0]);
    const int8_t py = pgm_read_byte(&p->points[i*2+1]);
    if (px == -1 && py == -1)
    {
      pen_down = 0;
      continue;
    }

    const uint8_t nx = x + scaling(px, scale);
    const uint8_t ny = y + scaling(py, scale);

    if (pen_down)
      line(ox, oy, nx, ny);

    pen_down = 1;
    ox = nx;
    oy = ny;
  }

  const uint8_t width = pgm_read_byte(&p->width);
  return scaling(width, scale);
#else
  if ('a' <= c && c <= 'z')
    c += 'A' - 'a';

  const asteroids_char_t * const p = &asteroids_font[c - 0x20];
  for (uint8_t i = 0 ; i < 8 ; i++)
  {
    const uint8_t xy = pgm_read_byte(&p->points[i]);
    if (xy == 0xFF)
      break;
    if (xy == 0xFE)
    {
      pen_down = 0;
      continue;
    }

    const int8_t px = ((xy >> 4) & 0xF) * 2;
    const int8_t py = ((xy >> 0) & 0xF) * 2;
    const uint8_t nx = x + scaling(px, scale);
    const uint8_t ny = y + scaling(py, scale);

    if (pen_down)
      line(ox, oy, nx, ny);

    pen_down = 1;
    ox = nx;
    oy = ny;
  }

  return scaling(20, scale);
#endif
}


uint8_t
draw_char_big(
  uint8_t x,
  uint8_t y,
  uint8_t c
)
{
  return _draw_char(x, y, c, 3);
}


uint8_t
draw_char_med(
  uint8_t x,
  uint8_t y,
  uint8_t c
)
{
  return _draw_char(x, y, c, 2);
}

uint8_t
draw_char_small(
  uint8_t x,
  uint8_t y,
  uint8_t c
)
{
  return _draw_char(x, y, c, 1);
}



/*
void
draw_char_rot(
  const vector_rot_t * const r,
  const int8_t x,
  const int8_t y,
  char c
)
{
  // we never use the original points since the lines always
  // start pen-up
  uint8_t ox = 0; // vector_rot_x(r, x, y);
  uint8_t oy = 0; // vector_rot_y(r, x, y);
  uint8_t pen_down = 0;

  if (c < 0x20)
    return;

  if ('a' <= c && c <= 'z')
    c += 'A' - 'a';

  const asteroids_char_t * const p = &asteroids_font[c - 0x20];
  for (uint8_t i = 0 ; i < 8 ; i++)
  {
    const uint8_t xy = pgm_read_byte(&p->points[i]);
    if (xy == 0xFF)
      break;
    if (xy == 0xFE)
    {
      pen_down = 0;
      continue;
    }

    const int8_t px = ((xy >> 4) & 0xF) * 2;
    const int8_t py = ((xy >> 0) & 0xF) * 2;

    const int8_t nx = x + px;
    const int8_t ny = y + py;
    const uint8_t rx = vector_rot_x(r, nx, ny);
    const uint8_t ry = vector_rot_y(r, nx, ny);

    if (pen_down)
      line(ox, oy, rx, ry);

    pen_down = 1;
    ox = rx;
    oy = ry;
  }
}
*/


