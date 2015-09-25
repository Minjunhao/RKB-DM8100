//typedef	unsigned char		UNS_8;
//typedef 	unsigned char		UCHAR;
//typedef	unsigned int  		UNS_16;
//typedef 	unsigned int		WORD;
//typedef 	unsigned long		DWORD;	 
//#if 
//#else
//#endif		
/**********************************************************
		Compile Option 
		! RS232     UART1 baud rate ( current : 115200 )
		!           UART2 baud rate ( current : 115200 )
		!           UART3 baud rate ( current :        )
		!           UART4 baud rate ( current : 57600  )
		
		? BLUETOOTH_INCLUDE
		?
//----------------------------------------------------------
//----------------------------------------------------------------
//	STM32F10x
//	mode_AHB_prescaler
//	1=36MHz, 2=18MHz, 4=9MHz, 8=4.5MHz
//	Normal : mode_AHB_prescaler = 1;
//	Standby: mode_AHB_prescaler = 8;
//----------------------------------------------------------
//RKB MP1 - re-work board 에서의 수정사항 : 20140619
//	option ID0 : (fix) = High(8100)
//	option ID1 : = Low(Analog), = High(Digital) =>> model name change : RKB Digital/Analog select , 850/D850 에서의 온도 display 가 항상 2개의 sensing mode 로 표현됨.
//
//	
//RKB MP1 re-work & MP2 를 위해서 아래 2개의 option 을 같이 움직여야합니다.
//----------------------------------------------------------
#define 	JTAG_DISABLE                //20140612 : JTAG disable => JTAG normal port!!!!!!
#ifdef JTAG_DISABLE
#define 	SIGNAL_SENSING_ENABLE		//20140612 : signal sensing mode : FAN Low Speed : enable // and need compile option : JTAG_DISABLE of config.h
#endif
**********************************************************/
#define _RKB_DM8100
//#define _TEST
#define _ETH_TEST
#ifdef _ETH_TEST
#define PORTG_PORT				GPIOG    
#define MAIN_STBY1				GPIO_Pin_2			//144_34 => when boot mode, STBY_Relay port not so good, so change to this pin
#define Standby_34(x)			( (x==1)? GPIO_SetBits(PORTG_PORT, MAIN_STBY1) : GPIO_ResetBits(PORTG_PORT, MAIN_STBY1) );
#endif
#define IAP_UPGRADE
//#define _IWDG_ENABLE                          //IWDG for the LWIP trap in the endless loop
#define _ETHERNET
#define UART4_ENABLE
#define UART5_ENABLE
#define FROM_MP2_CHANGE_TEMP					//20140731 : from MP2(add Signal Sensing Mode) change to newTemp.
#define SIGNAL_SENSING_DEBUG					//20140730 : Signal Sensing Mode string out
#define AK4117_Xtal_ENABLE						//20140611 : WS1/2 =>External Clock Mode, PP/MP=Off Mode, ( X'tal Mode is compile option )
//#define OPTICAL_POWER_ENABLE					//20140515 : optical power mode = enable
#define LED_DIMMER_ENABLE						//20140224 : Front LED dimming feature enable :
#define RS232_VOL_ENABLE_IS_TRIMMIN			//20140218 : front trimKonb enable / disable RS232 commands of front_trimKnob_enable of RS232 /
#define POWER_ON_ADDDELAY_BEFORE_RELAYON		//20140104 : before relay-on, add delay time 
#define MULTI_DIR_ENABLE						//130620 RKB multi-AK4117
//#define VFD_ENABLE   			    			//120910 for RA1570, VFD
#define	ROTEL_VFD_2x20		        			//[4x20] or [2x20] and dimmerRange[1..20] or [1..10] => dimmer Table 변경해세요..
//#define ADD_ID_ROTELLINK   					//120829 for RA1570, add ID ["11:" + RotelOpaqueData()] and command ["10:" or "00:" + command[]]
//#define VOLUME_SPEED							//volume speed
//#define ENCODER_IS_VOLUME   					//120906 for RA1570, encoder volume key
//#define ROTELLINK_DIMMER_TX_ENABLE   			//120917 for RA1570, rotel link dimmer key => enalble response


//#define 	IC_DEBUG					//rs232 i/o debugger
//#define 	UART1_DEBUG					//rs232 out debugger
//#define 	UART1_SCREEN_DEBUG			//rs232 out debugger [test:20100430]
//#define 	UART1_SCREENUTF8_DEBUG		//rs232 out debugger [test:20100511]

//#define 	IAP_UPGRADE                 //for iAP upgrade
#define 	AMP_POWERON_ENABLE          //20121011 : 
//#define     _TEST
//
//
//  test for RA1570 2 Line iPod display
//
#define RA_2LINE_DISP   			//120809 for RA1570 2Line display. <== Please check <===================================================

#ifdef RA_2LINE_DISP
#define OTHER_FUNCTION_PAUSE   		//120815 when go other function => USB "pause" maintain => find func_USBiPod
#endif

