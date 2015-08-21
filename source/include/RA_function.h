void TC9162_Tx(unsigned char t_data);
//void TC9482_Tx(unsigned char v_data, unsigned char ch);
void TC9482_Tx(unsigned char v_data, unsigned char ch, unsigned char direction);
void AK4104_init(void);

//RCX
void	Func_CD(void);
void	Func_Aux2(void);
void	Func_Eject(void);

void	Func_VolUp(void);
void	Func_VolDown(void);
void	Func_Mute(void);
void	Func_MuteOn(void);
void	Func_MuteOff(void);

void 	Func_EncoderVolumeUp(void);
void 	Func_EncoderVolumeDown(void);
void	Set_MainVolume(void);
void 	Func_RmcVolumeUp(void);
void 	Func_RmcVolumeDown(void);

void	proc_KeyRs232_MZ_VOL(unsigned char key);
void	Function_SelectStart(unsigned char new_func);   //20120619
void 	Func_speaker_A(void);
void 	Func_speaker_B(void);

void 	Func_bypass(void);
void 	Func_bass(void);
void 	Func_treble(void);
void 	Func_balance(void);
//RA
void s_PGA2310_Tx16(unsigned char l_data, unsigned char	r_data);
void	Function_RotelLinkRCD(void);
void 	Func_DirectBypassSet(unsigned char type);
void 	Func_BassUp(void);
void 	Func_BassDown(void);
void 	Func_DirectBassSet(unsigned char bas);
void 	Func_TrebleUp(void);
void 	Func_TrebleDown(void);
void 	Func_DirectTrebleSet(unsigned char trb);
void 	Func_BalanceLeft(void);
void 	Func_BalanceRight(void);
void 	Func_DirectBalanceSet(unsigned char bal);
void 	Func_SpeakerStatus(void);
void	set_SPEAKER_Relay(unsigned char spk_type);
void	Set_ToneInitialVolumeTx(void);
void	Set_ToneVolume(void);
void 	Func_DimmerTx(unsigned char type,unsigned char val);

void	Display_SetupTone(unsigned char pas, unsigned char bas, unsigned char trb);
void 	Func_SetupBypass_UpEnd(void);		
void 	Func_SetupBypass_DownEnd(void);
void 	Func_SetupBass_UpEnd(void);
void 	Func_SetupBass_DownEnd(void);
void 	Func_SetupTreble_UpEnd(void);
void 	Func_SetupTreble_DownEnd(void);				

void	Display_SetupBalance(unsigned char bal);
void 	Func_SetupBalance_UpEnd(void);
void 	Func_SetupBalance_DownEnd(void);

void	Display_SetupRotelLinkCDP(unsigned char func);
void 	Func_RotelLinkCDP_UpEnd(void);
void 	Func_RotelLinkCDP_DownEnd(void);

void 	Func_Aux12Toggle(void);
void 	Func_Opt12Toggle(void);     //20130408
void 	Func_Coax12Toggle(void);
