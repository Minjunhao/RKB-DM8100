#include "fan_ctrl.h"



FAN_STATUS fan_status;


/*
  fan status initial
*/
void fan_initial(void)
{
  fan_hs_ctrl(FAN_OFF);
  fan_ls_ctrl(FAN_OFF);
  
}
/*
  fan high speed sw initial
*/
void fan_hs_ctrl(u8 status)
{
   switch(status)
   	{
      case FAN_OFF:
	  	//FAN_hs_on(FAN_OFF);
		fan_status.fan_hs_status = FAN_OFF;
		FAN_hs_on(fan_status.fan_hs_status);
	  	break;
	  case FAN_ON:
	  	//FAN_hs_on(FAN_ON);
		fan_status.fan_hs_status = FAN_ON;
		FAN_hs_on(fan_status.fan_hs_status);
	  	break;
    }
}
/*
  fan low speed sw initial
*/
void fan_ls_ctrl(u8 status)
{
   switch(status)
   	{
      case FAN_OFF:
	  	//FAN_ls_on(FAN_OFF);
		fan_status.fan_ls_status = FAN_OFF;
		FAN_ls_on(fan_status.fan_ls_status);
	  	break;
	  case FAN_ON:
	  	//FAN_ls_on(FAN_ON);
		fan_status.fan_ls_status = FAN_ON;
		FAN_ls_on(fan_status.fan_ls_status);
	  	break;
    }   
}
u8 get_fan_running_mode(u8 temp,u8 type)
{
   u8 high_on_temp,high_off_temp,low_on_temp,low_off_temp,result;
   
   if(type == 0)
   	{
   	  high_on_temp = fan_status.fan_on_smps_temp;
	  high_off_temp = fan_status.fan_off_smps_temp;
	  
      if(temp>=high_on_temp)
      	{
          result = FAN_LS_HS_ON;
	    }
	  else if(temp>=high_off_temp)
	  	{
          result = FAN_HS_STAY;
	    }
	  else
	  	{
          result = FAN_HS_LS_OFF;
	    }
   	}
   else
   	{
   	  high_on_temp = fan_status.fan_hs_on_amp_temp;
	  high_off_temp = fan_status.fan_hs_off_amp_temp;
	  low_on_temp =   fan_status.fan_ls_on_amp_temp;
	  low_off_temp =  fan_status.fan_ls_off_amp_temp;
    

      if(temp>high_on_temp)
   	    {
          result = FAN_LS_HS_ON;
        }
      else if(temp > high_off_temp)
   	    {
          result = FAN_HS_STAY;
        }
      else if(temp>low_on_temp)
   	    {
          result = FAN_LS_ON;
        }
      else if(temp>low_off_temp)
   	    {
          result = FAN_LS_STAY;
        }
      else
   	    {
          result = FAN_HS_LS_OFF;
        }
   	}
   return result;
}
void fan_timer_check(void)
{
   u8 smps_temp,amp_temp,data;
   if(--fan_status.check_timer == 0)
   	{
   	   fan_status.check_timer = 100;
	   smps_temp = get_fan_running_mode(fan_status.smps_temperature,0);   //SMPS temperature test
	   amp_temp =  get_fan_running_mode(fan_status.amp_temperature,1);    //amp  temperature test
	   
	   data=(smps_temp>=amp_temp)?smps_temp : amp_temp;
   	   switch(data)
   	   	{
           case FAN_HS_LS_OFF:
		   	fan_hs_ctrl(FAN_OFF);
			fan_ls_ctrl(FAN_OFF);
		   	break;
		   case FAN_LS_STAY:
		   	fan_hs_ctrl(FAN_OFF);		   	
		   	break;
		   case FAN_LS_ON:
		   	fan_hs_ctrl(FAN_OFF);	
		   	fan_ls_ctrl(FAN_ON);
		   	break;
		   case FAN_HS_STAY:
		   	fan_ls_ctrl(FAN_ON);			
		   	break;
		   case FAN_LS_HS_ON:
		   	fan_hs_ctrl(FAN_ON);
			fan_ls_ctrl(FAN_ON);		   	
		   	break;
	    }
    }
}