#define BLUETOOTH_INCLUDE			//100507 for built-in bluetooth USB dongle
#define RESETMODULE_PER2SEC			//100628 added
//#define MULTI_LAN					//110826 USB/iPod multi-language ??? <<< test 해야함...니다..... >>>


/* project compile option */
#define _PRINTX						//for debug print state
#define ADD_SYNCSUB					//total sync byte extended 3 byte
#define UTF8toASCII					//090430

#ifndef RA_2LINE_DISP               //RA1570 <===not used ?
#define USED_NAVI_KEY				//090601 : not used in RA1570
#endif

#define TRACK_NEXT					//090630
#define LIST_CIRCULATION			//090703
#define GET_CATEGORYNAME			//090707 for get artist & album name when docked iPod

#ifndef RA_2LINE_DISP               //RA1570 <===not used ?
#define NOT_AUTOPLAY				//090709 for do not auto play when first iPod docking 
#endif

#define FIRSTTRACK_AFTER_TWICESTOP	//090713 for first track play due to twice stop key or module stop itself.
#define DEV_UNMOUNT					//090713 for safely remove device (iPod,USB)
#define YouTubeBug					//090805 for play YouTube on the iPod touch
#define FIXED_SUFFLE				//091028 modified for turn shuffle off if did turn shuffle on..occured only on ipod nano

#ifndef RA_2LINE_DISP               //RA1570 <===not used ?
#define CHANGE_PLAYINGMENU 			//091028 for make menu with mr mike`s suggestion 
#endif

#define RECOVERY_CATEGORY			//091030 for recovery category play list after did search browsing menu ===================================> RA1570 delete
#define TRACKNO_USB_INEACHCATEGORY	//100115 for recovery folder ===================================> RA1570 delete
#define MNGR_NAVI_MOTHER			//091028 for another navigate on browsing menu ..memorize mother step ===================================> RA1570 delete
#define CENTER_FIT					//091104 for center fit of display 
#define DIVIDE_SHUFFLE				//091104 for divide shuffle function ===================================> RA1570 delete

#define TWIRL_LOADING				//091109 for display as loading twirling bar "Loading..
#define DISP_TITLE					//091109 added for display title no.
#define PLAY_IN_BROWSINGMENU		//091110 for hot play when push play button in browsing menu
#define MNGR_NAVI_CHILD				//091110 for another navigate on browsing menu ..memorize child step ===================================> RA1570 delete


#ifndef RA_2LINE_DISP               //RA1570 <===not used ?
#define NUMERIC_10KEY				//100119 for search playlist using numeric key
#endif

#define GET_FIRSTLYCURRLIST			//100201 for get current browsing list before get previous list.

#ifdef SIMPLE_LINGO				//20100305 : enable Simple Lingo
#define SIMPLE_REMOTE_LINGO			//100218 for use simple remote lingo control on ipod.
#ifdef SIMPLE_REMOTE_LINGO
	#define EXTENDED_NAME 			//100224 for get genre,composer
#endif
#endif


/*
    Telechips : USB : display
	참조 buffer
		pMulti.UTF8Code = USB(UNICODE)
		pMulti.UTF8Code = iPod(UTF-8)
	title 정보	
		pMulti.Title
		pMulti.Album
		pMulti.Artist
	folder browsing을 위한 버퍼
		pMulti.CurrList[] --[0] : 첫번째 줄
			                [1] : 두번째 줄
							[2] : 세번째 줄 
							[3] : 네번째 줄
**********************************************************/
//#define PARAMETER_EQ				//090601
//#define IPOD_INCLUDE				//for iPod

/*********************************************************
ASCii Rotel-Link
*********************************************************/
#define	S_CONFIGURE_OK 				1
#define	S_CONFIGURE_NO 				0

/*********************************************************
*********************************************************/
#define 	MAX_STRING_TEXT_LENGTH		20			//20100201

#define LAN8720_PHY_ADDRESS    0x00

// on,off 
#define	ON							1
#define	OFF							0


#define	RKB_TRUE					1
#define	RKB_FALSE					0
#define	OK							1
#define	NO							0 
#define	YES							1
#define	DUMMY						2

/* key attribute */
#define	KEY_NONE					0x00
#define	KEY_FRONT					0x01
#define	KEY_RMC1					0x02
#define	KEY_RS232					0x03
#define KEY_SPECIALKEY				0x04
#define	KEY_RS232_0					0x05
#define	KEY_OPAQUE					0x06	//20100927

#define CR							0x0d	// carrige return	
#define LF							0x0a	// line feed

// boot종류.
#define BOOT_AC						0
#define BOOT_NORMAL					1

// power option
#define POWER_DIRECT				0x00
#define POWER_STANDBY				0x01
#define POWER_ALWAYSON				0x02
#define POWER_STANDBY1				0x03	//20090917 : RCX1500 AC on(POWER_STANDBY1) => power on(BOOT_AC)

