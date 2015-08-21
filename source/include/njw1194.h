#ifndef _NJW1194 
#define _NJW1194

#define NJW1194_HIGH                        1
#define NJW1194_LOW                         0

#define TONE_ON                             1
#define TONE_OFF                            0

#define DIR_HOLD                            0
#define DIR_UP                              1
#define DIR_DOWN                            2

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

void njw1194_treble_ctrl(Channel_TYPE *chunnel,u8 dir);
void njw1194_bass_ctrl(Channel_TYPE *chunnel,u8 dir);
void njw1194_balance_ctrl(Channel_TYPE *chunnel,u8 dir);
void njw1194_vol_ctrl(Channel_TYPE *chunnel,u8 volume);
#endif //_NJW1194