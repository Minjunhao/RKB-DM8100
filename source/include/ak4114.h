#define	P_LOW			0
#define	P_HIGH			1

#define DIR_READ_OK		0x01
#define DIR_READ_ERR	0x00

#define MODE_AUTO		0x00
#define MODE_NORMAL		0x01

#define INIT_OK			0xf0

/*** INPUT SELECTION ****/
#define I_COAX1			0x00		
#define I_ANALOG		0x06        //analog Function
#define I_NULL			0x7f

// frequency
#define ST_48			0x00
#define ST_441			0x01
#define ST_32			0x02
#define ST_96			0x03
#define ST_882			0x04
#define ST_192			0x05
#define ST_1764			0x06

/*** STREAM KIND     ****/
#define ST_A_AC3              0
#define ST_A_DTS              1
#define ST_A_PCM              2
#define ST_A_HDCD             3
#define ST_A_MP3              4
#define ST_A_MPEG             5
#define ST_A_ANALOG           6
#define ST_CHECKING           7
#define ST_A_OUTOFPCM         8
#define ST_A_NONA             9
#define ST_A_DDPLUS			  	20
#define ST_A_TRUEHD			  	21
#define ST_A_DTSHD_LBR		  	22
#define ST_A_DTSHD_HRA		  	23
#define ST_A_DTSHD_HRA_X96	  	24
#define ST_A_DTSHD_MA_48	  	25
#define ST_A_DTSHD_MA_96	  	26
#define ST_A_DTSHD_MA_192	  	27
#define ST_A_DTSHD_MA_NBC_48	28
#define ST_A_DTSHD_MA_NBC_96	29
#define ST_A_DTSHD_MA_NBC_192	30

/*** PLL STATUS    ***/
#define PLL_LOCK              0
#define PLL_UNLOCK            1
#define PLL_CHECKING          2

/*** SYSTEM MODE ********/
#define AK4114_INIT			0x00
#define AK4114_FREQSET		0x01
#define AK4114_MUTECHK		0x02
#define AK4114_MONITOR		0x03

void DIR_4114_Reset(void);
void s_4114_Exec(unsigned char typ);				//20130527
void s_Ak4114MsgRead(unsigned char typ, unsigned char s_data);
void s_Ak4114MsgTrans(unsigned char typ);
//20131022
void S_4114_RESET(unsigned char i);

