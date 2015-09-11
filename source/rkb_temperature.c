#include "rkb_temperature.h"


void temperature_read(void)
{
    u8 i;
    for (i=0;i<4;i++) {
		if (key.KeyOk[i] == ON) {
			key.KeyOk[i] = OFF;
			key_target[i] = key.KeyData[i];
			if ( current_key[i] != key_target[i] ) {
				current_key[i] = key_target[i];				
				if ( (i == 0)&&( (key.KeyOk[1] == ON)||(key.KeyOk[2] == ON)||(key.KeyOk[3] == ON) ) ) {
				}else if ( (i == 1)&&( (key.KeyOk[2] == ON)||(key.KeyOk[3] == ON) ) ) {
				}else if ( (i == 2)&&( (key.KeyOk[3] == ON) ) ) {
				}else{
					Func_AmpTemperatureCurrentStatus();	
					mode_temperature_check();
				}			
			}
		}	
	}	//for  
}
/*
  update the temperature
*/
void mode_temperature_check(void)
{
    u8 i;
	
	fan_status.smps_temperature = ((current_key[0]>=current_key[1])? (current_key[0]):(current_key[1]));

    fan_status.amp_temperature = ((current_key[2]>=current_key[3])?(current_key[2]):(current_key[3]))
	
}

