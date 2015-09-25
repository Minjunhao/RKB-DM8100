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
#define MODE_ID_02       0x02
#define MODE_ID_03       0x03
#define MODE_ERROR       0xff
#define RESPONSE_SIZE    50

#define COMMAND_SIZE     35

#define ADC_DISPLAY_ON    1
#define ADC_DISPLAY_OFF   0

#define ISDIGIT(x) ((x)>='0' && (x)<='9')
#define IS_ENTIRE_CHANNEL(x)  if((x)!=ENTIRE_MODE)\
	                                {return;}

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

bool key_ethernet_check(void);
bool rs232_key_parse(void);
u8 get_mode_id(u8 * cmd,u8 num);
void rs232_key_process(STRING_CMD_TYPE * cmd);
void reply_product_version(u8 chnl);
void data_send(u8 * data,u8 size ,u8 send_kind);
void reply_product_type(u8 chnl);
u8 get_info_from_command(STRING_CMD_TYPE *key_struct);
void proc_value_set(u8 cmd_key,u8 cmd_para,u8 chnl,u8 *p_cmd);
void proc_input_channel_set(u8 cmd_key,u8 chnl);
void proc_normal_cmd(u8 cmd_key,u8);
void key_input_check(void);
u8 get_freq_index(u8 data);
void freq_send_out(u8 chnl);
void reply_current_freq(u8 chnl);
void reply_fan_status(u8 chnl);
u8 rotel_strncpy(u8 *des,const u8 *src,u8 num);
u8 rotel_string_split(u8 *array,u8 *src,u8 comma,u8 size);
void reply_amp_status(u8 chnl);
void reply_fan_hs_on_temp_val(u8 chnl);
void reply_fan_hs_off_temp_val(u8 chnl);
void reply_shutdown_temp_val(u8 chnl);
void reply_fan_ls_on_temp_val(u8 chnl);
void reply_fan_ls_off_temp_val(u8 chnl);
void reply_smps_fan_on_temp_val(u8 chnl);
void reply_smps_fan_off_temp_val(u8 chnl);
void reply_mode_power_status(u8 chnl);
void set_amp_protection(u8 val);
void reply_display_update_status(u8 chnl);
void reply_delay_to_send_volume(u8 chnl);
void reply_mute_status(u8 chnl);
void reply_input_sel_mode(u8 chnl);
u8 add_prefix(u8 *des,u8 prefix_id);
void set_display_update(u8 data);
void rs232_key_type_init(void);
void reply_current_temperature(u8 chnl);
void reply_amp_chanel_status(u8 chnl);
void reply_volume(u8 chnl);
void reply_volume_left(u8 chnl);
void reply_volume_right(u8 chnl);
void reply_volume_max(u8 chnl);
void reply_volume_min(u8 chnl);
void reply_balance(u8 chnl);
void reply_ip_address(u8 chnl);
void reply_subnet_mask(u8 chnl);
void reply_gateway(u8 chnl);
void reply_dns_server(u8 chnl);
void reply_ip_mode(u8 chnl);
void reply_network_status(u8 chnl);
void reply_mac_addr(u8 chnl);
void exec_mac_addr_set(u8 chnl,u8 para,u8 *p_cmd);
void exec_static_ip_set(u8 chnl,u8 para,u8 *p_cmd);
void exec_static_gateway_set(u8 chnl,u8 para,u8 *p_cmd);
void exec_static_subnet_mask_set(u8 chnl,u8 para,u8 *p_cmd);
void exec_static_dns_set(u8 chnl,u8 para,u8 *p_cmd);


void exec_mode_power_on(u8 chnl);
void exec_mode_power_off(u8 chnl);
void exec_mode_power_toggle(u8 chnl);

void exec_amp_protection_off(u8 chnl);
void exec_amp_protection_on(u8 chnl);

void exec_set_display_update_auto(u8 chnl);
void exec_set_display_update_manual(u8 chnl);

void exec_mute_toggle(u8 chnl);
void exec_mute_on(u8 chnl);
void exec_mute_off(u8 chnl);

void exec_volume_set(u8 chnl,u8 data);
void exec_volume_right_set(u8 chnl,u8 data);
void exec_volume_left_set(u8 chnl,u8 data);
void exec_volume_up(u8 chnl);
void exec_volume_down(u8 chnl);
void exec_volume_left_up(u8 chnl);
void exec_volume_left_down(u8 chnl);
void exec_volume_right_up(u8 chnl);
void exec_volume_right_down(u8 chnl);

void exec_balance_left(u8 chnl);
void exec_balance_right(u8 chnl);
void exec_set_temperature_adc(u8 chnl,u8 val);
void exec_input_sel_mode(u8 chnl,u8 val);
void exec_amp_ch_on(u8 chnl);
void exec_amp_ch_off(u8 chnl);
void exec_factory_default(u8 chnl);
void exec_balance_left_set(u8 chnl,u8 data);
void exec_balance_right_set(u8 chnl,u8 data);
void exec_ip_mode_config(u8 chnl,u8 val);

#endif