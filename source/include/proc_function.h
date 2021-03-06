#ifndef _PROC_FUNC
#define _PROC_FUNC
#define IS_CHANNEL(x)     (x<=4)?0:(1)
#define IS_NULL(x)        ((x)==NULL)? 1:0
#define CONSTRUCT_TABLE_SIZE 54

#define CHANNEL_A   0
#define CHANNEL_B   1
#define CHANNEL_C   2
#define CHANNEL_D   3
#define ALL_CHANNEL 4
#define ENTIRE_MODE ALL_CHANNEL

#define MUTE_CH_A   0
#define MUTE_CH_B   1
#define MUTE_CH_C   2
#define MUTE_CH_D   3
#define MUTE_ALL    4


#define INPUT_MODE_ANALOG   0
#define INPUT_MODE_DIGITAL  1
#define INPUT_MODE_AUTO     2

extern u8  mode_display_update;
extern u8  volume_send_delay_time;
extern u8  adc_mode;


typedef struct ak4117_chip {
  unsigned char bit_map;          //flag:bit0-status, bit1-freq ,bit3-stream_kind                  //0-no change, 1-need change
  unsigned char status;           //initial step
  unsigned char freq;
  unsigned char pll_status;
  unsigned char stream_kind;
  unsigned char pll_check_time;
  unsigned char mode_status;
  
} AK4117_TYPE;
typedef struct{
  unsigned char vol;       //volume: 0~96
  unsigned char tone_sw;
  unsigned char vol_left;
  unsigned char vol_right;
  unsigned char bass_level;
  unsigned char treb_level;
  unsigned char balance;
} VOL_CTRL_TYPE;

typedef struct channel {
  unsigned char channel;
  unsigned char chip_id;
  unsigned char digital_nAnalog;       //njm2752 for selecting
  unsigned char digital_analog_mode;
  AK4117_TYPE   *ak4117_data;
  VOL_CTRL_TYPE *vol_data;
  unsigned char local_speaker_src;  //local speaker output   bit0~3: input channel sel, bit4:0-output on, 1-output off
  unsigned char link_channel_src;    //select link source to output
  unsigned char amp_channel_on;
  unsigned char mute_status;

  void (*p_ak4117_func)(struct channel *);       //for AK4117 control
  
  void (*p_digital_nAnalog_sw)(struct channel *,unsigned char);
  void (*p_local_speaker_src_sel)(struct channel *,unsigned char);
  void (*p_link_chanel_src_sel)(struct channel *,unsigned char ); 
  
  void (*p_vol_control)(struct channel *,unsigned char,unsigned char,unsigned char);
  void (*p_vol_ctrl_l)(struct channel*, unsigned char);
  void (*p_vol_ctrl_r)(struct channel*, unsigned char);
  void (*p_bass_control)(struct channel *, unsigned char,unsigned char);
  void (*p_treb_control)(struct channel *, unsigned char,unsigned char);
  void (*p_balance_contrl)(struct channel *,unsigned char,unsigned char);
  void (*p_mute_ctrl)(struct channel *,unsigned char);
  void (*p_tone_control)(struct channel *,unsigned char);
  void (*p_eeprom_write)(unsigned char,unsigned char);
} Channel_TYPE;


void njm2752_ctrl(Channel_TYPE *chunnel,u8 digital_nAnalog);

extern Channel_TYPE RKB_Channel[4];

void	Power_SetPowerStatus(unsigned char stat);
unsigned char	Power_GetPowerStatus(void);
void	Mute_CheckMuteOff_100msec(void);


void 	PowerOn_StandBy(unsigned char boot);
void	Func_PowerStandByOn(void);
void 	Func_PowerAcOn(void);
void	Func_PowerOff(void);
void	Func_PowerOn(void);
void	Power_toggle(void);
void 	Func_PowerControl(void);
void	Func_SeperatePowerOn(void);
void	Func_SeperatePowerOff(void);
unsigned char Check_FrontKeySkip_PowerOff(void);
void 	Set_Function(void);
void	Func_TriggerJackCheck_Power(void);

void	StandbyRelay_Off(void);
void	StandbyRelay_On(void);

void Mute_AllMuteOn(void);
void Mute_AllMuteOff(void);
void Mute_SetMuteOffTime(unsigned char time);

void	Func_PowerOff_PortLow(void);		//20091015

//20140219
void SMPS_On(void);
void SMPS_Off(void);

//RKB
void NJW1159V_AllMuteOn(void);	//
void NJW1159V_AmpMuteOn(unsigned char typ);
void	FAN_check(void);
//20131216
void	PowerOn_exec_1(void);
void	PowerOn_exec_DIR(void);
void	PowerOn_exec_2(void);
void	PowerOn_exec_3(void);
//20140224
void Check_DirectMuteOnOff(void);
//20140523
void signal_sensingPort_check(void);
void backup_range_check(u8 type);
void backup_reconstruct(unsigned char i, unsigned char *addr,unsigned char flag);
void mode_reset(void);
void mode_mute_on(u8 chunnel);
void RKB_channel_initial(void);
void mode_amp_protection_check(void);
void mode_ak4117_check(void);
void mode_status_initial(void);
void mode_input_check(void);
void ethernet_backup_default(void);
#endif