// power status
#define	POWER_OFF					0
#define	POWER_ON0_ING				1
#define	POWER_ON1_ING				2
#define	POWER_ON2_ING				3
#define	POWER_ON3_ING				4
#define	POWER_ON4_ING				5
#define	POWER_ON5_ING				6
#define	POWER_ON6_ING				7
#define	POWER_ON0					8

#define IRREGULAR_DATA              0x90
#define ALLDOTON_DATA				0x91
#define Unicode_IRREGULAR_DATA      0xE090

// LED dimmer [ 1(bright)..10(dark) ]
#define	LED_DIMMER_READY			0
#define	LED_DIMMER_MAX				10
#define	LED_DIMMER_DEFAULT			1
#define	LED_DIMMER_MIN				1

// display update (auto,manual)(0,1,0)
#define	DISPLAY_AUTO				0
#define	DISPLAY_MANUAL				1

//(mode_func) function status
#define	FUNCTION_DISP_OFF			0
#define	FUNCTION_AMP_A				1
#define	FUNCTION_AMP_B				2
#define	FUNCTION_AMP_C				3
#define	FUNCTION_AMP_D				4
//(Led_mode) function status
#define	LED_MODE_SKIP				0		//20140409
#define	LED_MODE_STOP				1
#define	LED_MODE_POWER_OFF			2		//20140901 : add LED-off time 500msec -> LED_MODE_POWER_OFF_DIM
#define	LED_MODE_POWER_OFF_DIM		3		//20140901 : 90% dimmed-LEd and blinking 1 time
#define	LED_MODE_POWER_ON			4
#define	LED_MODE_PROTECTION			5
#define	LED_MODE_POWER_ON_MUTEOFF	6		//20131216 : from PowerOn to MuteOff blinking
#define	LED_MODE_POWER_ON_SIG_OFF	7		//20140718 : from No signal(after 10minute) blinking( short time off )
#define	LED_MODE_POWER_ON_SIG_DIM	8		//20140901 : from No signal(after 10minute) add LED-Off time 500msec -> LED_MODE_POWER_ON_SIG_DIM1
#define	LED_MODE_POWER_ON_SIG_DIM1	9		//20140901 : from No signal(after 10minute) 50% dimmed-LED and blinking 1 time

//UART4,5 Tx
#define ALL_UNIT			0   	//20120131
#define RA12_UNIT			1   	//20120131
#define RCD12_UNIT			2   	//20120131
#define ALL_UNIT_SKIP		3   	//20120229

//RKB seriiieis
#define	RKB_850 					0
#define	RKB_8100					1
#define	RKB_D850					2
#define	RKB_D8100					3

// Temperature ADC mode
#define TEMP_ON						0
#define TEMP_ADC					1

// mute mode
#define	MUTE_OFF					0
#define	MUTE_ON						1

// ver display mode
#define	VER_DISPLAY_OFF				0
#define	VER_DISPLAY_ON				1

// FAN  mode
#define	FAN_READY					0
#define	FAN_EXEC					1
//#define FAN_ENABLE_TEMPERATURE		55			//2.071V	//20130708
//#define FAN_DISABLE_TEMPERATURE		52			//1.955V	//20130708
//------------------------------------------------------------------------------
#ifdef FROM_MP2_CHANGE_TEMP			//20140731
//------------------------------------------------------------------------------
// FAN ls on/off TEMP
#define	FAN_LS_OFF_TEMP_INITIAL		40
#define	FAN_LS_ON_TEMP_INITIAL		45
//------------------------------------------------------------------------------
// FAN hs on/off TEMP
#define	FAN_HS_OFF_TEMP_INITIAL		50
#define	FAN_HS_ON_TEMP_INITIAL		60
//------------------------------------------------------------------------------
// SMPS FAN on/off TEMP
#define	SMPS_FAN_OFF_TEMP_INITIAL	50
#define	SMPS_FAN_ON_TEMP_INITIAL	60
//------------------------------------------------------------------------------
// ShutDown TEMP -> got to AMP Protection Mode
//#define	SHUTDOWN_TEMP_INITIAL	75			//20131107
#define	SHUTDOWN_TEMP_INITIAL		80			//20131211
// SMPS ShutDown TEMP -> got to AMP Protection Mode
#define	SMPS_SHUTDOWN_TEMP_INITIAL	95			//20131107
#define SMPS_TEMPERATURE_MAX 		99			//20131211
//------------------------------------------------------------------------------
#else //FROM_MP2_CHANGE_TEMP
//------------------------------------------------------------------------------
// FAN ls on/off TEMP
#define	FAN_LS_OFF_TEMP_INITIAL		30			//20140521
#define	FAN_LS_ON_TEMP_INITIAL		35			//20140521
//------------------------------------------------------------------------------
// FAN hs on/off TEMP
#define	FAN_HS_OFF_TEMP_INITIAL		40			//20131105
#define	FAN_HS_ON_TEMP_INITIAL		45			//20131105
//------------------------------------------------------------------------------
// SMPS FAN on/off TEMP
#define	SMPS_FAN_OFF_TEMP_INITIAL	45			//20140325
#define	SMPS_FAN_ON_TEMP_INITIAL	50			//20140325
//------------------------------------------------------------------------------
// ShutDown TEMP -> got to AMP Protection Mode
//#define	SHUTDOWN_TEMP_INITIAL	75			//20131107
#define	SHUTDOWN_TEMP_INITIAL		80			//20131211
// SMPS ShutDown TEMP -> got to AMP Protection Mode
#define	SMPS_SHUTDOWN_TEMP_INITIAL	95			//20131107
#define SMPS_TEMPERATURE_MAX 		99			//20131211
//------------------------------------------------------------------------------
#endif //FROM_MP2_CHANGE_TEMP
//------------------------------------------------------------------------------
//20140718 : test : for signal sensing feature
#define	SIGNAL_SENSING_OFF_TIME		10*60*1000/100			//10 minutes / 100msec
//#define	SIGNAL_SENSING_OFF_TIME		10*1*1000/100		//10 sec / 100msec
//20140811 : change 500usec to 50msec
#define	SIGNAL_SENSING_ON_TIME		100/50					//100usec / 50usec

