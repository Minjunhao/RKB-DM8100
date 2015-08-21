#ifndef _AK4117 
#define _AK4117 

#define PLL_CHECK_TIME_40MS          40
#define AK4117_BIT_HIGH              1
#define AK4117_BIT_LOW               0
//CHIP SEL
#define CHIP_A                       0
#define CHIP_B                       1
#define CHIP_C                       2
#define CHIP_D                       3

//frequncy
#define FRQ_32KHZ                    0x03
#define FRQ_44KHZ                    0x00
#define FRQ_48KHZ                    0x02
#define FRQ_88KHZ                    0x08
#define FRQ_96KHZ                    0x0a
#define FRQ_176KHZ                   0x0c
#define FRQ_192KHZ                   0x0e

//register map
#define AK4117_PWR_DOWN_CTRL         0x00
#define AK4117_CLOCK_CTRL            0x01
#define AK4117_INOUT_CTRL            0x02
#define AK4117_INT0_MASK             0x03
#define AK4117_INT1_MASK             0x04
#define AK4117_REV_STATUS_0          0x05
#define AK4117_REV_STATUS_1          0x06
#define AK4117_REV_STATUS_2          0x07

//RX channel status byte 0~4
#define AK4117_RX_CHANNEL_STATUS_0   0x08
#define AK4117_RX_CHANNEL_STATUS_1   0x09
#define AK4117_RX_CHANNEL_STATUS_2   0x0a
#define AK4117_RX_CHANNEL_STATUS_3   0x0b
#define AK4117_RX_CHANNEL_STATUS_4   0x0c

//burst preamble pc byte 0~1
#define AK4117_BST_PRMB_PC_0         0x0d
#define AK4117_BST_PRMB_PC_1         0x0e
//burst preamble pd byte 0~1        
#define AK4117_BST_PRMB_PD_0         0x0f
#define AK4117_BST_PRMB_PD_1         0x10

//Q-subcode
#define AK4117_Q_SUBCODE_CTRL        0x11
#define AK4117_Q_SUBCODE_TRACK       0x12
#define AK4117_Q_SUBCODE_INDEX       0x13
#define AK4117_Q_SUBCODE_MNT         0x14        //minute
#define AK4117_Q_SUBCODE_SEC         0x15        //second
#define AK4117_Q_SUBCODE_FRM         0x16        //frame
#define AK4117_Q_SUBCODE_ZERO        0x17

//Q-subcode ABS
#define AK4117_Q_SUBCODE_ABS_MNT     0x18
#define AK4117_Q_SUBCODE_ABS_SEC     0x19
#define AK4117_Q_SUBCODE_ABS_FRM     0x1a


//ak4117 chip setting
#define AK4117_INITIAL               0x00
#define AK4117_FREQSET               0x01
#define AK4117_MUTE_CHECK            0x02
#define AK4117_CHECKING              0x03

//stream kind
#define STREAM_AC3            0
#define STREAM_DTS            1
#define STREAM_PCM            2
#define STREAM_MP3            4
#define STREAM_MPEG           5
#define STREAM_CHECKING       7
#define STREAM_OUTOFPCM       8
#define STREAM_NONA           9

void ak4117_get_stream(Channel_TYPE *chunnel);
void ak4117_int0_check(Channel_TYPE *chunnel);
void ak4117_ctrl(Channel_TYPE *chunnel);            
#endif //_AK4117