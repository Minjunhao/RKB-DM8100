//VOLUME SPEED (compile option)
unsigned char Timer_UpEncoder_Counter;		//20040821
unsigned char Timer_DownEncoder_Counter;	//20040821
unsigned char Backup_UpEncoder_Counter;		//20040821
unsigned char Backup_DownEncoder_Counter;	//20040821
unsigned char Adj_UpEncoder_Counter;		//20040821
unsigned char Adj_DownEncoder_Counter;		//20040821
unsigned char Timer_UpEncoder_TimeCounter;	//20040821
unsigned char Timer_DownEncoder_TimeCounter;	//20040821
unsigned char Backup_UpEncoder_TimeCounter;	//20040821
unsigned char Backup_DownEncoder_TimeCounter;	//20040821

unsigned char volspeed_enable;			//20040826
//remote volume speed
unsigned char rmc1_longkey_counter;	//20040827












#ifdef VOLUME_SPEED
void Clear_EncValue(void)
{
	Timer_UpEncoder_Counter = 0;
	Timer_DownEncoder_Counter = 0;
	Backup_UpEncoder_Counter = 0;
	Backup_DownEncoder_Counter = 0;
}
#endif
/*
rsx1067
*********************************************************************/
void 	Reset_FrontMicom(unsigned int time)
{
#ifdef VOLUME_SPEED
	Clear_EncValue();
#endif
	p_front_reset = 0;
	Wait_Absolute(time);
	p_front_reset = 1;
}	
//
//	encoder volume speed adjust
//
void	check_EncoderVolume(void)		/* 20011026 */
{                                  
	if(key_EncoderBackup == key_Value) {
		if(counter_EnoderVolumeAdjust < 3) {
			++counter_EnoderVolumeAdjust;
		}
	} else {
		key_EncoderBackup = key_Value;		
		counter_EnoderVolumeAdjust = 1;
	}
}
//rsx1067/68/56  : front encoder key(11):vol+ ,(12):vol- 
unsigned char Check_EncoderKey(void)
{
	if(key_Value <= SK_BASS_DOWN) {
		if(key_Value >= SK_VOLUME_UP) {
//20011026 s
			if(key_Value == SK_VOLUME_UP) {			//11
#ifdef VOLUME_SPEED
				Timer_UpEncoder_Counter = display_RxBuffer[3];
				Timer_UpEncoder_TimeCounter = display_RxBuffer[4];
				if (Timer_UpEncoder_Counter == 0 && Timer_UpEncoder_TimeCounter == 0) {
					volspeed_enable = 0;
				} else {
					volspeed_enable = 1;	//front volume speed adjust micom
				}
				if (volspeed_enable == 1) {
     				if (Timer_UpEncoder_Counter > Backup_UpEncoder_Counter) {
						Adj_UpEncoder_Counter = (int)Timer_UpEncoder_Counter - (int)Backup_UpEncoder_Counter;
						Backup_UpEncoder_Counter = Timer_UpEncoder_Counter;
						//time encoder
						Backup_UpEncoder_TimeCounter = Timer_UpEncoder_TimeCounter;
					}else{
						Adj_UpEncoder_Counter = 0x100 - (int)Backup_UpEncoder_Counter;
						Adj_UpEncoder_Counter = Adj_UpEncoder_Counter + (int)Timer_UpEncoder_Counter;
						Backup_UpEncoder_Counter = Timer_UpEncoder_Counter;
						//time encoder
						Backup_UpEncoder_TimeCounter = Timer_UpEncoder_TimeCounter;
					}
				}
#endif
				check_EncoderVolume();
			}else if(key_Value == SK_VOLUME_DOWN) {		//12
#ifdef VOLUME_SPEED
				Timer_DownEncoder_Counter = display_RxBuffer[3];
				Timer_DownEncoder_TimeCounter = display_RxBuffer[4];
				if (Timer_DownEncoder_Counter == 0 && Timer_DownEncoder_TimeCounter == 0) {
					volspeed_enable = 0;
				} else {
					volspeed_enable = 1;
				}
				if (volspeed_enable == 1) {
					if (Timer_DownEncoder_Counter > Backup_DownEncoder_Counter) {
						Adj_DownEncoder_Counter = (int)Timer_DownEncoder_Counter - (int)Backup_DownEncoder_Counter;
						Backup_DownEncoder_Counter = Timer_DownEncoder_Counter;
						//time encoder
						Backup_DownEncoder_TimeCounter = Timer_DownEncoder_TimeCounter;
					}else{
						Adj_DownEncoder_Counter = 0x100 - (int)Backup_DownEncoder_Counter;
						Adj_DownEncoder_Counter = Adj_DownEncoder_Counter + (int)Timer_DownEncoder_Counter;
						Backup_DownEncoder_Counter = Timer_DownEncoder_Counter;
						//time encoder
						Backup_DownEncoder_TimeCounter = Timer_DownEncoder_TimeCounter;
					}
				}
#endif
				check_EncoderVolume();
			}else {
				key_EncoderBackup = 0;
				counter_EnoderVolumeAdjust = 0;
			}	
// e
			return(TRUE);
		}else{
			return(FALSE);
		}
	}else{
		return(FALSE);
	}
}
//
//
//
unsigned char Get_UpVol_Adj(void)
{
	unsigned char data;
	
//     PutString("   j=> %",Adj_UpEncoder_Counter);
//     PutString("   t=> %\n",Backup_UpEncoder_TimeCounter);
	if (Adj_UpEncoder_Counter == 0) {			//20040825
		return(0);
	}
	if (Backup_UpEncoder_TimeCounter == 0xff) {
		return(1);
	}
	if (Adj_UpEncoder_Counter < 3) {
		if (Backup_UpEncoder_TimeCounter > 40) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 30) {
			return((Adj_UpEncoder_Counter * 2));
		}else if (Backup_UpEncoder_TimeCounter > 20) {
			return((Adj_UpEncoder_Counter * 3));
		}else if (Backup_UpEncoder_TimeCounter > 10) {
			return((Adj_UpEncoder_Counter * 4));
		}else {
			return((Adj_UpEncoder_Counter * 5));
		}
	}else if (Adj_UpEncoder_Counter < 5) {	
		// 3/4
		if (Backup_UpEncoder_TimeCounter > 36) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 26) {
			return((Adj_UpEncoder_Counter * 2));
		}else {
			return((Adj_UpEncoder_Counter * 3));
		}
	}else if (Adj_UpEncoder_Counter < 7) {	
		// 5/6 
		if (Backup_UpEncoder_TimeCounter > 36) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 26) {
			return((Adj_UpEncoder_Counter * 2));
		}else if (Backup_UpEncoder_TimeCounter > 16) {
			return((Adj_UpEncoder_Counter * 3));
		}else {
			return((Adj_UpEncoder_Counter * 4));
		}
	}else{
		// 7/8/9
		if (Backup_UpEncoder_TimeCounter > 38) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 28) {
			return((Adj_UpEncoder_Counter * 2));
		}else if (Backup_UpEncoder_TimeCounter > 18) {
			return((Adj_UpEncoder_Counter * 3));
		}else {
			return((Adj_UpEncoder_Counter * 4));
		}
	}
}
unsigned char Get_DownVol_Adj(void)
{
	if (Adj_DownEncoder_Counter == 0) {			//20040825
		return(0);
	}
	if (Backup_DownEncoder_TimeCounter == 0xff) {
		return(1);
	}
	if (Adj_DownEncoder_Counter < 3) {
		if (Backup_DownEncoder_TimeCounter > 40) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 30) {
			return((Adj_DownEncoder_Counter * 2));
		}else if (Backup_DownEncoder_TimeCounter > 20) {
			return((Adj_DownEncoder_Counter * 3));
		}else if (Backup_DownEncoder_TimeCounter > 10) {
			return((Adj_DownEncoder_Counter * 4));
		}else {
			return((Adj_UpEncoder_Counter * 5));
		}
	}else if (Adj_DownEncoder_Counter < 5) {	
		// 3/4
		if (Backup_DownEncoder_TimeCounter > 36) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 26) {
			return((Adj_DownEncoder_Counter * 2));
		}else {
			return((Adj_DownEncoder_Counter * 3));
		}
	}else if (Adj_DownEncoder_Counter < 7) {	
		// 5/6
		if (Backup_DownEncoder_TimeCounter > 36) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 26) {
			return((Adj_DownEncoder_Counter * 2));
		}else if (Backup_DownEncoder_TimeCounter > 16) {
			return((Adj_DownEncoder_Counter * 3));
		}else {
			return((Adj_DownEncoder_Counter * 4));
		}
	}else{
		// 7/8/9
		if (Backup_DownEncoder_TimeCounter > 38) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 28) {
			return((Adj_DownEncoder_Counter * 2));
		}else if (Backup_DownEncoder_TimeCounter > 18) {
			return((Adj_DownEncoder_Counter * 3));
		}else {
			return((Adj_DownEncoder_Counter * 4));
		}
	}
}
void 	Func_EncoderVolumeUp(void)
{
	char total;

#ifdef VOLUME_SPEED
			if (volspeed_enable == 1) {
				total = level_main + Get_UpVol_Adj();		//20040826
			} else {
				total = level_main + get_vol_speed();
			}
#else
			total = level_main + get_vol_speed();
#endif
			if(total > level_vol_max) {
				total = level_vol_max;
			}
			level_main = total;
			b_vol_direction = 1;			//up : tc9482
			Set_MainVolume();
			if(mute == MUTE_ON) {
				mute = MUTE_OFF;
			 	Mute_AllMuteOff();
			}
			Set_DisplayBlockCTimer(BLOCKC_VOLUME,COUNT100M_5SEC);

 }
