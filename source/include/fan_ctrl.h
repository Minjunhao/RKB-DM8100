#ifndef _FAN_CTRL
#define _FAN_CTRL
#define FAN_OFF                   0
#define FAN_ON                    1

#define FAN_LS_OFF_DEGREE         25
#define FAN_LS_ON_DEGREE          30
#define FAN_HS_OFF_DEGREE         35
#define FAN_HS_ON_DEGREE          38

#define FAN_HS_LS_OFF             0                //HS and LS both off
#define FAN_LS_STAY               1                //LS FAN no change, HS FAN OFF
#define FAN_LS_ON                 2                //LS ON
#define FAN_HS_STAY               3                //HS no change
#define FAN_LS_HS_ON              4                //both on


#define FAN_HS_ON_TEST(x)  (((x)>=FAN_HS_ON_DEGREE)? (FAN_LS_HS_ON:FAN_HS_STAY)
#define FAN_HS_OFF_TEST(x) (((x)>=FAN_HS_OFF_DEGREE)? (FAN_HS_ON_TEST(x)):FAN_LS_ON) 
#define FAN_LS_ON_TEST(x)  (((x)>=FAN_LS_ON_DEGREE)? (FAN_HS_OFF_TEST(x)):FAN_LS_STAY)
#define FAN_TEMP_TEST(x)   (((x)>=FAN_LS_OFF_DEGREE)?(FAN_LS_ON_TEST(x)):FAN_HS_LS_OFF)

typedef struct{
  unsigned char check_timer;
  unsigned char amp_temperature;
  unsigned char smps_temperature;
  unsigned char fan_hs_status;
  unsigned char fan_ls_status;
  
  unsigned char fan_hs_on_amp_temp;
  unsigned char fan_hs_off_amp_temp;
  unsigned char fan_ls_on_amp_temp;
  unsigned char fan_ls_off_amp_temp;

  unsigned char fan_on_smps_temp;
  unsigned char fan_off_smps_temp;
  
} FAN_STATUS;

extern FAN_STATUS fan_status;

void fan_initial(void);
void fan_hs_ctrl(u8 status);
void fan_ls_ctrl(u8 status);
#endif
