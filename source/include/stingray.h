/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : TFT Monitor with HDMI & YPbPr(CbCr) & YC & CVBS input
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : adc.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : / RTG
DESCRIPTION     :
*******************************************************************************/
#ifndef __STINGRAY_H__
#define __STINGRAY_H__

#define IRREGULAR_DATA			0x90
#define ALLDOTON_DATA				0x91

#define Unicode_IRREGULAR_DATA	0xE090




//CommandID(Master --->Slave)			//"Response"
#define CMD_GetStatus			0x01		//Status or ACK
#define CMD_GetScreen			0x02		//Screen or ACK					//when using serial interface, not need
#define CMD_Configure			0x03		//ACK
#define CMD_KeyPressed			0x04		//ACK
#define CMD_KeyReleased			0x05		//ACK
#define CMD_Sleep				0x06		//ACK
#define CMD_DeepSleep			0x07		//ACK
#define CMD_CommsError			0x08		//ACK
#define CMD_GetTimeAndDate		0x09		//TimeAndDATE
#define CMD_GetTimeAndDate2		0x0a		//TimeAndDATE2
#define CMD_SetConfigOption		0x0b		//ACK							//not supported yet
#define CMD_GetConfigText		0x0c		//ConfigText or ACK				//not supported yet
#define CMD_GetState			0x0d		//State or ACK
#define CMD_GetStationName 		0x0e		//StationName or ACK
#define CMD_SetLanguage			0x0f		//ACK
#define CMD_GetLanguage			0x10		//Language or ACK
#define CMD_SaveToPreset		0x11		//ACK
#define CMD_LoadPreset			0x12		//ACK
#define CMD_GetNetworkErrorCode	0x13		//NetworkErrorCode or ACK
#define CMD_FactoryReset		0x14		//ACK
#define CMD_SelectFont			0x15		//ACK
#define CMD_GetFontData			0x16		//FontData
#define CMD_SubmitText			0x17		//ACK							//20100129
#define CMD_OpaqueData			0x18		//					//20101007

