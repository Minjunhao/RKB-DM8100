/*
	File name: rmc2.soa

	Contents:

	-카운트가 0xff에서 하강 에지 감지시 카운트 리셋(아주 중요함)
		(카운트가 0xff아닌 상태에서 하강 에지에 카운트 리셋 않함)
	-상승에지에서 카운트값을 FIFO에넣고 카운트 리셋, 인트럽트 발생.
	-카운트가 0xff에 도달시 FIFO에 값을 넣고 인트럽트 발생.
		(수신완료후 0ffh때문에 한번더 인트럽트 걸림)
	-카운트가 0ffh상태에서는 카운트 리셋도 하지않고 인트럽트를 
		발생시키지도 않음.

	로칼 변수:
		f1_rmc2_repeat - 연속 키 
		b1_rmc2_mode - 리모콘 상태 
		b1_rmc2_flamecount - 플레임 측정 
		b1_rmc2_bitcount - 빗 카운트 
		d1_rmc2_data
		d1_rmc2_custom

	글로벌 변수:
		f1_rmc2_key - 키 입력 
		f1_rmc2_longkey - 롱키 발생 
		f1_rmc2_keyoff - 키 오프 
		b1_rmc2_key - 리모콘 키 
		b1_rmc2_longkeycount - 연속키 추적 
	
	Revision history:

*********************************************************************

	|--||||||||_____|--||_____count? _____|--||____|--||_____________
	
	           key                            key      key         key
	           keypress                       longkey  repeatkey   keyoff
                                                       
longcount      3*150ms       (450ms)          0
flamecount     180ms     180ms ....           180ms    180ms       180ms 
repeatcount                                   160ms    100ms
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "remocon.mem"
#include "remocon.h"

#include "function.ext"
#include "proc_display.ext"

/*
	 9    msec		1st leader
	 5.06 msec = 4.5+.56 	2nd leader
	 2.81 msec = 2.25+.56 	repeat code
	 2.25 msec 		bit 1
	 1.125msec 		bit 0
*/
#ifdef RMC_TIMER_200US
#define RT_COUNT	206
#else
#define RT_COUNT	256
#endif

#define	RT_OVFLW	0xff   
#define	RT_30MS		30000 / RT_COUNT
#define	RT_15MS		15000 / RT_COUNT
#define	RT_13MS		13000 / RT_COUNT
#define	RT_11MS		11000 / RT_COUNT
#define	RT_9MS		9000 / RT_COUNT
#define	RT_7MS		7000 / RT_COUNT
#define	RT_6MS		6000 / RT_COUNT
#define	RT_5_5MS	5500 / RT_COUNT
#define	RT_5MS		5000 / RT_COUNT
#define	RT_4MS		4000 / RT_COUNT
#define	RT_3_5MS	3500 / RT_COUNT
#define	RT_2700US	2700 / RT_COUNT
#define	RT_2550US	2550 / RT_COUNT
#define	RT_2250US	2250 / RT_COUNT
#define	RT_1687US	1687 / RT_COUNT
#define	RT_1_5MS	1500 / RT_COUNT
#define	RT_900US	900 / RT_COUNT
#define	RT_825US	825 / RT_COUNT


/* remocon mode */
#define RM_READY	0
#define RM_LEADER	1
#define RM_DATA		2

/* counter mode */
#define CM_READY	0
#define CM_LOW		1
#define CM_HIGH		2

/* count */
#define COUNT_FLAME			TIME10MSTO180MS
#define COUNT_FLAME_REPEAT	TIME10MSTO180MS


//#define COUNT_REPEATSTART	COUNT100M_1000MS
//#define COUNT_REPEATTIME	COUNT100M_800MS
/* 주의! COUNT_REPEATTIME = COUNT_REPEATSTART - 리피트간격 */

#define COUNT_LONGKEY	3	/* 30키가 입력되면 롱키 */		//150msec *3 = 450msec
//----------------------------------------------------------
#define CODE1_CUSTOM	0x1183		//Audio mode - 1
#define CODE11_CUSTOM	0x2183		//Audio/Tuner mode - 2

#define CODE2_CUSTOM	0x0983		//Tuner mode - 1
#define CODE21_CUSTOM	0x2183		//Audio/Tuner mode - 2

#define CODE3_CUSTOM	0x1283		//CD mode
#define CODE4_CUSTOM	0x1483		//USB mode
#define CODE5_CUSTOM	0x1583		//PC-USB mode
//----------------------------------------------------------
void	rmc1_restart(void);
void	rmc1_repeat(void);
void	rmc2_restart(void);
void	rmc2_repeat(void);
void	rmc3_restart(void);
void	rmc3_repeat(void);
void	rmc4_restart(void);		//jyj_merge
void	rmc4_repeat(void);		//jyj_merge
void	rmcE_restart(void);
void	rmcE_repeat(void);

unsigned char chk_customCode(unsigned int customCode);

#ifdef RMC4_ENABLE
//remote4
//
//
//	RMCLR		1
//	EDGSEL		0
//	RMISEL		1
//	RMPOL		0
//	RCK1,RCK0	1,0	51.2usec
//	FCK1,FCK0	1,0	25.6usec
//**********************************************************************/
/* jyj_merge : RMC34_ADD start */
void rmc4_Init(void)
{
	f1_rmc4_repeat = 0;
	f1_rmc4_repeatkey = 0;
	b1_rmc4_flamecount = 0;
	b1_rmc4_repeatcount = 0;
	b1_rmc4_mode = RM_READY;
	b1_rmd4_mode = CM_READY;

	// global var
	f1_rmc4_key = 0;
	f1_rmc4_keypress = 0;
	f1_rmc4_longkey = 0;
	f1_rmc4_keyoff = 0;
}

//
//	인트럽트 서비스 루틴.(499usec)
//********************************************************************
#define 	RMC4_INT_ACTIVE		1
#define 	RMC4_INT_NOACTIVE	0


//
//
//	remocon 인터럽트 루틴 high edge까지의 시간을 return
//
//********************************************************************
void rmc4_Timer_250usec(void)
{
	switch ( b1_rmd4_mode ) {
	case CM_READY:
		if ( p_rmc_zone4 == 0 ) {
			b1_rmd4_copy = 1;
			b1_rmd4_mode = CM_LOW;
		}
		b1_rmc4_status = FALSE;
		break;
	case CM_LOW:
		if ( p_rmc_zone4 == 0 ) { 
			// low count up
			if ( b1_rmd4_copy < 0xff ) {
				++b1_rmd4_copy;
			}
			b1_rmc4_status = FALSE;
		}else{
			// high되는 시점에 rmc처리루틴 on
			b1_rmd4_mode = CM_HIGH;
			b1_rmd4 = b1_rmd4_copy;		// low count값 return
			b1_rmd4_copy = 1;
			b1_rmc4_status = TRUE;
		}
		break;
		
	case CM_HIGH:
		if ( p_rmc_zone4 == 1 ) { 
		
			// high count up
			if ( b1_rmd4_copy < RT_30MS ) {
				++b1_rmd4_copy;
				b1_rmc4_status = FALSE;
			} else {
				// 오버플로우
				b1_rmd4_mode = CM_READY;
				b1_rmd4 = b1_rmd4_copy;
				b1_rmc4_status = FALSE;
			}
		}else{
			b1_rmd4_mode = CM_LOW;
			b1_rmc4_status = FALSE;
			if (b1_rmc4_mode == RM_READY) {		//jyj_rmcreader
				b1_rmd4_copy = 1;
			}
		}
		break;
	default:
		b1_rmd4_mode = CM_READY;
		b1_rmc4_status = FALSE;
		break;
	}
	rmc4_status = b1_rmc4_status;
}