//		  
#define	MAX_MASTER_VOLUME		    96			//
#define	MAX_TRIM_VOLUME			    96			//
#define	MAX_TEMP_VOLUME			    100			//
#define	TRIM_INITIAL			    0			//
#define	TEMP_INITIAL			    0			//
//
#define	AMP_ON_INITIAL				1			//20140212 : AMP A/B/C/D = ON
#define	VOLUME_INITIAL				0			//20140416
#define	MAX_BALANCE_VOLUME			30
#define	BALANCE_VOLUME_INITIAL		15			//20140212
// input sel mode
#define	INPUT_SEL_INITIAL			0			//20150302
#define	INPUT_SEL_AUTO				0			//20150302
#define	INPUT_SEL_DIGITAL			1
#define	INPUT_SEL_ANALOG			2
#define	MAX_INPUT_SEL				2

// system key
#define SK_POWER			0
#define SK_POWERON			1
#define SK_POWEROFF			2
#define SK_VOL_UP			3		//long key
#define SK_VOL_DN			4		//long key
#define SK_MUTE				5		
#define SK_CD				6		//RA
#define SK_ST_MPLAYER		7
#define SK_USB				8       //RA
#define SK_AUX1				9		//RA
#define SK_AUX2				10		//RA
#define SK_ST_FM			11		
#define SK_ST_DAB			12		
#define SK_ST_iRADIO		13		
#define SK_ST_QUEUE			14
#define SK_SETUP			15
#define SK_MENU				15

#define SK_EXIT				16
#define SK_UP				17		//RCX:long key
#define SK_DOWN				18		//RCX:long key
#define SK_ST_BACK			19      //back=left
#define SK_LEFT 			19      //back=left
#define SK_ST_REPLY			20      //replay=right
#define SK_RIGHT			20      //replay=right
#define SK_SELECT			21
#define SK_MEMORY			22
#define SK_DIMMER			23
#define SK_0				24
#define SK_1				25		
#define SK_2				26		
#define SK_3				27		
#define SK_4				28		
#define SK_5				29		
#define SK_6				30		
#define SK_7				31		
#define SK_8				32		
#define SK_9				33		
#define SK_10				34		

#define SK_RMC_0			35
#define SK_RMC_1			36		//long key
#define SK_RMC_2			37		//long key
#define SK_RMC_3			38		
#define SK_RMC_4			39		
#define SK_RMC_5			40		
#define SK_RMC_6			41		
	

#ifdef _RKB_DM8100
#define SK_SET_STATIC_IP    42
#define SK_SET_STATIC_GATEWAY 43
#define SK_SET_STATIC_SUBNETMASK 44
#define SK_SET_STATIC_DNS        45   
#define SK_GET_IPADDRESS	46		 
#define SK_GET_SUBNETMASK	47		
#define SK_GET_GATEWAY		48		
#define SK_GET_DNSSERVER	49		 
#define SK_GET_IPMODE		50		

#define SK_IPMODE_DHCP		51		 
#define SK_IPMODE_STATIC	52		
#define SK_NETWORK_STATUS	53		
#define SK_IP_REFRESH		54		 
#define SK_SET_MAC_ADDR     55
#define SK_GET_MAC_ADDR     56
#else
#define SK_RMC_6			41		
#define SK_RMC_7			42		
#define SK_RMC_8			43		
#define SK_RMC_9			44		
#define SK_RMC_10			45	
#define SK_CD_0				46
#define SK_CD_1				47		//long key
#define SK_CD_2				48		//long key
#define SK_CD_3				49		
#define SK_CD_4				50		
#define SK_CD_5				51		
#define SK_CD_6				52		
#define SK_CD_7				53		
#define SK_CD_8				54		
#define SK_CD_9				55		
#define SK_CD_10			56		
#endif


