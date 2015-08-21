//Audio(31)
//#define RMC_POWER		0x00
#define RMC_POWEROFF	0x80
#define RMC_POWERON		0x98
#define RMC_VOL_UP		0x16
#define RMC_VOL_DN		0x17
#define RMC_MUTE		0x18
#define RMC_CD			0x01
#define RMC_TUNER		0x02
#define RMC_AUX1		0x03
#define RMC_AUX2		0x04
#define RMC_USB			0x05

#define RMC_PHONO		0x0a
#define RMC_COAX1		0x07
#define RMC_COAX2		0x08
#define RMC_BAL_XLR		0xd2
#define RMC_PC_USB		0xd0
#define RMC_OPT1		0xce
#define RMC_OPT2		0xcf
#define RMC_BASS		0x0e
#define RMC_TREBLE		0x0f
#define RMC_BALANCE		0x10

#define RMC_DIMMER		0x55
#define RMC_SELECT		0x1e
#define RMC_SETUP		0x42
#define RMC_RIGHT		0x1b
#define RMC_LEFT		0x1c
#define RMC_UP			0x57
#define RMC_DOWN		0x58
#define RMC_EXIT		0x14
#define RMC_SPEAKER_A   0x81
#define RMC_SPEAKER_B   0x82

#define RMC_BYPASS      0x15
//20121217 add 4ea keys
#define RMC_OPT11		0x88
#define RMC_OPT21		0x89
#define RMC_COAX11		0x8a
#define RMC_COAX21		0x8b
//--------------------------------------
//tuner(14)
#define RMC_0			0x51
#define RMC_1			0x48
#define RMC_2			0x49
#define RMC_3			0x4a
#define RMC_4			0x4b
#define RMC_5			0x4c
#define RMC_6			0x4d
#define RMC_7			0x4e
#define RMC_8			0x4f
#define RMC_9			0x50
//#define RMC_10			0x52
#define RMC_TUNE_DN 	0xcb
#define RMC_TUNE_UP 	0xca
#define RMC_PRESET_DN	0xc9
#define RMC_PRESET_UP	0xc8
//--------------------------------------
//cd (20)
#define RMC_CD_0		0x12
#define RMC_CD_1		0x09
#define RMC_CD_2		0x1d
#define RMC_CD_3		0x1f
#define RMC_CD_4		0x0d
#define RMC_CD_5		0x19
#define RMC_CD_6		0x1b
#define RMC_CD_7		0x11
#define RMC_CD_8		0x15
#define RMC_CD_9		0x17
#define RMC_CD_10		0x16

#define RMC_CD_PLAY		0x13
#define RMC_CD_STOP		0x0f
#define RMC_CD_TRACK_M	0x0e
#define RMC_CD_TRACK_P	0x1a
#define RMC_CD_PAUSE	0x0b
#define RMC_CD_RANDOM	0x00
#define RMC_CD_REPEAT	0x01
#define RMC_CD_SEARCH_M 0x0a
#define RMC_CD_SEARCH_P 0x1e
//--------------------------------------
#define RMC_CD_PROG		0x03        //RA not used.
#define RMC_CD_REVIEW	0x07        //RA not used.
#define RMC_CD_CLEAR	0x08        //RA not used.
#define RMC_CD_TIME		0x02        //RA not used.
#define RMC_CD_SCAN		0x04        //RA not used.
#define RMC_CD_EJECT	0x10        //RA not used.
//--------------------------------------


/*
void rmc4_Init(void);
void 		rmc4_Timer_250usec(void);
void		rmc4_CheckFlame_10msec(void);
void		rmc4_MakeRepeatKey_100msec(void);
void 		rmc4_process(void);

void rmc3_Init(void);	// RMC34_ADD    //jyj_merge
void 		rmc3_Timer_250usec(void);
void		rmc3_CheckFlame_10msec(void);
void		rmc3_MakeRepeatKey_100msec(void);
void 		rmc3_process(void);

void rmc2_Init(void);	// RMC34_ADD    //jyj_merge
void 		rmc2_Timer_250usec(void);
void		rmc2_CheckFlame_10msec(void);
void		rmc2_MakeRepeatKey_100msec(void);
void 		rmc2_process(void);
*/
void rmc1_Init(void);	// RMC34_ADD    //jyj_merge
void 		rmc1_Timer_250usec(void);
void		rmc1_CheckFlame_10msec(void);
void		rmc1_MakeRepeatKey_20msec(void);			//chd
void 		rmc1_process(void);

void rmcE_Init(void);	// RMC34_ADD    //jyj_merge
void 		rmcE_Timer_250usec(void);
void		rmcE_CheckFlame_10msec(void);
void		rmcE_MakeRepeatKey_20msec(void);			//chd
void 		rmcE_process(void);

//unsigned char Get_SystemKeyForRmcKey(unsigned char rmc_key);
unsigned char Get_SystemKeyForRmcKey(unsigned int rmc_custom, unsigned char rmc_key);

/* */