//
//
//	remocon 인터럽트 루틴 high edge까지의 시간을 return
//
//********************************************************************
void rmc4_process(void) 
{
	unsigned short masking;

	switch ( b1_rmc4_mode ) {
	// 9msec짜리 첫번째 리드코드를 기다린다.
	case RM_READY:
		if ( b1_rmd4 != RT_OVFLW ) {
			// 마지막 타임아웃
			if( RT_7MS <= b1_rmd4) {
				if( b1_rmd4 < RT_15MS ) {
					b1_rmc4_mode = RM_LEADER;      
				}
			}
		}
		break;
	case RM_LEADER:
		// 5.06msec == 1st flame
		// 2.81msec == repeat flame
		
		// 6msec보다 크면 error로 restart.
		if ( b1_rmd4 >= RT_9MS ) {
			rmc4_restart();
		}else{
			// 4ms보다 작고 2.25ms보다 크면 repeat인가 검사.
			if (RT_2250US <= b1_rmd4 ) {
				if( b1_rmd4 < RT_4MS ) {
					if ( f1_rmc4_repeat == 1 ) {
						rmc4_repeat();
					}else{
						rmc4_restart();
					}
				}else{
					// 4ms < b1_rmd2 < 6mse
					// 데이타 플레임
					b1_rmc4_mode = RM_DATA;
					b1_rmc4_bitcount = 32;
					RMC4_DATA.iValue = 0;
					rmc4_custom = 0;
					//20091218 //chd : repeat 바로이어서(짧은시간) -> 새로운 code start!!
					f1_rmc4_longkey = 0;
					f1_rmc4_repeat = 0;
					b1_rmc4_flamecount = 0;
					b1_rmc4_repeatcount = 0;

				}
			}else{
				rmc4_restart();
				
			}
		}
		break;
	case RM_DATA:
		// 범위 검사
		if ( b1_rmd4 >= RT_2700US ) {
			rmc4_restart();
		}else{
			if( b1_rmd4 >= RT_900US ) {
			//  0/1 검사
				if ( b1_rmd4 >= RT_1687US ) {
					b1_rmc4_masking = 0x8000;
				} else {
					b1_rmc4_masking = 0x0000;
				}
			
				// custome code 를 수신..
				if(b1_rmc4_bitcount >= 17) {
					rmc4_custom >>= 1;
					rmc4_custom &= 0x7fff;
					rmc4_custom |= b1_rmc4_masking;
				}else{
					// data수신..
					RMC4_DATA.iValue >>= 1;
					RMC4_DATA.iValue &= 0x7fff;
					RMC4_DATA.iValue |= b1_rmc4_masking;
				}
				if(--b1_rmc4_bitcount == 0){	
					// 커스텀 코드 검사
#if 1	//jyj_zoneir
					if((rmc4_custom != CODE2_CUSTOM)&&(rmc4_custom != ZONE4_CUSTOM)) {
#else
					if(rmc4_custom != CODE2_CUSTOM) { 
#endif
						rmc4_restart();
					}else{
						// 책섬 검사 ..
						if((RMC4_DATA.bValue[0] ^ RMC4_DATA.bValue[1]) == 0xff){
					
							// repeat key처리..
							if(f1_rmc4_repeat == 1 ) {
								if(RMC4_DATA.bValue[0] != b1_rmc4_key) {
									rmc4_restart();
								}else{
									rmc4_repeat();
								}
							// new key처리..
							}else{
								//if ( f1_rmc4_keyoff == 1 ) {
								//	rmc4_restart();
								//}else{
									b1_rmc4_key = RMC4_DATA.bValue[0];
									f1_rmc4_key = 1;
									f1_rmc4_keypress = 1;
									f1_rmc4_longkey = 0;
									f1_rmc4_keyoff = 0;
									f1_rmc4_repeat = 1;
									b1_rmc4_longkeycount = COUNT_LONGKEY;
									b1_rmc4_flamecount = COUNT_FLAME;
									b1_rmc4_mode = RM_READY;	// 다음키 입력 준비
									b4_pwrlong_count = 0;	//JYJ_PWRLONG
									z4_party_count = 0;
									z4_partyoff_count = 0;
								//}
							}
						}
					}
				}
			}
		}
		break;
	default:
		rmc4_restart();
		break;
	}
}
void	rmc4_restart(void)
{
	// 한번만 키오프가 발생되게 한다.
	if ( f1_rmc4_repeat == 1 ) {
		f1_rmc4_keyoff = 1;
		f1_rmc4_key = 1;
		f1_rmc4_repeat = 0;
		f1_rmc4_repeatkey = 0;
		b1_rmc4_flamecount = 0;
		b1_rmc4_repeatcount = 0;
	}	
	b1_rmc4_mode = RM_READY;

}
void	rmc4_repeat(void)
{
	// 리피트 카운트
	if ( b1_rmc4_longkeycount != 0 ) {
		--b1_rmc4_longkeycount;
		if ( b1_rmc4_longkeycount == 0 ) {
			f1_rmc4_longkey = 1;
			f1_rmc4_key = 1;
			b1_rmc4_repeatcount = TIME20MSTO160MS;			//20091218 20msec timer
		}
	}
	// 리피트코드는 조금 더 기다린다.
	b1_rmc4_flamecount = COUNT_FLAME_REPEAT;
	b1_rmc4_mode = RM_READY;		// 다음키 입력 준비
}
//
//	연속된 플레임(점멸)신호인가 검사 
//	데이타가 108밀리세크마다 반복되므로 
//	그 이상이되면 카운트가 '0'이된다(되게 한다).
//*******************************************************************
void	rmc4_CheckFlame_10msec(void)
{
	if ( b1_rmc4_flamecount != 0 ) {
		--b1_rmc4_flamecount;
		if ( b1_rmc4_flamecount == 0 ) {
			if ( f1_rmc4_repeat == 1 ) {
				f1_rmc4_keyoff = 1;
				f1_rmc4_key = 1;
				f1_rmc4_repeat = 0;
				f1_rmc4_repeatkey = 0;
				b1_rmc4_repeatcount = 0;
				f1_rmc4_longkey = 0;
			}
		}
	}
}
//
//	리피트키를 발생시킨다.
//*******************************************************************
void	rmc4_MakeRepeatKey_20msec(void)
{
	if ( f1_rmc4_repeat == 1 ) {
		--b1_rmc4_repeatcount;
		if ( b1_rmc4_repeatcount == 0 ) {
			b1_rmc4_repeatcount = TIME20MSTO100MS;		//20091218 : 450ms ->160ms->100ms->(100ms)
			f1_rmc4_key = 1;
			f1_rmc4_repeatkey = 1;
		}
	}
}
/* jyj_merge : RMC34_ADD end */
#endif /* RMC4_ENABLE */


#ifdef RMC3_ENABLE

//remote3
//	RMCLR		1
//	EDGSEL		0
//	RMISEL		1
//	RMPOL		0
//	RCK1,RCK0	1,0	51.2usec
//	FCK1,FCK0	1,0	25.6usec
//**********************************************************************
void rmc3_Init(void)
{
	f1_rmc3_repeat = 0;
	f1_rmc3_repeatkey = 0;
	b1_rmc3_flamecount = 0;
	b1_rmc3_repeatcount = 0;
	b1_rmc3_mode = RM_READY;
	b1_rmd3_mode = CM_READY;

	// global var
	f1_rmc3_key = 0;
	f1_rmc3_keypress = 0;
	f1_rmc3_longkey = 0;
	f1_rmc3_keyoff = 0;
}

//
//	인트럽트 서비스 루틴.(499usec)
//********************************************************************
#define 	RMC3_INT_ACTIVE		1
#define 	RMC3_INT_NOACTIVE	0


//
//
//	remocon 인터럽트 루틴 high edge까지의 시간을 return
//
//********************************************************************
void rmc3_Timer_250usec(void)
{
	switch ( b1_rmd3_mode ) {
	case CM_READY:
		if ( p_rmc_zone3 == 0 ) {
			b1_rmd3_copy = 1;
			b1_rmd3_mode = CM_LOW;
		}
		b1_rmc3_status = FALSE;
		break;
	case CM_LOW:
		if ( p_rmc_zone3 == 0 ) { 
			// low count up
			if ( b1_rmd3_copy < 0xff ) {
				++b1_rmd3_copy;
			}
			b1_rmc3_status = FALSE;
		}else{
			// high되는 시점에 rmc처리루틴 on
			b1_rmd3_mode = CM_HIGH;
			b1_rmd3 = b1_rmd3_copy;		// low count값 return
			b1_rmd3_copy = 1;
			b1_rmc3_status = TRUE;
		}
		break;
		
	case CM_HIGH:
		if ( p_rmc_zone3 == 1 ) { 
		
			// high count up
			if ( b1_rmd3_copy < RT_30MS ) {
				++b1_rmd3_copy;
				b1_rmc3_status = FALSE;
			} else {
				// 오버플로우
				b1_rmd3_mode = CM_READY;
				b1_rmd3 = b1_rmd3_copy;
				b1_rmc3_status = FALSE;
			}
		}else{
			b1_rmd3_mode = CM_LOW;
			b1_rmc3_status = FALSE;
			if (b1_rmc3_mode == RM_READY) {		//jyj_rmcreader
				b1_rmd3_copy = 1;
			}
		}
		break;
	default:
		b1_rmd3_mode = CM_READY;
		b1_rmc3_status = FALSE;
		break;
	}
	rmc3_status = b1_rmc3_status;
}


//
//
//	remocon 인터럽트 루틴 high edge까지의 시간을 return
//
//********************************************************************
void rmc3_process(void) 
{
	unsigned short masking;

	switch ( b1_rmc3_mode ) {
	// 9msec짜리 첫번째 리드코드를 기다린다.
	case RM_READY:
		if ( b1_rmd3 != RT_OVFLW ) {
			// 마지막 타임아웃
			if( RT_7MS <= b1_rmd3) {
				if( b1_rmd3 < RT_15MS ) {
					b1_rmc3_mode = RM_LEADER;      
				}
			}
		}
		break;
	case RM_LEADER:
		// 5.06msec == 1st flame
		// 2.81msec == repeat flame
		
		// 6msec보다 크면 error로 restart.
		if ( b1_rmd3 >= RT_9MS ) {
			rmc3_restart();
		}else{
			// 4ms보다 작고 2.25ms보다 크면 repeat인가 검사.
			if (RT_2250US <= b1_rmd3 ) {
				if( b1_rmd3 < RT_4MS ) {
					if ( f1_rmc3_repeat == 1 ) {
						rmc3_repeat();
					}else{
						rmc3_restart();
					}
				}else{
					// 4ms < b1_rmd2 < 6mse
					// 데이타 플레임
					b1_rmc3_mode = RM_DATA;
					b1_rmc3_bitcount = 32;
					RMC3_DATA.iValue = 0;
					rmc3_custom = 0;
					//20091218 //chd : repeat 바로이어서(짧은시간) -> 새로운 code start!!
					f1_rmc3_longkey = 0;
					f1_rmc3_repeat = 0;
					b1_rmc3_flamecount = 0;
					b1_rmc3_repeatcount = 0;

				}
			}else{
				rmc3_restart();
				
			}
		}
		break;
	case RM_DATA:
		// 범위 검사
		if ( b1_rmd3 >= RT_2700US ) {
			rmc3_restart();
		}else{
			if( b1_rmd3 >= RT_900US ) {
			//  0/1 검사
				if ( b1_rmd3 >= RT_1687US ) {
					b1_rmc3_masking = 0x8000;
				} else {
					b1_rmc3_masking = 0x0000;
				}
			
				// custome code 를 수신..
				if(b1_rmc3_bitcount >= 17) {
					rmc3_custom >>= 1;
					rmc3_custom &= 0x7fff;
					rmc3_custom |= b1_rmc3_masking;
				}else{
					// data수신..
					RMC3_DATA.iValue >>= 1;
					RMC3_DATA.iValue &= 0x7fff;
					RMC3_DATA.iValue |= b1_rmc3_masking;
				}
				if(--b1_rmc3_bitcount == 0){	
					// 커스텀 코드 검사
#if 1	//jyj_zoneir
					if((rmc3_custom != CODE2_CUSTOM)&&(rmc3_custom != ZONE3_CUSTOM)) {
#else
					if(rmc3_custom != CODE2_CUSTOM) { 
#endif
						rmc3_restart();
					}else{
						// 책섬 검사 ..
						if((RMC3_DATA.bValue[0] ^ RMC3_DATA.bValue[1]) == 0xff){
					
							// repeat key처리..
							if(f1_rmc3_repeat == 1 ) {
								if(RMC3_DATA.bValue[0] != b1_rmc3_key) {
									rmc3_restart();
								}else{
									rmc3_repeat();
								}
							// new key처리..
							}else{
								//if ( f1_rmc3_keyoff == 1 ) {
								//	rmc3_restart();
								//}else{
									b1_rmc3_key = RMC3_DATA.bValue[0];
									f1_rmc3_key = 1;
									f1_rmc3_keypress = 1;
									f1_rmc3_longkey = 0;
									f1_rmc3_keyoff = 0;
									f1_rmc3_repeat = 1;
									b1_rmc3_longkeycount = COUNT_LONGKEY;
									b1_rmc3_flamecount = COUNT_FLAME;
									b1_rmc3_mode = RM_READY;	// 다음키 입력 준비
									b3_pwrlong_count = 0;    /* jyj_merge */
									z3_party_count = 0;      /* jyj_merge */
									z3_partyoff_count = 0;   /* jyj_merge */
								//}
							}
						}
					}
				}
			}
		}
		break;
	default:
		rmc3_restart();
		break;
	}
}
void	rmc3_restart(void)
{
	// 한번만 키오프가 발생되게 한다.
	if ( f1_rmc3_repeat == 1 ) {
		f1_rmc3_keyoff = 1;
		f1_rmc3_key = 1;
		f1_rmc3_repeat = 0;
		f1_rmc3_repeatkey = 0;
		b1_rmc3_flamecount = 0;
		b1_rmc3_repeatcount = 0;
	}	
	b1_rmc3_mode = RM_READY;

}
	
void	rmc3_repeat(void)
{
	// 리피트 카운트
	if ( b1_rmc3_longkeycount != 0 ) {
		--b1_rmc3_longkeycount;
		if ( b1_rmc3_longkeycount == 0 ) {
			f1_rmc3_longkey = 1;
			f1_rmc3_key = 1;
			b1_rmc3_repeatcount = TIME20MSTO160MS;			//20091218 20msec timer
		}
	}
	// 리피트코드는 조금 더 기다린다.
	b1_rmc3_flamecount = COUNT_FLAME_REPEAT;
	b1_rmc3_mode = RM_READY;		// 다음키 입력 준비
}	
//
//	연속된 플레임(점멸)신호인가 검사 
//	데이타가 108밀리세크마다 반복되므로 
//	그 이상이되면 카운트가 '0'이된다(되게 한다).
//*******************************************************************
void	rmc3_CheckFlame_10msec(void)
{
	if ( b1_rmc3_flamecount != 0 ) {
		--b1_rmc3_flamecount;
		if ( b1_rmc3_flamecount == 0 ) {
			if ( f1_rmc3_repeat == 1 ) {
				f1_rmc3_keyoff = 1;
				f1_rmc3_key = 1;
				f1_rmc3_repeat = 0;
				f1_rmc3_repeatkey = 0;
				b1_rmc3_repeatcount = 0;
				f1_rmc3_longkey = 0;
			}
		}
	}
}
//
//	리피트키를 발생시킨다.
//*******************************************************************
void	rmc3_MakeRepeatKey_20msec(void)
{
	if ( f1_rmc3_repeat == 1 ) {
		--b1_rmc3_repeatcount;
		if ( b1_rmc3_repeatcount == 0 ) {
			b1_rmc3_repeatcount = TIME20MSTO100MS;		//20091218 : 450ms ->160ms->100ms->(100ms)
			f1_rmc3_key = 1;
			f1_rmc3_repeatkey = 1;
		}
	}
}
#endif /* RMC3_ENABLE */


#ifdef RMC2_ENABLE
//
//remote2
//
/*
	RMCLR		1
	EDGSEL		0
	RMISEL		1
	RMPOL		0
	RCK1,RCK0	1,0	51.2usec
	FCK1,FCK0	1,0	25.6usec
***********************************************************************/
void rmc2_Init(void)
{
	f1_rmc2_repeat = 0;
	f1_rmc2_repeatkey = 0;
	b1_rmc2_flamecount = 0;
	b1_rmc2_repeatcount = 0;
	b1_rmc2_mode = RM_READY;
	b1_rmd2_mode = CM_READY;

	/* global var */
	f1_rmc2_key = 0;
	f1_rmc2_keypress = 0;
	f1_rmc2_longkey = 0;
	f1_rmc2_keyoff = 0;
}

/*                              
	인트럽트 서비스 루틴.(499usec)
*********************************************************************/
#define 	RMC2_INT_ACTIVE		1
#define 	RMC2_INT_NOACTIVE	0


/*

	remocon 인터럽트 루틴 high edge까지의 시간을 return

*********************************************************************/
void rmc2_Timer_250usec(void)
{
	switch ( b1_rmd2_mode ) {
	case CM_READY:
		if ( p_rmc_zone2 == 0 ) {
			b1_rmd2_copy = 1;
			b1_rmd2_mode = CM_LOW;
		}
		b1_rmc2_status = FALSE;
		break;
	case CM_LOW:
		if ( p_rmc_zone2 == 0 ) { 
			// low count up
			if ( b1_rmd2_copy < 0xff ) {
				++b1_rmd2_copy;
			}
			b1_rmc2_status = FALSE;
		}else{
			// high되는 시점에 rmc처리루틴 on
			b1_rmd2_mode = CM_HIGH;
			b1_rmd2 = b1_rmd2_copy;		// low count값 return
			b1_rmd2_copy = 1;
			b1_rmc2_status = TRUE;
		}
		break;
		
	case CM_HIGH:
		if ( p_rmc_zone2 == 1 ) { 
		
			// high count up
			if ( b1_rmd2_copy < RT_30MS ) {
				++b1_rmd2_copy;
				b1_rmc2_status = FALSE;
			} else {
				/* 오버플로우 */
				b1_rmd2_mode = CM_READY;
				b1_rmd2 = b1_rmd2_copy;
				b1_rmc2_status = FALSE;
			}
		}else{
			b1_rmd2_mode = CM_LOW;
			b1_rmc2_status = FALSE;
			if (b1_rmc2_mode == RM_READY) {		//jyj_rmcreader
				b1_rmd2_copy = 1;
			}
		}
		break;
	default:
		b1_rmd2_mode = CM_READY;
		b1_rmc2_status = FALSE;
		break;
	}
	rmc2_status = b1_rmc2_status;
}


/*

	remocon 인터럽트 루틴 high edge까지의 시간을 return

*********************************************************************/
void rmc2_process(void) 
{
	unsigned short masking;

	switch ( b1_rmc2_mode ) {
	/* 9msec짜리 첫번째 리드코드를 기다린다. */
	case RM_READY:
		if ( b1_rmd2 != RT_OVFLW ) {
			/* 마지막 타임아웃 */
			if( RT_7MS <= b1_rmd2) {
				if( b1_rmd2 < RT_15MS ) {
					b1_rmc2_mode = RM_LEADER;      
				}
			}
		}
		break;
	case RM_LEADER:
		/* 5.06msec == 1st flame */
		/* 2.81msec == repeat flame */
		
		// 6msec보다 크면 error로 restart.
		if ( b1_rmd2 >= RT_9MS ) {
			rmc2_restart();
		}else{
			// 4ms보다 작고 2.25ms보다 크면 repeat인가 검사.
			if (RT_2250US <= b1_rmd2 ) {
				if( b1_rmd2 < RT_4MS ) {
					if ( f1_rmc2_repeat == 1 ) {
						rmc2_repeat();
					}else{
						rmc2_restart();
					}
				}else{
					// 4ms < b1_rmd2 < 6mse
					/* 데이타 플레임 */
					b1_rmc2_mode = RM_DATA;
					b1_rmc2_bitcount = 32;
					RMC2_DATA.iValue = 0;
					rmc2_custom = 0;
					//20091218 //chd : repeat 바로이어서(짧은시간) -> 새로운 code start!!
					f1_rmc2_longkey = 0;
					f1_rmc2_repeat = 0;
					b1_rmc2_flamecount = 0;
					b1_rmc2_repeatcount = 0;

				}
			}else{
				rmc2_restart();
				
			}
		}
		break;
	case RM_DATA:
		/* 범위 검사 */
		if ( b1_rmd2 >= RT_2700US ) {
			rmc2_restart();
		}else{
			if( b1_rmd2 >= RT_900US ) {
			/* 0/1 검사 */
				if ( b1_rmd2 >= RT_1687US ) {
					b1_rmc2_masking = 0x8000;
				} else {
					b1_rmc2_masking = 0x0000;
				}
			
				// custome code 를 수신..
				if(b1_rmc2_bitcount >= 17) {
					rmc2_custom >>= 1;
					rmc2_custom &= 0x7fff;
					rmc2_custom |= b1_rmc2_masking;
				}else{
					// data수신..
					RMC2_DATA.iValue >>= 1;
					RMC2_DATA.iValue &= 0x7fff;
					RMC2_DATA.iValue |= b1_rmc2_masking;
				}
				if(--b1_rmc2_bitcount == 0){	
					/* 커스텀 코드 검사 */
#if 1	//jyj_zoneir
					if((rmc2_custom != CODE2_CUSTOM)&&(rmc2_custom != ZONE2_CUSTOM)) {
#else
					if(rmc2_custom != CODE2_CUSTOM) { 
#endif
						rmc2_restart();
					}else{
						// 책섬 검사 ..
						if((RMC2_DATA.bValue[0] ^ RMC2_DATA.bValue[1]) == 0xff){
					
							// repeat key처리..
							if(f1_rmc2_repeat == 1 ) {
								if(RMC2_DATA.bValue[0] != b1_rmc2_key) {
									rmc2_restart();
								}else{
									rmc2_repeat();
								}
							// new key처리..
							}else{
								//if ( f1_rmc2_keyoff == 1 ) {
								//	rmc2_restart();
								//}else{
									b1_rmc2_key = RMC2_DATA.bValue[0];
									f1_rmc2_key = 1;
									f1_rmc2_keypress = 1;
									f1_rmc2_longkey = 0;
									f1_rmc2_keyoff = 0;
									f1_rmc2_repeat = 1;
									b1_rmc2_longkeycount = COUNT_LONGKEY;
									b1_rmc2_flamecount = COUNT_FLAME;
									b1_rmc2_mode = RM_READY;	/* 다음키 입력 준비 */
									b2_pwrlong_count = 0;	//jyj_merge
									z2_party_count = 0;     //jyj_merge
									z2_partyoff_count = 0;  //jyj_merge
								//}
							}
						}
					}
				}
			}
		}
		break;
	default:
		rmc2_restart();
		break;
	}
}


void	rmc2_restart(void)
{
	/* 한번만 키오프가 발생되게 한다. */
	if ( f1_rmc2_repeat == 1 ) {
		f1_rmc2_keyoff = 1;
		f1_rmc2_key = 1;
		f1_rmc2_repeat = 0;
		f1_rmc2_repeatkey = 0;
		b1_rmc2_flamecount = 0;
		b1_rmc2_repeatcount = 0;
	}	
	b1_rmc2_mode = RM_READY;
}
	
void	rmc2_repeat(void)
{
	/* 리피트 카운트 */
	if ( b1_rmc2_longkeycount != 0 ) {
		--b1_rmc2_longkeycount;
		if ( b1_rmc2_longkeycount == 0 ) {
			f1_rmc2_longkey = 1;
			f1_rmc2_key = 1;
			b1_rmc2_repeatcount = TIME20MSTO160MS;			//20091218 20msec timer
		}
	}
	/* 리피트코드는 조금 더 기다린다. */
	b1_rmc2_flamecount = COUNT_FLAME_REPEAT;
	b1_rmc2_mode = RM_READY;		/* 다음키 입력 준비 */
}	




/*
	연속된 플레임(점멸)신호인가 검사 
	데이타가 108밀리세크마다 반복되므로 
	그 이상이되면 카운트가 '0'이된다(되게 한다).
********************************************************************/
void	rmc2_CheckFlame_10msec(void)
{
	if ( b1_rmc2_flamecount != 0 ) {
		--b1_rmc2_flamecount;
		if ( b1_rmc2_flamecount == 0 ) {
			if ( f1_rmc2_repeat == 1 ) {
				f1_rmc2_keyoff = 1;
				f1_rmc2_key = 1;
				f1_rmc2_repeat = 0;
				f1_rmc2_repeatkey = 0;
				b1_rmc2_repeatcount = 0;
				f1_rmc2_longkey = 0;
			}
		}
	}
}
/*
	리피트키를 발생시킨다.
********************************************************************/
void	rmc2_MakeRepeatKey_20msec(void)
{
	if ( f1_rmc2_repeat == 1 ) {
		--b1_rmc2_repeatcount;
		if ( b1_rmc2_repeatcount == 0 ) {
			b1_rmc2_repeatcount = TIME20MSTO100MS;		//20091218 : 450ms ->160ms->100ms->(100ms)
			f1_rmc2_key = 1;
			f1_rmc2_repeatkey = 1;
		}
	}
}
#endif /* RMC2_ENABLE */



//
//	여기서 부터 rmc1.............
//
//




/*
	RMCLR		1
	EDGSEL		0
	RMISEL		1
	RMPOL		0
	RCK1,RCK0	1,0	51.2usec
	FCK1,FCK0	1,0	25.6usec
***********************************************************************/
void rmc1_Init(void)
{
	f1_rmc1_repeat = 0;
	f1_rmc1_repeatkey = 0;
	b1_rmc1_flamecount = 0;
	b1_rmc1_repeatcount = 0;
	b1_rmc1_mode = RM_READY;
	b1_rmd1_mode = CM_READY;

	/* global var */
	f1_rmc1_key = 0;
	f1_rmc1_keypress = 0;
	f1_rmc1_longkey = 0;
	f1_rmc1_keyoff = 0;
}

/*                              
	인트럽트 서비스 루틴.(499usec)
*********************************************************************/
#define 	RMC1_INT_ACTIVE		1
#define 	RMC1_INT_NOACTIVE	0


/*

	remocon 인터럽트 루틴 high edge까지의 시간을 return

*********************************************************************/
void rmc1_Timer_250usec(void)
{
	switch ( b1_rmd1_mode ) {
	case CM_READY:
		if ( p_rmc == 0 ) {
			b1_rmd1_copy = 1;
			b1_rmd1_mode = CM_LOW;
		}
		b1_rmc1_status = FALSE;
		break;
	case CM_LOW:
		if ( p_rmc == 0 ) { 
			// low count up
			if ( b1_rmd1_copy < 0xff ) {
				++b1_rmd1_copy;
			}
			b1_rmc1_status = FALSE;
		}else{
			// high되는 시점에 rmc처리루틴 on
			b1_rmd1_mode = CM_HIGH;
			b1_rmd1 = b1_rmd1_copy;		// low count값 return
			b1_rmd1_copy = 1;
			b1_rmc1_status = TRUE;
		}
		break;
		
	case CM_HIGH:
		if ( p_rmc == 1 ) { 
		
			// high count up
			if ( b1_rmd1_copy < RT_30MS ) {
				++b1_rmd1_copy;
				b1_rmc1_status = FALSE;
			} else {
				/* 오버플로우 */
				b1_rmd1_mode = CM_READY;
				b1_rmd1 = b1_rmd1_copy;
				b1_rmc1_status = FALSE;
			}
		}else{
			b1_rmd1_mode = CM_LOW;
			b1_rmc1_status = FALSE;
			if (b1_rmc1_mode == RM_READY) {		//jyj_rmcreader
				b1_rmd1_copy = 1;
			}
		}
		break;
	default:
		b1_rmd1_mode = CM_READY;
		b1_rmc1_status = FALSE;
		break;
	}
	rmc1_status = b1_rmc1_status;
}


/*

	remocon 인터럽트 루틴 high edge까지의 시간을 return

*********************************************************************/
void rmc1_process(void)
{
	switch ( b1_rmc1_mode ) {
	/* 9msec짜리 첫번째 리드코드를 기다린다. */
	case RM_READY:
		if ( b1_rmd1 != RT_OVFLW ) {
			/* 마지막 타임아웃 */
			if( RT_7MS <= b1_rmd1) {
				if(b1_rmd1 < RT_15MS ) {
					b1_rmc1_mode = RM_LEADER;
				}
			}
		}
		break;
	case RM_LEADER:
		/* 5.06msec == 1st flame */
		/* 2.81msec == repeat flame */
		
		// 6msec보다 크면 error로 restart.
		if ( b1_rmd1 >= RT_9MS ) {
			rmc1_restart();
		}else{
			// 4ms보다 작고 2.25ms보다 크면 repeat인가 검사.
			if (RT_2250US <= b1_rmd1) {
				if( b1_rmd1 < RT_4MS ) {
					if ( f1_rmc1_repeat == 1 ) {
						rmc1_repeat();
					}else{
						rmc1_restart();
					}
				}else{
			
					// 4ms < b1_rmd2 < 6mse
					/* 데이타 플레임 */
					b1_rmc1_mode = RM_DATA;
					b1_rmc1_bitcount = 32;
					RMC1_DATA.iValue = 0;
					rmc1_custom = 0;
					//20091218 //chd : repeat 바로이어서(짧은시간) -> 새로운 code start!!
					f1_rmc1_longkey = 0;
					f1_rmc1_repeat = 0;
					b1_rmc1_flamecount = 0;
					b1_rmc1_repeatcount = 0;
				}
			}else{
				rmc1_restart();			
			
			}
		}
		break;
	case RM_DATA:
		/* 범위 검사 */
		if ( b1_rmd1 >= RT_2700US ) {
			rmc1_restart();
		}else{
			if( b1_rmd1 >= RT_900US ) {
				/* 0/1 검사 */
				if ( b1_rmd1 >= RT_1687US ) {
					b1_rmc1_masking = 0x8000;
				} else {
					b1_rmc1_masking = 0x0000;
				}
			
				// custome code 를 수신..
				if(b1_rmc1_bitcount >= 17) {
					rmc1_custom >>= 1;
					rmc1_custom &= 0x7fff;
					rmc1_custom |= b1_rmc1_masking;
				}else{
					// data수신..
					RMC1_DATA.iValue >>= 1;
					RMC1_DATA.iValue &= 0x7fff;
					RMC1_DATA.iValue |= b1_rmc1_masking;
				}
				if(--b1_rmc1_bitcount == 0){	
				
					/* 커스텀 코드 검사 */
					//20120626
					if( (rmc1_custom!=CODE1_CUSTOM)&&(rmc1_custom!=CODE11_CUSTOM)&&(rmc1_custom!=CODE2_CUSTOM)&&(rmc1_custom!=CODE21_CUSTOM)&&(rmc1_custom!=CODE3_CUSTOM)&&(rmc1_custom!=CODE4_CUSTOM)&&(rmc1_custom!=CODE5_CUSTOM) ) { 
						rmc1_restart();
					}else{
						b2_rmc1_custom = rmc1_custom;			//saving! custom code
						// 책섬 검사 ..
						if((RMC1_DATA.bValue[0] ^ RMC1_DATA.bValue[1]) == 0xff){
					
							// repeat key처리..
							if(f1_rmc1_repeat == 1 ) {
								if(RMC1_DATA.bValue[0] != b1_rmc1_key) {
									rmc1_restart();
								}else{
									rmc1_repeat();
								}
							// new key처리..
							}else{
							
								//if ( f1_rmc1_keyoff == 1 ) {
								
								//	rmc1_restart();
								//}else{
								
								
									b1_rmc1_key = RMC1_DATA.bValue[0];
									f1_rmc1_key = 1;
									f1_rmc1_keypress = 1;
									f1_rmc1_longkey = 0;
									f1_rmc1_keyoff = 0;
									f1_rmc1_repeat = 1;

            						b_osdkey_count = 0;			/*chd*/
						            b_mutekey_count = 0;			/* 20020225 */
									
									// longkey flag set하는 카운터..
									b1_rmc1_longkeycount = COUNT_LONGKEY;
									// repeat code기다리는 카운터..
									b1_rmc1_flamecount = COUNT_FLAME;
									b1_rmc1_mode = RM_READY;	/* 다음키 입력 준비 */
									
								//}
							}
						}else{
							rmc1_restart();
						}
					}
				}
			}
		}
		break;
	default:
		rmc1_restart();
		break;
	}
}


void	rmc1_restart(void)
{
#if DEBUG_RMC_x
		rs232_printf("rs",blank);
#endif		
	/* 한번만 키오프가 발생되게 한다. */
	if ( f1_rmc1_repeat == 1 ) {
		f1_rmc1_keyoff = 1;
		f1_rmc1_key = 1;
		f1_rmc1_repeat = 0;
		f1_rmc1_repeatkey = 0;
		b1_rmc1_flamecount = 0;
		b1_rmc1_repeatcount = 0;
	}	
	b1_rmc1_mode = RM_READY;

}
	
void	rmc1_repeat(void)
{
	/* 리피트 카운트 */
	if ( b1_rmc1_longkeycount != 0 ) {
#if DEBUG_RMC_x
		rs232_printf(".",blank);
#endif		
		--b1_rmc1_longkeycount;
		if ( b1_rmc1_longkeycount == 0 ) {
			f1_rmc1_longkey = 1;
			f1_rmc1_key = 1;
			b1_rmc1_repeatcount = TIME20MSTO160MS;			//20091218 20msec timer
#if DEBUG_RMC_x
			rs232_printf("longkey start\n",blank);
#endif						
		}
	}
	/* 리피트코드는 조금 더 기다린다. */
	b1_rmc1_flamecount = COUNT_FLAME_REPEAT;
	b1_rmc1_mode = RM_READY;		/* 다음키 입력 준비 */
}	




/*
	연속된 플레임(점멸)신호인가 검사 
	데이타가 108밀리세크마다 반복되므로 
	그 이상이되면 카운트가 '0'이된다(되게 한다).
********************************************************************/
void	rmc1_CheckFlame_10msec(void)
{
	if ( b1_rmc1_flamecount != 0 ) {
		--b1_rmc1_flamecount;
		if ( b1_rmc1_flamecount == 0 ) {
			if ( f1_rmc1_repeat == 1 ) {
			
				f1_rmc1_keyoff = 1;
				f1_rmc1_key = 1;
				f1_rmc1_repeat = 0;
				f1_rmc1_repeatkey = 0;
				b1_rmc1_repeatcount = 0;
				f1_rmc1_longkey = 0;
				
				count_rmcRepeatKey = 0;			//20090722
			}
		}
	}
}
/*
	리피트키를 발생시킨다.
********************************************************************/
void	rmc1_MakeRepeatKey_20msec(void)
{

	if ( f1_rmc1_longkey == 1 ) {
		--b1_rmc1_repeatcount;
		if ( b1_rmc1_repeatcount == 0) {
			b1_rmc1_repeatcount = TIME20MSTO100MS;		//20091218 : 450ms ->160ms->100ms->(100ms)
			f1_rmc1_key = 1;
			f1_rmc1_repeatkey = 1;
		}
	}
}


//
//	여기서 부터 rmcE.............
//
//

/*
	RMCLR		1
	EDGSEL		0
	RMISEL		1
	RMPOL		0
	RCK1,RCK0	1,0	51.2usec
	FCK1,FCK0	1,0	25.6usec
***********************************************************************/
void rmcE_Init(void)
{
	f1_rmcE_repeat = 0;
	f1_rmcE_repeatkey = 0;
	b1_rmcE_flamecount = 0;
	b1_rmcE_repeatcount = 0;
	b1_rmcE_mode = RM_READY;
	b1_rmdE_mode = CM_READY;

	/* global var */
	f1_rmcE_key = 0;
	f1_rmcE_keypress = 0;
	f1_rmcE_longkey = 0;
	f1_rmcE_keyoff = 0;
}

/*                              
	인트럽트 서비스 루틴.(499usec)
*********************************************************************/
#define 	RMCE_INT_ACTIVE		1
#define 	RMCE_INT_NOACTIVE	0


/*

	remocon 인터럽트 루틴 high edge까지의 시간을 return

*********************************************************************/
void rmcE_Timer_250usec(void)
{
#ifdef ExtIR_DISABLE
#else
	
	switch ( b1_rmdE_mode ) {
	case CM_READY:
		if ( p_rmc_ext == 0 ) {
			b1_rmdE_copy = 1;
			b1_rmdE_mode = CM_LOW;
		}
		b1_rmcE_status = FALSE;
		break;
	case CM_LOW:
		if ( p_rmc_ext == 0 ) { 
			// low count up
			if ( b1_rmdE_copy < 0xff ) {
				++b1_rmdE_copy;
			}
			b1_rmcE_status = FALSE;
		}else{
			// high되는 시점에 rmc처리루틴 on
			b1_rmdE_mode = CM_HIGH;
			b1_rmdE = b1_rmdE_copy;		// low count값 return
			b1_rmdE_copy = 1;
			b1_rmcE_status = TRUE;
		}
		break;
		
	case CM_HIGH:
		if ( p_rmc_ext == 1 ) { 
		
			// high count up
			if ( b1_rmdE_copy < RT_30MS ) {
				++b1_rmdE_copy;
				b1_rmcE_status = FALSE;
			} else {
				/* 오버플로우 */
				b1_rmdE_mode = CM_READY;
				b1_rmdE = b1_rmdE_copy;
				b1_rmcE_status = FALSE;
			}
		}else{
			b1_rmdE_mode = CM_LOW;
			b1_rmcE_status = FALSE;
			if (b1_rmcE_mode == RM_READY) {		//jyj_rmcreader
				b1_rmdE_copy = 1;
			}
		}
		break;
	default:
		b1_rmdE_mode = CM_READY;
		b1_rmcE_status = FALSE;
		break;
	}
	rmcE_status = b1_rmcE_status;
#endif //ExtIR_DISABLE
}


/*

	remocon 인터럽트 루틴 high edge까지의 시간을 return

*********************************************************************/
void rmcE_process(void)
{
#ifdef ExtIR_DISABLE
#else

	switch ( b1_rmcE_mode ) {
	/* 9msec짜리 첫번째 리드코드를 기다린다. */
	case RM_READY:
		if ( b1_rmdE != RT_OVFLW ) {
			/* 마지막 타임아웃 */
			if( RT_7MS <= b1_rmdE) {
				if(b1_rmdE < RT_15MS ) {
					b1_rmcE_mode = RM_LEADER;
				}
			}
		}
		break;
	case RM_LEADER:
		/* 5.06msec == 1st flame */
		/* 2.81msec == repeat flame */
		
		// 6msec보다 크면 error로 restart.
		if ( b1_rmdE >= RT_9MS ) {
			rmcE_restart();
		}else{
			// 4ms보다 작고 2.25ms보다 크면 repeat인가 검사.
			if (RT_2250US <= b1_rmdE) {
				if( b1_rmdE < RT_4MS ) {
					if ( f1_rmcE_repeat == 1 ) {
						rmcE_repeat();
					}else{
						rmcE_restart();
					}
				}else{
			
					// 4ms < b1_rmd2 < 6mse
					/* 데이타 플레임 */
					b1_rmcE_mode = RM_DATA;
					b1_rmcE_bitcount = 32;
					RMCE_DATA.iValue = 0;
					rmcE_custom = 0;
					//20091218 //chd : repeat 바로이어서(짧은시간) -> 새로운 code start!!
					f1_rmcE_longkey = 0;
					f1_rmcE_repeat = 0;
					b1_rmcE_flamecount = 0;
					b1_rmcE_repeatcount = 0;
				}
			}else{
				rmcE_restart();			
			
			}
		}
		break;
	case RM_DATA:
		/* 범위 검사 */
		if ( b1_rmdE >= RT_2700US ) {
			rmcE_restart();
		}else{
			if( b1_rmdE >= RT_900US ) {
				/* 0/1 검사 */
				if ( b1_rmdE >= RT_1687US ) {
					b1_rmcE_masking = 0x8000;
				} else {
					b1_rmcE_masking = 0x0000;
				}
			
				// custome code 를 수신..
				if(b1_rmcE_bitcount >= 17) {
					rmcE_custom >>= 1;
					rmcE_custom &= 0x7fff;
					rmcE_custom |= b1_rmcE_masking;
				}else{
					// data수신..
					RMCE_DATA.iValue >>= 1;
					RMCE_DATA.iValue &= 0x7fff;
					RMCE_DATA.iValue |= b1_rmcE_masking;
				}
				if(--b1_rmcE_bitcount == 0){	
				
					/* 커스텀 코드 검사 */
					//20120626
					if( (rmcE_custom!=CODE1_CUSTOM)&&(rmcE_custom!=CODE11_CUSTOM)&&(rmcE_custom!=CODE2_CUSTOM)&&(rmcE_custom!=CODE21_CUSTOM)&&(rmcE_custom!=CODE3_CUSTOM)&&(rmcE_custom!=CODE4_CUSTOM)&&(rmcE_custom!=CODE5_CUSTOM) ) { 
						rmcE_restart();
					}else{
						// 책섬 검사 ..
						b2_rmcE_custom = rmcE_custom;			//saving! custom code

						if((RMCE_DATA.bValue[0] ^ RMCE_DATA.bValue[1]) == 0xff){
					
							// repeat key처리..
							if(f1_rmcE_repeat == 1 ) {
								if(RMCE_DATA.bValue[0] != b1_rmcE_key) {
									rmcE_restart();
								}else{
									rmcE_repeat();
								}
							// new key처리..
							}else{
							
								//if ( f1_rmcE_keyoff == 1 ) {
								
								//	rmcE_restart();
								//}else{
								
								
									b1_rmcE_key = RMCE_DATA.bValue[0];
									f1_rmcE_key = 1;
									f1_rmcE_keypress = 1;
									f1_rmcE_longkey = 0;
									f1_rmcE_keyoff = 0;
									f1_rmcE_repeat = 1;
            
            						b_osdkey_count = 0;			/*chd*/
						            b_mutekey_count = 0;			/* 20020225 */
						
									// longkey flag set하는 카운터..
									b1_rmcE_longkeycount = COUNT_LONGKEY;
									// repeat code기다리는 카운터..
									b1_rmcE_flamecount = COUNT_FLAME;
									b1_rmcE_mode = RM_READY;	/* 다음키 입력 준비 */
									
								//}
							}
						}else{
							rmcE_restart();
						}
					}
				}
			}
		}
		break;
	default:
		rmcE_restart();
		break;
	}
#endif //ExtIR_DISABLE

}


void	rmcE_restart(void)
{
#ifdef ExtIR_DISABLE
#else
	
	/* 한번만 키오프가 발생되게 한다. */
	if ( f1_rmcE_repeat == 1 ) {
		f1_rmcE_keyoff = 1;
		f1_rmcE_key = 1;
		f1_rmcE_repeat = 0;
		f1_rmcE_repeatkey = 0;
		b1_rmcE_flamecount = 0;
		b1_rmcE_repeatcount = 0;
	}	
	b1_rmcE_mode = RM_READY;
#endif //ExtIR_DISABLE
}
	
void	rmcE_repeat(void)
{
#ifdef ExtIR_DISABLE
#else

	/* 리피트 카운트 */
	if ( b1_rmcE_longkeycount != 0 ) {
#if DEBUG_RMC_x
		rs232_printf(".",blank);
#endif		
		--b1_rmcE_longkeycount;
		if ( b1_rmcE_longkeycount == 0 ) {
			f1_rmcE_longkey = 1;
			f1_rmcE_key = 1;
			b1_rmcE_repeatcount = TIME20MSTO160MS;		//20091218 20msec timer	
#if DEBUG_RMC_x
			rs232_printf("longkey start\n",blank);
#endif						
		}
	}
	/* 리피트코드는 조금 더 기다린다. */
	b1_rmcE_flamecount = COUNT_FLAME_REPEAT;
	b1_rmcE_mode = RM_READY;		/* 다음키 입력 준비 */
#endif //ExtIR_DISABLE
}	




/*
	연속된 플레임(점멸)신호인가 검사 
	데이타가 108밀리세크마다 반복되므로 
	그 이상이되면 카운트가 '0'이된다(되게 한다).
********************************************************************/
void	rmcE_CheckFlame_10msec(void)
{
#ifdef ExtIR_DISABLE
#else
	if ( b1_rmcE_flamecount != 0 ) {
		--b1_rmcE_flamecount;
		if ( b1_rmcE_flamecount == 0 ) {
			if ( f1_rmcE_repeat == 1 ) {
			
				f1_rmcE_keyoff = 1;
				f1_rmcE_key = 1;
				f1_rmcE_repeat = 0;
				f1_rmcE_repeatkey = 0;
				b1_rmcE_repeatcount = 0;
				f1_rmcE_longkey = 0;

				count_rmcRepeatKey = 0;			//20090722

			}
		}
	}
#endif //ExtIR_DISABLE
}
/*
	리피트키를 발생시킨다.
********************************************************************/
void	rmcE_MakeRepeatKey_20msec(void)
{
#ifdef ExtIR_DISABLE
#else
	if ( f1_rmcE_longkey == 1 ) {
		--b1_rmcE_repeatcount;
		if ( b1_rmcE_repeatcount == 0) {
			b1_rmcE_repeatcount = TIME20MSTO100MS;		//20091218 : 450ms ->160ms->100ms->(100ms)
			f1_rmcE_key = 1;
			f1_rmcE_repeatkey = 1;
		}
	}
#endif	//ExtIR_DISABLE
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//		AUDIO Mode CustomCode = 0x8311
//		TUNER Mode CustomCode = 0x8309
//		AUDIO2/TUNER2 Mode CustomCode = 0x8321
//		CD Mode CustomCode = 0x8312
//		USB Mode CustomCode = 0x8314
//		PC-USB Mode CustomCode = 0x8315
//
//		POWER OFF	= 0x80 
//		POWER ON	= 0x98
//------------------------------------------------------------------------------
//Audio mode(31)
const char tbl_rmc_8311_8321[][2] = {
//0	
	{RMC_POWEROFF,SK_POWEROFF},
	{RMC_POWERON,SK_POWERON},  
	{RMC_VOL_UP,SK_VOL_UP},		   
	{RMC_VOL_DN,SK_VOL_DN},	   
	{RMC_MUTE,SK_MUTE},		   
    
	{RMC_CD,SK_CD},				
	{RMC_TUNER,SK_TUNER},		   
	{RMC_AUX1,SK_AUX1},	   
	{RMC_AUX2,SK_AUX2},
	{RMC_USB,SK_USB},			   
//10
	{RMC_PHONO,SK_PHONO},				
	{RMC_COAX1,SK_COAX1},			   
	{RMC_COAX2,SK_COAX2},			   
	{RMC_BAL_XLR,SK_BAL_XLR},		   
	{RMC_PC_USB,SK_PC_USB},			

	{RMC_OPT1,SK_OPT1},			   
	{RMC_OPT2,SK_OPT2},			   
	{RMC_BASS,SK_BASS},			   
	{RMC_TREBLE,SK_TREBLE},			   
	{RMC_BALANCE,SK_BALANCE},			   
//20
	{RMC_DIMMER,SK_DIMMER},			   
	{RMC_SELECT,SK_SELECT},		   
	{RMC_SETUP,SK_SETUP},			
	{RMC_RIGHT,SK_RIGHT},			   
	{RMC_LEFT,SK_LEFT},			

	{RMC_UP,SK_UP},				
	{RMC_DOWN,SK_DOWN},		   
	{RMC_EXIT,SK_EXIT},			   
	{RMC_SPEAKER_A,SK_SPEAKER_A},		   
	{RMC_SPEAKER_B,SK_SPEAKER_B},		   

	{RMC_BYPASS,SK_BYPASS},
//31
	{RMC_OPT11,SK_OPT1},		    //20121217	   
	{RMC_OPT21,SK_OPT2},			   
	{RMC_COAX11,SK_COAX1},			   
	{RMC_COAX21,SK_COAX2}
//35	   
};
//------------------------------------------------------------------------------
//TUNER mode(14)
const char tbl_rmc_8309_8321[][2] = {
//0	
	{RMC_0,SK_RMC_0},			   
	{RMC_1,SK_RMC_1},				   
	{RMC_2,SK_RMC_2},				   
	{RMC_3,SK_RMC_3},			   
	{RMC_4,SK_RMC_4},				   
	{RMC_5,SK_RMC_5},			   
	{RMC_6,SK_RMC_6},		   
	{RMC_7,SK_RMC_7},			   
	{RMC_8,SK_RMC_8},
	{RMC_9,SK_RMC_9},
//10    
	{RMC_TUNE_DN,SK_TUNE_DOWN},
	{RMC_TUNE_UP,SK_TUNE_UP},
	{RMC_PRESET_DN,SK_PRESET_DOWN},
	{RMC_PRESET_UP,SK_PRESET_UP}
//14    
};    
//------------------------------------------------------------------------------
//CD mode(20)
const char tbl_rmc_8312[][2] = {
//0	
	{RMC_CD_0,SK_CD_0},			   
	{RMC_CD_1,SK_CD_1},			   
	{RMC_CD_2,SK_CD_2},			   
	{RMC_CD_3,SK_CD_3},			   
	{RMC_CD_4,SK_CD_4},			   
	{RMC_CD_5,SK_CD_5},  
	{RMC_CD_6,SK_CD_6},			   
	{RMC_CD_7,SK_CD_7},			   
	{RMC_CD_8,SK_CD_8},			   
	{RMC_CD_9,SK_CD_9},			   
//10
	{RMC_CD_10,SK_CD_10},			
	{RMC_CD_PLAY,SK_CD_PLAY},
	{RMC_CD_STOP,SK_CD_STOP},
	{RMC_CD_TRACK_M,SK_CD_TRACK_M},
	{RMC_CD_TRACK_P,SK_CD_TRACK_P},
	{RMC_CD_PAUSE,SK_CD_PAUSE},
	{RMC_CD_RANDOM,SK_CD_RANDOM},	   
	{RMC_CD_REPEAT,SK_CD_REPEAT},	   
	{RMC_CD_SEARCH_M,SK_CD_SEARCH_M},
	{RMC_CD_SEARCH_P,SK_CD_SEARCH_P}
//20  
};
//------------------------------------------------------------------------------
//USB mode(19)
const char tbl_rmc_8314[][2] = {
//0	
	{RMC_CD_0,SK_CD_0},			   
	{RMC_CD_1,SK_CD_1},			   
	{RMC_CD_2,SK_CD_2},			   
	{RMC_CD_3,SK_CD_3},			   
	{RMC_CD_4,SK_CD_4},			   
	{RMC_CD_5,SK_CD_5},  
	{RMC_CD_6,SK_CD_6},			   
	{RMC_CD_7,SK_CD_7},			   
	{RMC_CD_8,SK_CD_8},			   
	{RMC_CD_9,SK_CD_9},			   
//10
//	{RMC_CD_10,SK_CD_10},			
	{RMC_CD_PLAY,SK_CD_PLAY},
	{RMC_CD_STOP,SK_CD_STOP},
	{RMC_CD_TRACK_M,SK_CD_TRACK_M},
	{RMC_CD_TRACK_P,SK_CD_TRACK_P},
	{RMC_CD_PAUSE,SK_CD_PAUSE},
	{RMC_CD_RANDOM,SK_CD_RANDOM},	   
	{RMC_CD_REPEAT,SK_CD_REPEAT},	   
	{RMC_CD_SEARCH_M,SK_CD_SEARCH_M},
	{RMC_CD_SEARCH_P,SK_CD_SEARCH_P}
//19  
};
//------------------------------------------------------------------------------
//PC-USB mode(19)
const char tbl_rmc_8315[][2] = {
//0	
	{RMC_CD_0,SK_CD_0},			   
	{RMC_CD_1,SK_CD_1},			   
	{RMC_CD_2,SK_CD_2},			   
	{RMC_CD_3,SK_CD_3},			   
	{RMC_CD_4,SK_CD_4},			   
	{RMC_CD_5,SK_CD_5},  
	{RMC_CD_6,SK_CD_6},			   
	{RMC_CD_7,SK_CD_7},			   
	{RMC_CD_8,SK_CD_8},			   
	{RMC_CD_9,SK_CD_9},			   
//10
//	{RMC_CD_10,SK_CD_10},			
	{RMC_CD_PLAY,SK_CD_PLAY},
	{RMC_CD_STOP,SK_CD_STOP},
	{RMC_CD_TRACK_M,SK_CD_TRACK_M},
	{RMC_CD_TRACK_P,SK_CD_TRACK_P},
	{RMC_CD_PAUSE,SK_CD_PAUSE},
	{RMC_CD_RANDOM,SK_CD_RANDOM},	   
	{RMC_CD_REPEAT,SK_CD_REPEAT},	   
	{RMC_CD_SEARCH_M,SK_CD_SEARCH_M},
	{RMC_CD_SEARCH_P,SK_CD_SEARCH_P}
//19  
};
//------------------------------------------------------------------------------
//
/*
	custom code
	mode_AudioCustomCode = 0;
	mode_CDCustomCode = 0;

EX)		key_Value = Get_SystemKeyForRmcKey(b2_rmc1_custom, b1_rmc1_key);	//reading rmc key

**************************************************************/
unsigned char Get_SystemKeyForRmcKey(unsigned int rmc_custom, unsigned char rmc_key)
{
	unsigned char key_return,i;

	key_return = SK_NULL;

    //--------------------------------------------------------------------------
	//AUDIO mode(31) [ CODE1_CUSTOM=0x8311, CODE11_CUSTOM=0x8321 ] 
	//TUNER mode(14) [ CODE2_CUSTOM=0x8309, CODE21_CUSTOM=0x8321 ]
	//CD    mode(19) [ CODE3_CUSTOM=0x8312 ]
	//USB   mode(18) [ CODE4_CUSTOM=0x8314 ]
	//PCUSB mode(18) [ CODE5_CUSTOM=0x8315 ]
    //--------------------------------------------------------------------------
    //?Audio mode
	if(((mode_AudioCustomCode==0)&&(rmc_custom==CODE1_CUSTOM))||((mode_AudioCustomCode==1)&&(rmc_custom==CODE11_CUSTOM))){
	    //20121217
		for (i=0;i<35;i++) {
			if (tbl_rmc_8311_8321[i][0] == rmc_key) {
				key_return = tbl_rmc_8311_8321[i][1];
				break;
			}
		}
	}
    //?Tuner mode
	if(((mode_AudioCustomCode==0)&&(rmc_custom==CODE2_CUSTOM))||((mode_AudioCustomCode==1)&&(rmc_custom==CODE21_CUSTOM))){
		    for (i=0;i<14;i++) {							
			    if (tbl_rmc_8309_8321[i][0] == rmc_key) {
				    key_return = tbl_rmc_8309_8321[i][1];
				    break;
			    }
			}
	}
    //?CD mode
	if(rmc_custom==CODE3_CUSTOM){
			for (i=0;i<20;i++) {
				if (tbl_rmc_8312[i][0] == rmc_key) {
					key_return = tbl_rmc_8312[i][1];
					break;
				}
			}
	}
    //?USB mode => all data support
	if(rmc_custom==CODE4_CUSTOM){
		    for (i=0;i<19;i++) {
			    if (tbl_rmc_8314[i][0] == rmc_key) {
				    key_return = tbl_rmc_8314[i][1];
				    break;
				}
			}
	}
    //?PC-USB mode
	if(rmc_custom==CODE5_CUSTOM){
			for (i=0;i<19;i++) {
				if (tbl_rmc_8315[i][0] == rmc_key) {
					key_return = tbl_rmc_8315[i][1];
					break;
				}
			}
	}
	return(key_return);
}
/*
tbl_noeffect_display::
	db	SK_OSD_UP	
	db	SK_OSD_BACK	
	db	SK_OSD_RIGHT	
	db	SK_OSD_LEFT	
	db	SK_OSD_ENTER	
	db	SK_OSD_DOWN	    
	db	SK_OSD_UP
	db	SK_BAND		
	db	SK_TUNING_UP	
	db	SK_TUNING_DOWN	
	db	SK_PRESETSCAN	
	db	SK_RDS_DISPLAY	
	db	SK_RDS_PTY	
	db	SK_RDS_TP	
	db	SK_RDS_TA	
	db	SK_TUNEPRESET	
	db	SK_NULL		
	db	0ffh                           
*/
/* */
