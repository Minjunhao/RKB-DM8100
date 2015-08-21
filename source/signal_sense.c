#include "signal_sense.h"

SS_TYPE ss_status;
AMP_TYPE   amp_status;

void signal_sense_initial(void)
{
   //ss_status.amp_status = 0;
   ss_status.chk_timer  = 0;
   ss_status.low_counter= 0;
   ss_status.mode_on    = 0;
   ss_status.noise_counter = 0;
   ss_status.reset_counter = 0;
}

void amp_status_initial(void)
{
   amp_status.check_timer = 0;
   amp_status.protect_status = 0;
   //amp_status.status = 0;
}
/*
   amp status check
*/
void amp_check(void)
{
   if(--amp_status.check_timer == 0)
   	{
       if(get_amp_n_ready)
       	{
	   	  amp_status.protect_status = AMP_PROTECT_OFF;    //OFF--amp ready
		  //AMP_On();
       	}
	   else
	   	{
          amp_status.protect_status = AMP_PROTECT_ON;   
		  //AMP_Off();
	    }
	   if(get_amp_n_err)
	   	{
          amp_status.protect_status = AMP_PROTECT_OFF;    //OFF--amp no error
	    }
	   else
	   	{
          amp_status.protect_status = AMP_PROTECT_ON;
	    }
	  amp_status.check_timer = AMP_CHECK_PERIOD
    }
}