#define	INIT_SCROLL_TIME 	TIME20MSTO1000MS
#define	SCROLL_TIME 		TIME20MSTO340MS



void	Set_DisplayBlockCTimer(unsigned char block,unsigned char display_time);
void	Display_UpdateDisplayBuffer_20msec(void);
void	Display_TempBufferClear(void);
void	Display_PowerAllBufferClear(void);
void	Display_BufferClear(void);
void	Display_Initial(void);
void	Display_Block(void);
void	Display_PowerOffClearTimer(void);

void	Display_AllBufferClear(void);
void	Display_RS232DispBufferClear(void);
void	Display_FrontDispBufferClear(void);


void	Display_BlockCModeCheck_100msec(void);

void	Display_VersionDisplay(void);

//20110314
void 	Func_DisplayAll(void);
void 	Func_Display1234(unsigned char j);
void 	Func_ProductType(void);
void 	Func_ProductVersion(void);
void 	Func_GetVolumeMax(void);
void 	Func_GetVolumeMin(void);
//20110317
void 	Func_PowerStatus(void);
//20120807
void Stingray_Tx_RotelOpaqueData(unsigned short Ssize);
//20130520
void addID_RoteOpaqueData(unsigned char mode, unsigned char AMPtype, unsigned short* m);
//20130520
void 	Func_CurrentFreqTx(unsigned char typ);
void 	Func_AmpTemperatureCurrentStatus(unsigned char typ);
void 	ListenFunctionAutoChange(void);
//20140521
void 	Func_FAN_HS_Status(void);
//20130527
void Display_RKB_Function_1LineVolume(unsigned char typ);
//20130708
void 	Func_AMPStatus(void);
//20130712
void 	proc_KeyRs232_FanHsOnSet(unsigned char val);
void 	proc_KeyRs232_FanHsOffSet(unsigned char val);
void 	Func_FanHsOnStatus(void);
void 	Func_FanHsOffStatus(void);
//20140521
void 	proc_KeyRs232_FanLsOnSet(unsigned char val);
void 	proc_KeyRs232_FanLsOffSet(unsigned char val);
void 	Func_FanLsOnStatus(void);
void 	Func_FanLsOffStatus(void);

void 	Func_ProtectionStatus(unsigned char mod);
void 	proc_KeyRs232_ShutdownSet(unsigned char val);
void 	Func_getShutdownTemp(void);
//20131024
void 	proc_KeyRs232_SmpsShutdownSet(unsigned char val);
void 	Func_getSmpsShutdownTemp(void);
//20131212
void 	Func_SetDisplayUpdate(unsigned char mode);
void 	Func_DisplayUpdateStatus(void);
//20131218
void 	Func_SetDelayTimeVolumeDataTx(unsigned char val);
void 	Func_getDelayTimeVolumeDataTx(void);
//20140108
void 	Func_PowerOnCommand_Tx(void);
void 	Func_PowerOffCommand_Tx(void);
//20140213
void 	Func_GetCurrentBalance(unsigned char ampType);
void 	Func_GetVolumeMax(void);
void 	Func_GetVolumeMin(void);
void 	Func_VolumeLchStatus(unsigned char ampType);
void 	Func_VolumeRchStatus(unsigned char ampType);
//20140213
void 	Func_MuteStatus(unsigned char type, unsigned char ampType);
void 	Func_AMP_CH_status(unsigned char ampType);
void 	Func_AMP_CH_on(unsigned char ampType);
void 	Func_AMP_CH_off(unsigned char ampType);
void 	Func_frontTrimStatus(void);
void 	Func_frontTrim_enable(void);
void 	Func_frontTrim_disable(void);
//20140220
void 	Func_GetCurrentFreq(unsigned char ampType);
void 	Func_GetCurrentTrim(unsigned char ampType);
//20140224
void 	Func_ledDimmerStatus(void);
void 	Func_LED_Dimmer_set(unsigned char val);
//20140311
void 	Func_SMPSFanOnStatus(void);
void 	Func_SMPSFanOffStatus(void);
void 	proc_KeyRs232_SMPSFanOnSet(unsigned char val);
void 	proc_KeyRs232_SMPSFanOffSet(unsigned char val);
//20140404
void 	Func_VolumeLRStatus(unsigned char ampType);
//20140515
void 	Func_opticalPowerModeStatus(void);
void 	Func_opticalPower_enable(void);
void 	Func_opticalPower_disable(void);
//20150302
void 	Func_inputSelStatus(unsigned char ampType);
void 	Func_inputSel_auto(unsigned char ampType);
void 	Func_inputSel_digital(unsigned char ampType);
void 	Func_inputSel_analog(unsigned char ampType);
/* */