void 	Func_EncoderVolumeDown(void)
{ 
	char total;

#ifdef VOLUME_SPEED
			if (volspeed_enable == 1) {
				if ( level_main >= Get_DownVol_Adj() ) {		
					total = level_main - Get_DownVol_Adj();		//20040826
				}else{
					total = 0;
				}
			} else {
				if ( level_main >= get_vol_speed() ) {
					total = level_main - get_vol_speed();
				}else{
					total = 0;
				}
			}
#else
			if ( level_main >= get_vol_speed() ) {
				total = level_main - get_vol_speed();
			}else{
				total = 0;
			}
#endif
			level_main = total;
			b_vol_direction = 0;			//down : tc9482
			Set_MainVolume();
			if(mute == MUTE_ON) {
				mute = MUTE_OFF;
			 	Mute_AllMuteOff();
			}
			Set_DisplayBlockCTimer(BLOCKC_VOLUME,COUNT100M_5SEC);
}
//
//
//
const char tbl_RmcVolSpeedValue[23] = { 0,1,0,1,0,1,0,1,0,2,0,2,0,2,0,2,0,2,0,3,0,3,3 };	//20070307
unsigned char Get_RmcVol_Adj(unsigned char data)
{
	return(tbl_RmcVolSpeedValue[data]);
}
//
const char tbl_VolSpeedValue[3] = { 1,2,3 };
unsigned char get_vol_speed(void)
{
	return(tbl_VolSpeedValue[0]);
}
void 	Func_RmcVolumeUp(void)
{
	char total;

	if ( f1_rmc1_longkey == 1 ) {				//20040827
		if (rmc1_longkey_counter < 21) {		//20070307
			++rmc1_longkey_counter;
		}
		total = level_main + Get_RmcVol_Adj(rmc1_longkey_counter);
	}else{
		rmc1_longkey_counter = 0;			//20040827
		total = level_main + get_vol_speed();
	}
	if(total > level_vol_max) {
		total = level_vol_max;
	}
	level_main = total;
	b_vol_direction = 1;			//up : tc9482
	Set_MainVolume();
	if(mute == MUTE_ON) {
		mute = MUTE_OFF;
	 	Mute_AllMuteOff();
	}
	Set_DisplayBlockCTimer(BLOCKC_VOLUME,COUNT100M_5SEC);
}