/*	Master --->Slave
	Master <---Slave (ACK)
	
ACK			: CommandID(0x80+?) + SequenceNumber + Status(0:Fail, 1:Pass)

GetStatus	: CheckSum + Length(2) + Command ID(0x01) + SequenceNumber
Status		: CommandID(0x81) + SequenceNumber + status(0:not been configured, 1:has been configured)	

//GetScreen	: CheckSum + Length(2) + Command ID(0x02) + SequenceNumber		//not used serial interface
Screen		: CommandID(0x82) + SequenceNumber + tbl_Screen(n) 
	
const byte tbl_Screen[] = { 
	0x00, 0x04, 0x??, 0x??,			//Icons(0), Length, IconStatus, SignalStrength(0~100)
	0x01, 0x??, 0x00,	0x??,..		//LineContents(1), Length, LineNumber(0), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x01, 0x??, 0x01,	0x??,..		//LineContents(1), Length, LineNumber(1), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x01, 0x??, 0x02,	0x??,..		//LineContents(1), Length, LineNumber(2), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x01, 0x??, 0x03,	0x??,..		//LineContents(1), Length, LineNumber(3), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x02, 0x05, 0x01, 0x00, 0x00 };	//CursorPosition(2), Length, CursorOn, XPosition, YPosition(line number)
};	


Configure	: CheckSum + Length(2) + Command ID(0x03) + SequenceNumber + tbl_Configure(10)
ACK			: CommandID(0x83) + SequenceNumber + status(0:Fail, 1:Pass)	
	
KeyPressed  : CheckSum + Length(2) + Command ID(0x04) + SequenceNumber + keyValue
ACK			: CommandID(0x84) + SequenceNumber + status(0:Fail, 1:Pass)	

KeyReleased	: CheckSum + Length(2) + Command ID(0x05) + SequenceNumber + keyValue
ACK			: CommandID(0x85) + SequenceNumber + status(0:Fail, 1:Pass)	

Sleep		: CheckSum + Length(2) + Command ID(0x06) + SequenceNumber
ACK			: CommandID(0x86) + SequenceNumber + status(0:Fail, 1:Pass)	

DeepSleep	: CheckSum + Length(2) + Command ID(0x07) + SequenceNumber
ACK			: CommandID(0x87) + SequenceNumber + status(0:Fail, 1:Pass)	

CommsError	: CheckSum + Length(2) + Command ID(0x08) + SequenceNumber + LastMessageID + LastMessageSequenceNumber
ACK			: CommandID(0x88) + SequenceNumber + status(0:Fail, 1:Pass)	

GetTimeAndDate	: CheckSum + Length(2) + Command ID(0x09) + SequenceNumber
TimeAndDate 	: CommandID(0x89) + SequenceNumber + Seconds(4) < since 1970, [0]=MSB, [3]=LSB >
	
GetTimeAndDate2	: CheckSum + Length(2) + Command ID(0x0a) + SequenceNumber
TimeAndDate 	: CommandID(0x8a)+SequenceNumber+Year(MSB)+Year(LSB)+Weekday(0~6)+Month(0~11)+Day(1~31)+Hour(0~23)+Minute(0~59)+Seconds(0~59)
	
GetState		: CheckSum + Length(2) + Command ID(0x0d) + SequenceNumber
State			: CommandID(0x8d) + SequenceNumber + Mode + State + Reserved1 + Reserved2

GetStationName	: CheckSum + Length(2) + Command ID(0x0e) + SequenceNumber + InfoType + PresetNumber <0: currently playing station>
StationName		: CommandID(0x8e) + SequenceNumber + InfoType + PresetNumber + StationName(n) + null(0x00)
	
SetLanguage		: CheckSum + Length(2) + Command ID(0x0f) + SequenceNumber + LanguageString(5) + null(0x00)
ACK				: CommandID(0x8f) + SequenceNumber + status(0:Fail, 1:Pass)	

GetLanguage		: CheckSum + Length(2) + Command ID(0x10) + SequenceNumber
Language		: CommandID(0x90) + SequenceNumber + LanguageString(5) + null(0x00)
	
SaveToPreset	: CheckSum + Length(2) + Command ID(0x11) + PresetNumber
ACK				: CommandID(0x91) + SequenceNumber + status(0:Fail, 1:Pass)	
	
LoadPreset		: CheckSum + Length(2) + Command ID(0x12) + InfoType(0xff) + PresetNumber < 0xff: preset from current mode(IR/FM/DAB) >
ACK				: CommandID(0x92) + SequenceNumber + status(0:Fail, 1:Pass)	
		
*/
//This is a bitmask indicating the current status of the icons.


//Indicates if line is inverted
typedef enum { 
	Invert_not_inverted = 0x00,
	Invert_inverted = 0x01
} tbl_Invert;
//Incicates whether any arrows should be drawn on the left or right hand edges of the line.
typedef enum { 
	ArrowStatus_Arrow_off = 0x00,
	ArrowStatus_Left_Arrow = 0x01,
	ArrowStatus_Right_Arrow = 0x02,
	ArrowStatus_Both_Arrows = 0x03
} tbl_ArrowStatus;
//Indicates if the cursor is on
typedef enum { 
	CursorOn_off = 0x00,
	CursorOn_on = 0x01
} tbl_CursorOn;
//





typedef enum { 
	PRESET_END = 0x00,
	MEMORY_START = 0x01,
	MEMORY_PRESET2Ready = 0x02,
	MEMORY_ENDing = 0x03,
	MEMORY_END = 0x04,
	PRESET_START = 0x06,
	PRESET_PRESET2Ready = 0x07,
	PRESET_ENDing = 0x08,
	PRESET_EMPTY = 0x09,
	PRESET_UpDownENDing = 0x0a			//20110602
} tbl_PresetStatus;

