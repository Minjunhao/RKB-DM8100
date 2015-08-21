#ifndef _POWER_LED
#define _POWER_LED

#define POWER_MODE_NORMAL                 0     //switch = ON, >0.5W
#define POWER_MODE_12V_TRIG               1     //switch = 12V TRIG, <0.5W
#define POWER_MODE_SS_NORMAL              3     //signal sense mode
#define POWER_MODE_SS_QUICK               4     //CPU control waking the unit

#define SMPS_OFF         0
#define SMPS_ON          1

#define AMP_POWER_OFF    0
#define AMP_POWER_ON     1

#define MAIN_POWER_OFF   0
#define MAIN_POWER_ON    1
typedef struct{
  unsigned char power_mode;
  unsigned char pwr_check_timer;
  unsigned char pwr_off_delay_counter;
  unsigned char pwr_status;
  unsigned char SMPS_power_status;
  unsigned char amp_power_status;
  unsigned char amp_shut_down_temp;    //the temperature of shutting down the amp
  unsigned char smps_shut_down_temp;   //the temperature of shutting down the SMPS
} POWER_TYPE;

typedef struct{
  unsigned char ss_led_dim_counter;
  unsigned char pwr_led_dim_counter;
  unsigned char ss_led_mode;          //for signal sense mode
  unsigned char pwr_led_mode;         //for power indicate
} LED_TYPE;

#endif
