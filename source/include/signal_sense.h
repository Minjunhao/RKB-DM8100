#ifndef _SIGNAL_SENSE
#define _SIGNAL_SENSE

#define AMP_PROTECT_OFF      1
#define AMP_PROTECT_ON       0
#define AMP_CHECK_PERIOD     5         //50ms

typedef struct{
  unsigned char mode_on;
  unsigned char chk_timer;
  unsigned char reset_counter;
  unsigned char low_counter;
  unsigned char noise_counter;
} SS_TYPE;

typedef struct{
  //unsigned char status;
  unsigned char check_timer;
  unsigned char protect_status;
} AMP_TYPE;
#endif#
