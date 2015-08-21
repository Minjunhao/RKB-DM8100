// MPEG(CDP) key
#define MCK_0				0x00
#define MCK_1				0x01
#define MCK_2				0x02
#define MCK_3				0x03
#define MCK_4				0x04
#define MCK_5				0x05
#define MCK_6				0x06
#define MCK_7				0x07
#define MCK_8				0x08
#define MCK_9				0x09
#define MCK_10				0x0a
#define MCK_POWERON			0x10
#define MCK_POWEROFF		0x11		//STANDBY
#define MCK_OPEN_CLOSE		0x12		//? 254/32/5/18/53
#define MCK_PLAY			0x13
#define MK_PAUSE_STEP		0x14
#define MCK_STOP			0x15
#define MCK_FWD_SKIP		0x16
#define MCK_BWD_SKIP		0x17
#define MCK_FAST_FWD		0x18
#define MCK_FAST_BWD		0x19
#define MK_SLOW				0x1a		//SLOW MODE ON
#define MK_SLOW_FWD			0x1b		//ACT ON SLOWMODE ON
#define MK_SLOW_BWD			0x1c		//ACT ON SLOWMODE ON
#define MK_PROGRESSIVE_SCAN	0x1d		//HDMI RESOLUTION(480I/480P/720P/1080I/1080P)
#define MK_GOTO				0x1e		//SEARCH OF THE TITLE(TRACK) / CHAPER / TIME
#define MK_ZOOM				0x1f		//ZOOM IN, ZOOM OUT
#define MK_AUDIO			0x20
#define MK_SUBTITLE			0x21
#define MK_ANGLE			0x22
#define MCK_CLEAR			0x23
#define MK_RETURN			0x24
#define MK_DISPLAY			0x25		//PLAY INFORMATION
#define MK_SETUP			0x26		//DVD SETUP
#define MK_ENTER			0x27
#define MK_ARROW_UP			0x28
#define MK_ARROW_DOWN		0x29
#define MK_ARROW_LEFT		0x2a
#define MK_ARROW_RIGHT		0x2b
#define MK_MENU				0x2c
#define MK_TITLE			0x2d
#define MK_RESUME			0x2e
#define MCK_DIMMER			0x2f
#define MCK_REPEAT_AB		0x30
#define MCK_REPEAT			0x31
#define MCK_RANDOM			0x32
#define MCK_PROGRAM			0x33
#define MK_TV_TYPE			0x34		//CHANGE NTSC OR PAL(IN CASE THAT CAN'T DIRECT CHECK IN MPEG)
#define MK_BOOKMARK			0x35
#define MK_PAGE_UP			0x36		//DVD-A PAGE UP
#define MK_PAGE_DOWN		0x37		//DVD-A PAGE DOWN
#define MK_REGIONCHANGE		0x40		//NOT USED..
#define MK_MPEGUPGRADEMODE	0x41
#define MK_FACTORYRESET		0x43		//RESET ALL OF THE SETUP MENU ITEM(INCLUDE PASSWORD)
#define MCK_SW_VERSION		0x45		//DISPLAY MPEG SOFTWARE VERSION AND CREATED DATE
#define MK_PLAY_ENTER		0x46
#define MCK_PAUSE_PLAY		0x47
#define MCK_USB_CARD		0x48
#define MCK_POWER			0x49		//TOGGLE KEY
#define MK_STEP				0x4a
#define MCK_VOLUP			0x4b
#define MCK_VOLDOWN			0x4c
#define MCK_MUTE			0x4d
#define MCK_REVIEW			0x4e
#define MCK_SCAN			0x4f
#define MCK_TIME			0x50
#define MCK_PAUSE			0x51

#define MPEG_KEY_READY		0x00
#define MPEG_KEY_DATA		0x01


void MTK_init(void);
void Display_cdpBufferClear(void);
void tx_MTK(unsigned char t_data);
void uart2_keyData_Init(void);
void redata_execute1(void);
void Update_VFD(void);
void MPEG_main(void);

void	Func_TenKey_10(void);
void	Func_Program(void);
void	Func_Clear(void);
void	Func_Review(void);
void	Func_Time(void);
void	Func_Scan(void);
void	Func_CDPowerCheck_10msec(void);