#define SK_CD_PROG			57		
#define SK_CD_REVIEW		58		
#define SK_CD_CLEAR			59
#define SK_CD_TIME			60
#define SK_CD_SCAN			61
#define SK_CD_EJECT			62		//RCX
#define SK_CD_PLAY			63
#define SK_CD_STOP			64
#define SK_CD_TRACK_M		65		//long key	
#define SK_CD_TRACK_P		66		//long key
#define SK_CD_PAUSE			67
#define SK_CD_RANDOM		68		
#define SK_CD_REPEAT		69

#define SK_CD_SEARCH_M 		70		//long key		
#define SK_CD_SEARCH_P 		71		//long key		
#define SK_MUTE_ON			72		
#define SK_MUTE_OFF			73		
//
#define SK_OPT1		        74      //RA opt1
#define SK_COAX1		    75      //RA coax1
#define SK_OPT2		        76      //RA opt2
#define SK_COAX2		    77      //RA coax2
//20110602
#define SK_PRESET_UP		78
#define SK_PRESET_DOWN		79
//RC1590 [ 80~102 ]
#define SK_PC_USB				80		//RC1590,RA
#define SK_BALANCE_LEFT			81		//
#define SK_BALANCE_RIGHT		82		//
#define SK_BASS_UP				83		//
#define SK_BASS_DOWN			84		//
#define SK_TREBLE_UP			85		//
#define SK_TREBLE_DOWN			86		//
#define SK_LINE_1				87		//
#define SK_LINE_2				88		//
#define SK_LINE_3				89		//
#define SK_LINE_4_OPTI			90		//
#define SK_LINE_5_OPTI			91		//
#define SK_LINE_6_OPTI			92		//
#define SK_LINE_7_OPTI			93		//
#define SK_LINE_8_OPTI			94		//
#define SK_LINE_11				95		//
#define SK_LINE_12				96		//
#define SK_LINE_13				97		//
#define SK_LINE_4_COAX			98		//
#define SK_LINE_5_COAX			99		//
#define SK_LINE_6_COAX			100		//
#define SK_LINE_7_COAX			101		//
#define SK_LINE_8_COAX			102		//
//20111222
#define SK_RDSDISP				103		//
#define SK_FMMONO				104		//	
#define SK_TUNE_UP				105		//
#define SK_TUNE_DOWN			106		//	
//20111229
#define SK_PHONO				107		//RA
#define SK_TUNER				108		//RA	
#define SK_AUX					109		// RT12
#define SK_BYPASS				110		//toggle key
#define SK_GET_BYPASS			111		//	
#define SK_BYPASS_ON			112		//	
#define SK_BYPASS_OFF			113		//	
#define SK_GET_BASS				114		//	
#define SK_GET_TREBLE			115		//	
#define SK_GET_BALANCE			116		//	

#define SK_NULL				0x7f	//127
#define NO_KEY				0x7f	//127
//ASCiiKEY [ 128~145 ]
#define SK_DISPLAY_ALL	    128		//chd_keyadd
#define SK_DISPLAY1		    129		//chd_keyadd
#define SK_DISPLAY2		    130		//chd_keyadd
#define SK_DISPLAY3		    131		//chd_keyadd
#define SK_DISPLAY4		    132		//chd_keyadd
#define SK_PRODUCT_TYPE		133		//chd_keyadd
#define SK_PRODUCT_VERSION	134		//chd_keyadd
#define SK_GET_VOLUME_MAX  	135		//chd_keyadd
#define SK_GET_VOLUME_MIN   136		//chd_keyadd