void 	Func_RmcVolumeDown(void)
{
	char total;

	if ( f1_rmc1_longkey == 1 ) {				//20040827
		if (rmc1_longkey_counter < 21) {		//20070307
			++rmc1_longkey_counter;
		}
		if ( level_main >= Get_RmcVol_Adj(rmc1_longkey_counter) ) {
			total = level_main - Get_RmcVol_Adj(rmc1_longkey_counter);
		}else{
			total = 0;
		}
	}else{
		rmc1_longkey_counter = 0;			//20040827
		if ( level_main >= get_vol_speed() ) {
			total = level_main - get_vol_speed();
		}else{
			total = 0;
		}
	}
	level_main = total;
	b_vol_direction = 0;			//down : tc9482
	Set_MainVolume();
	if(mute == MUTE_ON) {
		mute = MUTE_OFF;
	 	Mute_AllMuteOff();
	}
	Set_DisplayBlockCTimer(BLOCKC_VOLUME,COUNT100M_5SEC);
}
//20030108
void	Set_MainVolume(void)
{
	char front_L,front_R,subw,center,rear_L,rear_R,CB,CB1_L,CB2_R;
	char SW,CEN,RL,RR,CB1L,CB2R;

	//volume 0db, main mute on
	if(level_main == 0) {			/*chd*/
		Mute_AllMuteOn();		/*=>io_sub */
	}else{
		if(level_main < 6) {			/*chd*/
			if(mute == MUTE_OFF) {
				if(timer_MuteOff == 0) {
					Mute_SetMuteOffTime(1);		/*=>sub*/
				}
			}
		}
	}

	Write9482(IC9482_SL_FL,b_vol_direction,RL,front_L);			//tc9482
	Write9482(IC9482_SR_FR,b_vol_direction,RR,front_R);			//tc9482
	Write9482(IC9482_CEN_CB1,b_vol_direction,CEN,CB1L);			//tc9482
	Write9482(IC9482_SUB_CB2,b_vol_direction,SW,CB2R);			//tc9482
}
//
//RSP1098
//
//10:vol+ /11:vol- 
unsigned char Check_EncoderKey(void)
{
	if(f_encoder_mvol_key == ON) {
		f_encoder_mvol_key = OFF;

		if(f_mvol_updown_dir == ON) {		// ? vol up
			key_Value = K_VOLUME_UP;
#ifdef VOLUME_SPEED 					//20040825
//     PutString("B=> %",Backup_UpEncoder_Counter);
//     PutString("   N=> %",Timer_UpEncoder_Counter);
     			if (Timer_UpEncoder_Counter >= Backup_UpEncoder_Counter) {
				Adj_UpEncoder_Counter = (int)Timer_UpEncoder_Counter - (int)Backup_UpEncoder_Counter;
				Backup_UpEncoder_Counter = Timer_UpEncoder_Counter;
				//time encoder
				Backup_UpEncoder_TimeCounter = Timer_UpEncoder_TimeCounter;
			}else{
				Adj_UpEncoder_Counter = 0x100 - (int)Backup_UpEncoder_Counter;
				Adj_UpEncoder_Counter = Adj_UpEncoder_Counter + (int)Timer_UpEncoder_Counter;
				Backup_UpEncoder_Counter = Timer_UpEncoder_Counter;
				//time encoder
				Backup_UpEncoder_TimeCounter = Timer_UpEncoder_TimeCounter;
			}
//     PutString("   j=> %",Adj_UpEncoder_Counter);
//     PutString("   t=> %\n",Backup_UpEncoder_TimeCounter);
#endif
		}else{
			key_Value = K_VOLUME_DOWN;
#ifdef VOLUME_SPEED 					//20040825
     			if (Timer_DownEncoder_Counter >= Backup_DownEncoder_Counter) {
				Adj_DownEncoder_Counter = (int)Timer_DownEncoder_Counter - (int)Backup_DownEncoder_Counter;
				Backup_DownEncoder_Counter = Timer_DownEncoder_Counter;
				//time encoder
				Backup_DownEncoder_TimeCounter = Timer_DownEncoder_TimeCounter;
			}else{
				Adj_DownEncoder_Counter = 0x100 - (int)Backup_DownEncoder_Counter;
				Adj_DownEncoder_Counter = Adj_DownEncoder_Counter + (int)Timer_DownEncoder_Counter;
				Backup_DownEncoder_Counter = Timer_DownEncoder_Counter;
				//time encoder
				Backup_DownEncoder_TimeCounter = Timer_DownEncoder_TimeCounter;
			}
#endif
		}
		return(TRUE);
	}else{
		//key_EncoderBackup = 0;			//20041222
		//counter_EnoderVolumeAdjust = 0;
		return(FALSE);
	}
}
//
//
//1msec Timer ¿¡ Æ÷ÇÔ.
//
//
#ifdef VOLUME_SPEED
	if(timer_1msec_encoder < 0xff) {
		++timer_1msec_encoder;			//1msec timer encoder
	}
