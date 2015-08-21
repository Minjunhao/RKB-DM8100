#ifndef _NJW1112 
#define _NJW1112
#define NJW1112_HIGH        1
#define NJW1112_LOW         0
//chip address
#define CHIP_ADDR_LOW       0x0a
#define CHIP_ADDR_HIGH      0x0b

//select address
#define OUTPUT_A_ADDR       0x00
#define OUTPUT_B_ADDR       0x10
#define OUTPUT_C_ADDR       0x20
#define OUTPUT_D_ADDR       0x30
//selector
#define NJW1112_MUTE        0x00
#define INPUT_A1_B1         0x01
#define INPUT_A2_B2         0x02
#define INPUT_A3_B3         0x03
#define INPUT_A4_B4         0x04
#define INPUT_A5_B5         0x05
#define INPUT_A6_B6         0x06
#define INPUT_A7_B7         0x07
#define INPUT_A8_B8         0x08
//output switch
#define OUTPUT_ON           0x00
#define OUTPUT_OFF          0x01


void njw1112_delay(u32 delay_time);
void njw1112_write_start(void);
void njw1112_write_byte(u8 data);
void njw1112_register_write(u8 data,u8 addr);
void njw1112_local_speaker_ctrl(Channel_TYPE *chunnel,u8 selector);
void njw1112_link_speaker_ctrl(Channel_TYPE *chunnel,u8 selector);
#endif //_NJW1112