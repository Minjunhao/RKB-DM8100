//RKB

void s_NJW1159_Tx16(unsigned char t_data, unsigned char	t_type);
void 	Func_AmpTrimCurrentStatus(unsigned char typ);
void s_NJW1159PortInit(void);											//20140211
//20140213
void	Set_MainVolume(unsigned char vol, unsigned char ampType);
void	Set_MainVolumeLeft(unsigned char L_ch, unsigned char ampType);
void	Set_MainVolumeRight(unsigned char R_ch, unsigned char ampType);
void 	func_DirectBalanceSet(unsigned char bal, unsigned char ampType);
void	Func_MuteFlagAllCH_Off(void);
void	Func_MuteFlag_CH_Off(unsigned char ampType);
void	Func_Mute(unsigned char ampType);
void	Func_MuteOn(unsigned char ampType);
void	Func_MuteOff(unsigned char ampType);
//20140304
void	Func_VolUp(unsigned char ampType);
void	Func_VolDown(unsigned char ampType);
void	Func_VolLeftChUp(unsigned char ampType);
void	Func_VolLeftChDown(unsigned char ampType);
void	Func_VolRightChUp(unsigned char ampType);
void	Func_VolRightChDown(unsigned char ampType);
//20140408
void 	Func_BalanceRight(unsigned char ampType);
void 	Func_BalanceLeft(unsigned char ampType);