#endif
/*

	ENCODER Main VOLUME Chat	[ 2msec timer ]
		
             B_PORTD_YREG [ xxxx **xx ]
             B_PORTD_OLD  [ xxxx **xx ]
             B_PORTD_DATA [ xxxx **xx ]
                                 ||____ f_encoder_vol2 [f_en_mvol_dn]
                                 |_____ f_encoder_vol1 [f_en_mvol_up]
             
             B_PORTC_YREG [ xx** xxxx ]
             B_PORTC_OLD  [ xx** xxxx ]
             B_PORTC_DATA [ xx** xxxx ]
                              ||____ f_encoder_func1 [f_en_func_up]
                              |_____ f_encoder_func2 [f_en_func_dn]
*/
void	ReadEncoder(void)
{     
	unsigned char a,x;

	a = port_d;
	a = a & 0x0c;       /* WANTED-BIT '1' */	
	x = a;

	a = a & B_PORTD_OLD;
	B_PORTD_YREG = a;

	a = x;
	a = a ^ B_PORTD_OLD;
	a = a & B_PORTD_DATA;
	a = a | B_PORTD_YREG;
	B_PORTD_DATA = a;

	B_PORTD_OLD = x;
//
	a = port_c;
	a = a & 0x30;       /* WANTED-BIT '1' */
	x = a;

	a = a & B_PORTC_OLD;
	B_PORTC_YREG = a;

	a = x;
	a = a ^ B_PORTC_OLD;
	a = a & B_PORTC_DATA;
	a = a | B_PORTC_YREG;
	B_PORTC_DATA = a;

	B_PORTC_OLD = x;

//	ENCODER M.VOLUME
	if (f_encoder_mvol_ck == ON) {		/* encoder check on? */
		if (f_encoder_vol1 == 0) {		/* encoder1 input? */
			/* ready,skip */
			if (f_encoder_vol2 == 0) {		/* encoder2? up/down? */
				f_encoder_mvol_key = ON;
				f_mvol_updown_dir = ON;		/* mvol direction up */
				f_encoder_mvol_ck = OFF;		/* encoder check OFF! */
#ifdef VOLUME_SPEED
				//encoder counter :20040821
				++Timer_UpEncoder_Counter;		//
				//encoder time 
				Timer_UpEncoder_TimeCounter = timer_1msec_encoder;
				timer_1msec_encoder = 0;
#endif
			}
		} else {
			if (f_encoder_vol2 == 1) {
				f_encoder_mvol_ck = ON;	/* encoder check ON! */
			} else {
				f_encoder_mvol_key = ON;
				f_mvol_updown_dir = OFF;		/* mvol direction down */
				f_encoder_mvol_ck = OFF;		/* encoder check OFF! */
#ifdef VOLUME_SPEED
				//encoder counter :20040821
				++Timer_DownEncoder_Counter;		//
				//encoder time 
				Timer_DownEncoder_TimeCounter = timer_1msec_encoder;
				timer_1msec_encoder = 0;
#endif
			}
		}
	} else {
		if (f_encoder_vol1 == 1) {		/* encoder1 input? */
			if (f_encoder_vol2 == 1) {		/* encoder2 input? */
				f_encoder_mvol_ck = ON;		/* encoder check ON! */
			}
		}
	}

//	ENCODER func
	if (f_encoder_func_ck == ON) {		/* encoder check on? */
		if (f_encoder_func1 == 0) {		/* encoder1 input? */
			/* ready,skip */
			if (f_encoder_func2 == 0) {		/* encoder2? up/down? */
				f_encoder_func_key = ON;
				f_func_updown_dir = ON;		/* func direction up */
				f_encoder_func_ck = OFF;		/* encoder check OFF! */
			}
		} else {
			if (f_encoder_func2 == 1) {
				f_encoder_func_ck = ON;	/* encoder check ON! */
			} else {
				f_encoder_func_key = ON;
				f_func_updown_dir = OFF;		/* func direction down */
				f_encoder_func_ck = OFF;		/* encoder check OFF! */
			}
		}
	} else {
		if (f_encoder_func1 == 1) {		/* encoder1 input? */
			if (f_encoder_func2 == 1) {		/* encoder2 input? */
				f_encoder_func_ck = ON;		/* encoder check ON! */
			}
		}
	}
}
