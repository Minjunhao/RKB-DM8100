#ifndef _NJW1194 
#define _NJW1194

#define VOL_ALL                             0
#define VOL_LEFT                            1
#define VOL_RIGHT                           2

#define NJW1194_HIGH                        1
#define NJW1194_LOW                         0

#define TONE_ON                             1
#define TONE_OFF                            0

#define DIR_HOLD                            0
#define DIR_UP                              1
#define DIR_DOWN                            2
#define DIR_DIRECT                          3

#define VOLUME_MUTE_VAL                     0
#define VOLUME_MAX_VAL                      255

//define register addr
#define VOL_CTRL_LEFT                       0x00
#define VOL_CTRL_RIGHT                      0x10
#define STERO_INPUT_SEL                     0x20
#define TONE_CTRL_TREBLE                    0x30
#define TONE_CTRL_BASS                      0x40
//chip addr
#define NJW1194_CHIP_A                      0x00
#define NJW1194_CHIP_B                      0x01
#define NJW1194_CHIP_C                      0x02
#define NJW1194_CHIP_D                      0x03

#define INPUT_MUTE                          0x00
#define INPUT_A                             0x04

void njw1194_treble_ctrl(Channel_TYPE *chunnel,u8 dir,u8 val);
void njw1194_bass_ctrl(Channel_TYPE *chunnel,u8 dir,u8 val);
void njw1194_balance_ctrl(Channel_TYPE *chunnel,u8 dir,u8 data);
void njw1194_vol_ctrl(Channel_TYPE *chunnel,u8 volume);
void volume_ctrl(Channel_TYPE *chunnel,u8 left_or_right,u8 direct,u8 set_volume);
void volume_mute_ctrl(Channel_TYPE *chunnel,u8 stat);
void njw1194_write_start(void);
void njw1194_tone_switch(Channel_TYPE *chunnel,u8 tone_val);
void njw1194_vol_ctrl_l(Channel_TYPE *chunnel,u8 volume);
void njw1194_vol_ctrl_r(Channel_TYPE *chunnel,u8 volume);
#endif //_NJW1194