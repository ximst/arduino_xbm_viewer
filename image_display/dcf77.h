#ifndef DCF77_H
#define DCF77_H
/**
 * Where is the DCF receiver connected?
 */
#define DCF77PIN 10
/**
 * Where is the LED connected?
 */
#define BLINKPIN 13
/**
 * Turn debugging on or off
 */
#undef DCF_DEBUG
/**
 * Number of milliseconds to elapse before we assume a "1",
 * if we receive a falling flank before - its a 0.
 */
#define DCF_split_millis 140
/**
 * There is no signal in second 59 - detect the beginning of 
 * a new minute.
 */
#define DCF_sync_millis 1200
/**
 * Definitions for the timer interrupt 2 handler:
 * The Arduino runs at 16 Mhz, we use a prescaler of 64 -> We need to 
 * initialize the counter with 6. This way, we have 1000 interrupts per second.
 * We use tick_counter to count the interrupts.
 */
#define INIT_TIMER_COUNT 6
#define RESET_TIMER2 TCNT2 = INIT_TIMER_COUNT
int tick_counter = 0;
int TIMSK;
int TCCR2;
int OCIE2;
/**
 * DCF time format struct
 */
struct DCF77Buffer {
  unsigned long long prefix  :21;
  unsigned long long Min  :7; // minutes
  unsigned long long P1   :1; // parity minutes
  unsigned long long Hour :6; // hours
  unsigned long long P2   :1; // parity hours
  unsigned long long Day  :6; // day
  unsigned long long Weekday  :3; // day of week
  unsigned long long Month  :5; // month
  unsigned long long Year :8; // year (5 -> 2005)
  unsigned long long P3   :1; // parity
};
struct {
  unsigned char parity_flag :1;
  unsigned char parity_min    :1;
  unsigned char parity_hour :1;
  unsigned char parity_date :1;
} flags;
/**
 * Clock variables 
 */
volatile unsigned char DCFSignalState = 0;  
unsigned char previousSignalState;
int previousFlankTime;
int bufferPosition;
unsigned long long dcf_rx_buffer;
/**
 * time vars: the time is stored here!
 */
volatile unsigned char ss;
volatile unsigned char mm;
volatile unsigned char hh;
volatile unsigned char day;
volatile unsigned char mon;
volatile unsigned int year;
    

/**
 * used in main loop: detect a new second...
 */
unsigned char previousSecond;

void DCF77Init(void);
void serialDumpTime(void);
#endif