#define SK_DISP_SIZE	    137		//chd_keyadd
#define SK_DISP_UPDATE	    138		//chd_keyadd
#define SK_DISP_AUTO	    139		//chd_keyadd
#define SK_DISP_MANUAL	    140		//chd_keyadd
#define SK_CURRENT_PRESET   141		//chd_keyadd
#define SK_PRESET_MAX	    142		//chd_keyadd
//20110414
#define SK_GET_CDTRAY	  	143		//chd_keyadd
#define SK_GET_CDDISC  		144		//chd_keyadd
#define SK_GET_CDPLAY	  	145		//chd_keyadd
//
//20110719
#define SK_STOP_SHORT	  	146		//chd_keyadd
#define SK_SELECT_SHORT		147		//chd_keyadd
#define SK_STOP_LONG	  	148		//chd_keyadd
#define SK_SELECT_LONG 		149		//chd_keyadd
//20110721
#define SK_SEARCH_INFO	  	150		//chd_keyadd
#define SK_SEARCH_TEXT 		151		//chd_keyadd
//20110809
#define SK_GET_SOURCE	  	152		//chd_keyadd
#define SK_GET_POWERMODE	153		//chd_keyadd
#define SK_POWERMODE_QUICK 	154		//chd_keyadd
#define SK_POWERMODE_NORMAL	155		//chd_keyadd
#define SK_GET_PLAYSTATUS	156		//chd_keyadd
//20110810
#define SK_GET_VOLUME	  	157		//chd_keyadd
//20111010
#define SK_GET_ALLPREST_IRADIO	158		//chd_keyadd
#define SK_GET_ALLPREST_FM		159		//chd_keyadd
#define SK_GET_ALLPREST_DAB		160		//chd_keyadd
//
#define SK_DIMMER_0			161		//20120316
#define SK_DIMMER_1			162		//20120316
#define SK_DIMMER_2			163		//20120316
#define SK_DIMMER_3			164		//20120316
#define SK_DIMMER_4			165		//20120316
#define SK_DIMMER_5			166		//20120316
#define SK_DIMMER_6			167		//20120316
#define SK_GET_POWER		168 	//20120316
//
#define SK_AUTO_PLAY		169 	//20120327  : RCD rotel link "play!"
#define SK_GET_DIMMER		170 	//20120328  : rotel link "get_dimmer!"
#define SK_RCD              171 	//20120328  : rotel link "rcd!"
//

#define SK_BAL_XLR			172		//20120612 : RA1570 : balanced XLR input
#define SK_BASS             173		//20120316 : RA1570 : 
#define SK_TREBLE			174		//20120316 : RA1570 : 
#define SK_BALANCE			175		//20120316 : RA1570 : 
#define SK_GET_SPEAKER      176		//20120619 : RA1570 : 
#define SK_SPEAKER_A		177		//20120619 : RA1570 : 
#define SK_SPEAKER_B		178		//20120619 : RA1570 : 
#define SK_SPEAKER_ABOff	179		//20120619 : RA1570 : 
#define SK_GET_PCUSB_CLASS  180		//20130128 : RA1570 : 
#define SK_PCUSB_CLASS_1    181		//20130128 : RA1570 : 
#define SK_PCUSB_CLASS_2    182		//20130128 : RA1570 : 

#define SK_TC_VERSION       183		//20130130 : RA1570 : 
#define SK_GET_FREQ         184		//20130131 : RA1570 : 
//
#define SK_AUX1_2           185		//20130301 : RC1570 : toggle
#define SK_OPT1_2           186		//20130408 : RC1570 : toggle
#define SK_COAX1_2          187		//20130408 : RC1570 : toggle
#define SK_D1				188		//20130806 : RA12V02: opt1 and coax1 toggle
#define SK_D2				189		//20130806 : RA12V02: opt2 and coax2 toggle
//
#define SK_GET_AMP_TRIM		190		//20130530 : RKB
#define SK_GET_TEMPERATURE	191		//20130530 : RKB
#define SK_GET_FAN_STATUS	192		//20130522 : RKB
#define SK_GET_AMP_STATUS	193		//20130708 : RKB
//
#define SK_GET_FAN_HS_ON_TEMP	194		//20140521 : RKB
#define SK_GET_FAN_HS_OFF_TEMP	195		//20140521 : RKB
//
#define SK_GET_SHUTDOWN_TEMP		196		//20130916 : RKB
#define SK_GET_SMPS_SHUTDOWN_TEMP	197		//20131024 : RKB
//
#define SK_SET_TEMPERATURE_ADC		198		//20131129 : RKB
#define SK_SET_TEMPERATURE_ON		199		//20131129 : RKB
#define SK_FACTORY_DEFAULT_SET		200		//20131129 : RKB
#define SK_AMP_PROTECTION_OFF		201		//20131129 : RKB
#define SK_AMP_PROTECTION_ON		202		//20131129 : RKB
//
#define SK_GET_DELAYTIME_VOLUME		203		//20131218 : RKB
#define SK_SET_DELAYTIME_VOLUME		204		//20131218 : RKB
//20140211 : add RS232 trim L/R control
#define SK_GET_AMPCH_STATUS			205	
#define SK_AMPCH_ON					206
#define SK_AMPCH_OFF				207
#define SK_GET_FRONT_TRIM_STATUS	208
#define SK_FRONT_TRIM_ENABLE		209
#define SK_FRONT_TRIM_DISABLE		210
#define SK_GET_VOLUME_L				211
#define SK_GET_VOLUME_R				212
//20140225
#define SK_GET_LED_DIMMER			213
//20140304
#define SK_VOL_L_UP					214
#define SK_VOL_L_DN					215
#define SK_VOL_R_UP					216
#define SK_VOL_R_DN					217
//
#define SK_GET_SMPS_FAN_ON_TEMP		218		//20140311 : RKB
#define SK_GET_SMPS_FAN_OFF_TEMP	219		//20140311 : RKB
//20140515
#define SK_GET_OPTI_POWER_MODE		220
#define SK_OPTI_POWER_ENABLE		221
#define SK_OPTI_POWER_DISABLE		222
//20140521
#define SK_GET_FAN_LS_ON_TEMP		223		//20140521 : RKB
#define SK_GET_FAN_LS_OFF_TEMP		224		//20140521 : RKB
//20150302
#define SK_GET_INPUT_SEL_MODE		225		//20150302 : RKB
#define SK_INPUT_SEL_AUTO			226		//20150302 : RKB
#define SK_INPUT_SEL_DIGITAL		227		//20150302 : RKB
#define SK_INPUT_SEL_ANALOG			228		//20150302 : RKB

