#ifndef _CMD_KEY 
#define _CMD_KEY 
#define TUNNEL_ALL       0x01
#define TUNNEL_PC        0x02
#define TUNNEL_ETH       0x04
#define TUNNEL_UART4     0x08
#define TUNNEL_UART5     0x10

#define UNDEFINE         0xff

#define KEY_SET          0x01        //SET VOLUME,BALANCE,TEMPERATURE
#define KEY_INPUT_SET    0x02        //INPUT CHANNEL SET
#define KEY_NORMAL_CMD   0x03        //MUTE etc


#define MODE_ID_00       0x00
#define MODE_ID_01       0x01
#define MODE_ERROR       0xff
#define RESPONSE_SIZE    50

#define COMMAND_SIZE     35

#define ISDIGIT(x) ((x)>='0' && (x)<='9')

typedef struct
{
  unsigned char key_to_be_pro;
  unsigned char unit_id;
  unsigned char cmd_for_channel;
  unsigned char id;
  unsigned char *p_cmd;
  unsigned char cmd_key;
  unsigned char key_para;
  unsigned char key_type;
} STRING_CMD_TYPE;

bool rs232_key_parse(void);
u8 get_mode_id(u8 * cmd);
void rs232_key_process(STRING_CMD_TYPE * cmd);
void reply_product_version(void);
void data_send(u8 * data,u8 size ,u8 send_kind);
void reply_product_type(void);
u8 get_info_from_command(STRING_CMD_TYPE *key_struct);
void proc_value_set(u8 cmd_key,u8 cmd_para,u8 chnl);
void proc_input_channel_set(u8 cmd_key,u8 chnl);
void proc_normal_cmd(u8 cmd_key,u8);
void key_input_check(void);
u8 get_freq_index(u8 data);
void freq_send_out(u8 chnl);
void reply_current_freq(u8 chnl);
void reply_fan_status(void);
u8 rotel_strncpy(u8 *des,const u8 *src,u8 num);
void reply_amp_status(void);
void reply_fan_hs_on_temp_val(void);
void reply_fan_hs_off_temp_val(void);
void reply_shutdown_temp_val(void);
void reply_fan_ls_on_temp_val(void);
void reply_fan_ls_off_temp_val(void);
void reply_smps_fan_on_temp_val(void);
void reply_smps_fan_off_temp_val(void);
void reply_mode_power_status(void);
void set_amp_protection(u8 val);
void reply_display_update_status(void);
void reply_delay_to_send_volume(void);
void reply_mute_status(u8 chnl);
void reply_input_sel_mode(u8 chnl);
u8 add_prefix(u8 *des,u8 prefix_id);
void set_display_update(u8 data);
void rs232_key_type_init(void);


void exec_mode_power_on(void);
void exec_mode_power_off(void);
void exec_mode_power_toggle(void);

void exec_amp_protection_off(void);
void exec_amp_protection_on(void);

void exec_set_display_update_auto(void);
void exec_set_display_update_manual(void);

void exec_mute_toggle(u8 chnl);
void exec_mute_on(u8 chnl);
void exec_mute_off(u8 chnl);

void exec_volume_up(u8 chnl);
void exec_volume_down(u8 chnl);
void exec_volume_left_up(u8 chnl);
void exec_volume_left_down(u8 chnl);
void exec_volume_right_up(u8 chnl);
void exec_volume_right_down(u8 chnl);

void exec_balance_left(u8 chnl);
void exec_balance_right(u8 chnl);
#endif