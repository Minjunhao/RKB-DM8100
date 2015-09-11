#ifndef _SIGNAL_SENSE
#define _SIGNAL_SENSE

#define PROTECT_FUNCTION_OFF 0
#define PROTECT_FUNCTION_ON  1
#define AMP_PROTECT_OFF      1
#define AMP_PROTECT_ON       0
#define AMP_CHECK_TIME       4
#define AMP_CHECK_PERIOD     5         //50ms


#define SIGNAL_ON            1

#define TIMER_SIGNAL_OFF_TIME    (10*60*1000*1000)  

//for signal sense
typedef struct{
  unsigned char mode_on;                   //signal sense on
  unsigned char chk_timer;                 //signal pin check timer
  //unsigned char reset_counter;
  unsigned char noise_counter;             //signal pin noise check when standby
  unsigned long no_signal_counter;         //when SS on, check the signal pin
} SS_TYPE;

//for amp protect
typedef struct{
  //unsigned char status;
  unsigned char protect_func_on;  //switch for the protect function
  unsigned char check_timer;      //protect check timer if protect_func_on=PROTECT_FUNCTION_ON
  unsigned char protect_status;   //amp protect status
} AMP_TYPE;

extern AMP_TYPE   amp_status;
void amp_protection_on(void);
void amp_protection_off(void);
void signal_sense_check(void);
void amp_status_initial(void);
void signal_sense_initial(void);
void amp_status_initial(void);
#endif