#define SK_SET_BALANCE_L            229
#define SK_SET_BALANCE_R            230
#define SK_SET_FAN_LS_ON            231
#define SK_SET_FAN_LS_OFF           232
#define SK_SET_FAN_HS_ON            233
#define SK_SET_FAN_HS_OFF           234
#define SK_SET_SHUTDOWN             235
#define SK_SET_SMPS_SHUTDOWN        236
#define SK_SET_VOLUME_TX_DLY        237
#define SK_SET_SMPS_FAN_ON          238
#define SK_SET_SMPS_FAN_OFF         239
#define SK_SET_VOLUME               240
#define SK_SET_VOLUME_L             241
#define SK_SET_VOLUME_R             242
#define SK_SET_LED_DIMMER           243

#define SK_LOCAL_INPUT_A            244
#define SK_LOCAL_INPUT_B            245
#define SK_LOCAL_INPUT_C            246
#define SK_LOCAL_INPUT_D            247
#define SK_LINK_INPUT_A             248
#define SK_LINK_INPUT_B             249
#define SK_LINK_INPUT_C             250
#define SK_LINK_INPUT_D             251

#define SK_BALANCE_0                252

#define SK_REFRESH			0xff	//255


// [mode_DispBlockC][blockc_DisplayTimer]
#define BLOCKC_NORMAL				0
#define BLOCKC_FACTORYDEFAULT		1
#define BLOCKC_VERSION_DISPLAY		2
#define BLOCKC_AMP_PROTECTION	    3


//#define DIMMER_MAX		6
#define DIMMER_MAX			6						//[0/1/2/3/4/5/6]
/**********************************************************/
// eeprom addr(last backup)
//0PAGE ["upgrade"  : 010 ~ 016]
//1PAGE [TUNER AREA : 1a0 ~ 1ff]
/**********************************************************/
//current_key[6]
#define ADDR_CURRENT_KEY0			    0x00			//ADC key0
#define ADDR_CURRENT_KEY1			    0x01			//ADC key1
#define ADDR_CURRENT_KEY2			    0x02			//ADC key2
#define ADDR_CURRENT_KEY3			    0x03			//ADC key3
#define ADDR_CURRENT_KEY4			    0x04			//ADC key4
#define ADDR_CURRENT_KEY5			    0x05			//ADC key5

#define ADDR_FAN_HS_ON_TEMP			    0x06			//Fan Hs On Temp		//20140521
#define ADDR_FAN_HS_OFF_TEMP			0x07			//Fan Hs Off Temp		//20140521
#define ADDR_SHUTDOWN_TEMP			    0x08			//Shutdown Temp
#define ADDR_SMPS_SHUTDOWN_TEMP		    0x09			//SMPS Shutdown Temp
#define ADDR_SMPS_FAN_ON_TEMP		    0x0a			//SMPS Fan On Temp		//20140311
#define ADDR_SMPS_FAN_OFF_TEMP			0x0b			//SMPS Fan Off Temp		//20140311
#define ADDR_FAN_LS_ON_TEMP			    0x0c			//Fan Ls On Temp		//20140521
#define ADDR_FAN_LS_OFF_TEMP			0x0d			//Fan Ls Off Temp		//20140521

#define EEPROM_ADDR_CHECKSUM			0x0f			//check sum	
//
// 0x10~0x16 : "upgrade"
//
#define ADDR_AMP_A_ONOFF				0x20			//AMP A on/off
#define ADDR_AMP_B_ONOFF				0x21			//AMP B on/off
#define ADDR_AMP_C_ONOFF				0x22			//AMP C on/off
#define ADDR_AMP_D_ONOFF				0x23			//AMP D on/off
//
#define ADDR_A_VOL_LCH				    0x24			//AMP A LCH
#define ADDR_B_VOL_LCH				    0x25			//AMP B LCH
#define ADDR_C_VOL_LCH				    0x26			//AMP C LCH
#define ADDR_D_VOL_LCH				    0x27			//AMP D LCH
//
#define ADDR_A_VOL_RCH				    0x28			//AMP A RCH
#define ADDR_B_VOL_RCH				    0x29			//AMP B RCH
#define ADDR_C_VOL_RCH				    0x2a			//AMP C RCH
#define ADDR_D_VOL_RCH				    0x2b			//AMP D RCH
//
#define ADDR_A_BAL					    0x2c			//AMP A BAL
#define ADDR_B_BAL					    0x2d			//AMP B BAL
#define ADDR_C_BAL					    0x2e			//AMP C BAL
#define ADDR_D_BAL					    0x2f			//AMP D BAL
//
//
#define ADDR_AMP_A_INPUTSELMODE			0x30			//AMP A input sel mode [AUTO/DIGITAL/ANALOG]
#define ADDR_AMP_B_INPUTSELMODE			0x31			//AMP A input sel mode [AUTO/DIGITAL/ANALOG]
#define ADDR_AMP_C_INPUTSELMODE			0x32			//AMP A input sel mode [AUTO/DIGITAL/ANALOG]
#define ADDR_AMP_D_INPUTSELMODE			0x33			//AMP A input sel mode [AUTO/DIGITAL/ANALOG]