typedef enum { 
	DAB_POWER_ON = 0x01,
	DAB_SID_SEARCH = 0x02,
	DAB_NORMAL = 0x03,
	DAB_NOSERVICE = 0x05,
	DAB_PRESET = 0x06,
	DAB_PRESET_TRACKING = 0x07
} tbl_DisplayStatus;


typedef enum { 
	S_STARTUP = 0x00,
	S_STARTUP1 = 0x01,
	S_GETSTATUS = 0x02,
	S_ACKSTATUS = 0x03,
	S_SETCONFIGOPTION = 0x04,
	S_ACKCONFIGOPTION = 0x05,
	S_SETLANGUAGE = 0x06,
	S_ACKLANGUAGE = 0x07,
	S_TX_CONFIGURE = 0x08,
	S_ACKCONFIGURE = 0x09,
	S_REBOOTING = 0x0a,
	S_INITIALIZE_FAIL = 0x0b,
	S_STATIONNAME_READ = 0x0c,
	S_GETSTATE = 0x0d,
	S_CONFIGURE_OK = 0x0e
} tbl_StingrayStatus;
//
typedef enum { 
	STRX232_ACKERR = 0x00,
	STRX232_SYNC = 0x01,
	STRX232_ACK80 = 0x02,
	STRX232_ACK81 = 0x03,
	STRX232_ACKSTATUS = 0x04,

	STRX232_STATESN = 0x06,	
	STRX232_STATEdata = 0x07,	

	STRX232_STATIONNAMESN = 0x09,	
	STRX232_STATIONNAMEinfo = 0x0a,	
	STRX232_STATIONNAMEpreset = 0x0b,	
	STRX232_STATIONNAMEdata = 0x0c,	
	
	STRX232_SCREENSN = 0x0e,
	STRX232_SCREENSTATUS = 0x0f,
	STRX232_ICONSCHECK = 0x10,
	STRX232_ICONSCHECK1 = 0x11,
	STRX232_LINECHECK = 0x12,
	STRX232_LINECHECK1 = 0x13,
	STRX232_CURSORCHECK = 0x14,
	STRX232_CURSORCHECK1 = 0x15,

	STRX232_KeyOPAQUE = 0x16		//20100927 
} tbl_StingrayReadStatus;

#define MODE_NONE		0


/*----------------------------------------------------------------------------*/

void Stingray_Init(void);
void Stingray_Main(void);
void Stingray_Tx_GetStatus(void);
void Stingray_Tx_SetConfigOption(void);
void Stingray_Tx_SetLanguage(void);
void Stingray_Tx_Configure(void);
void Stingray_Tx_KeyPressed(unsigned char key_value);
void Stingray_Tx_KeyReleased(unsigned char key_value);
void Stingray_Tx_Sleep(void);
void Stingray_Tx_DeepSleep(void);
void Stingray_Tx_SaveToPreset(unsigned char preset_value);
void Stingray_Tx_LoadPreset(unsigned char infoType,unsigned char preset_value);		//20100128
void Stingray_Tx_GetState(void);
void Stingray_Tx_GetStationName(unsigned char infoType, unsigned char preset_value);
void Stingray_Tx_FactoryReset(void);
void Stingray_Tx_SubmitText(void);						//20100129
void Stingray_Init(void);
void DAB_Preset_Reset(void);
void Func_STMPresetStart(unsigned char num);
void Func_STMPreset(unsigned char num);
void Func_TunerMemory(void);
void Func_MemoryStart(void);
void Stingray_KeyInput(unsigned char data);
void Stingray_Uart3_check(void);
void PresetEmptyCheck(unsigned char preset_num);		//20091216
unsigned char stringKeyCheck(void);						//20100201
void makeSortPresetListTable(void);					//20100210
void Stingray_Tx_RotelOpaqueData(unsigned short Ssize);	//20101007

void Func_Preset_Up(void);		//20110602
void Func_Preset_Down(void);
void rFunc_Preset_Up(void);		//20110922
void rFunc_Preset_Down(void);


#endif /* __STINGRAY_H__ */

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/