#define ADDR_A_TONE_SW                  0x34
#define ADDR_B_TONE_SW                  0x35
#define ADDR_C_TONE_SW                  0x36
#define ADDR_D_TONE_SW                  0x37

#define ADDR_A_TREB                     0x38
#define ADDR_B_TREB                     0x39
#define ADDR_C_TREB                     0x3a
#define ADDR_D_TREB                     0x3b

#define ADDR_A_BASS                     0x3c
#define ADDR_B_BASS                     0x3d
#define ADDR_C_BASS                     0x3e
#define ADDR_D_BASS                     0x3f

#define ADDR_A_LOCAL_SRC                0x40
#define ADDR_B_LOCAL_SRC                0x41
#define ADDR_C_LOCAL_SRC                0x42
#define ADDR_D_LOCAL_SRC                0x43

#define ADDR_A_LINK_SRC                 0x44
#define ADDR_B_LINK_SRC                 0x45
#define ADDR_C_LINK_SRC                 0x46
#define ADDR_D_LINK_SRC                 0x47
#define ADDR_DHCP_ENABLE                0x48
//
/**********************************************************/
#define ADDR_CHECKRAM					0x50				//backup ? 0xaa->0x55->0x5a
/**********************************************************/
#define ADDR_POWEROPTION				0x51        //powerOn option : < POWER_DIRECT,POWER_STANDBY,POWER_ALWAYSON >
#define ADDR_RESET						0x52        //Eeprom Factory Reset!!! < ON, off>
//20131212
#define ADDR_DISPLAYUPDATE				0x54        //ASCii command : display update : < auto, manual >
//20131212
#define ADDR_DELAY_VOLUME_TX			0x55        //Volume Data Tx delay time : default = 0 (per 500msec)
//20140212
#define ADDR_FRONT_TRIM_ENABLE			0x56        //Front Trim Knob enable(default)/disable
//20140224
#define ADDR_LED_DIMMER					0x57		//20140224 : Front LED dimming
//20140515
#define ADDR_OPTI_POWER_MODE			0x58		//20140515 : OPTICAL POWER MODE


#define ADDR_DUMMYDUMMY					0x5d	    //eeprom address change

#define ADDR_USER_IP_ADDR               0x60
#define ADDR_USER_SUBNET_MASK           0x64
#define ADDR_USER_GATEWAY_ADDR          0x68
#define ADDR_USER_DNS                   0x6c

//0x70~0x75 mac address
#define MAC_ADDR                        0x70



#define LAN8720_PHY_ADDRESS    0x00
#define USE_DHCP

extern u16 task_initialize_time;


void 	rs232_Key_Init(void);
void	Key_Init(void);

void 	main_SystemInit(void);
void 	main_Power(byte onoff);
void 	main_PowerDownCheck(void);

unsigned char	Key_InputCheck(void);
//void	proc_KeyFront(unsigned char key);
void	proc_KeyRs232(unsigned char key);

void 	Reload_DefaultBackup(void);
void	main_PowerDownCheckSumSave(void);
unsigned char	Check_Backup(void);

void 	floff_counter_250msec(void);				//20010831
unsigned char Key_DirectMainKeyCheck(unsigned char key);    //20120807

void	Get_OneByteBackup(void);				//20050625

#ifdef IAP_UPGRADE
void 	main_DisableUpgrade(void);
void 	main_iAPUpgrade(void);
#endif

void backup_reconstruct_Start(void);
void	Main_TCLS_NaviMain(unsigned char key);	//20100603
void	main_iPodKeyReset(void);				//20100604

void 	CompareTxDisplayBuffer_100msec(void);		//20110113

int getCmdID(char* comand, char* cmd_type);			//20130621
void makeRotelUTF8(unsigned short c,unsigned char* UTF8_buf, unsigned char* cnt);
//20120807
void main_AmpProtectionCheck(void);
//RKB
void Led_control_125msec(void);
void set_LED_mode(unsigned char mod);
