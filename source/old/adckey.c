/**************** (c)2012 Rotel ************************************************
PROJECT         : RKBD850
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : adckey.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "adckey.h"
#include "adckey.mem"
#include "function.ext"

/*----------------------------------------------------------------------------*/
Key_t key;

/*----------------------------------------------------------------------------*/
//#define TEMPERATURE_100STEP_ENABLE		//temperature display table : [0..99/1STEP]
#define TEMPERATURE_2299STEP_ENABLE			//temperature display table : [22..99/1STEP]
#define V109_TEMPERATURE_2299STEP_ENABLE	//revised 20131128 : delete zenor diode 
/*----------------------------------------------------------------------------*/
#define _MN_ "ADC_KEY: "
/*-----------------------------------------------------------------------------
ROUTINE NAME : tbl_key
INPUT        :
OUTPUT       :
DESCRIPTION  : 
COMMENTS     : 
-----------------------------------------------------------------------------*/
const int ConvertTableTrim97[][2] = {			//Trim 1..97
	{42   	,1},	// < 0.0340 v
	{84   	,2},	// < 0.0680 v
	{127  	,3},	// < 0.1021 v
	{169  	,4},	// < 0.1361 v
	{211  	,5},	// < 0.1701 v
	{253  	,6},	// < 0.2041 v
	{296  	,7},	// < 0.2381 v
	{338  	,8},	// < 0.2722 v
	{380  	,9},	// < 0.3062 v
	{422  	,10},	// < 0.3402 v
	{464  	,11},	// < 0.3742 v
	{507  	,12},	// < 0.4082 v
	{549  	,13},	// < 0.4423 v
	{591  	,14},	// < 0.4763 v
	{633  	,15},	// < 0.5103 v
	{676  	,16},	// < 0.5443 v
	{718  	,17},	// < 0.5784 v
	{760  	,18},	// < 0.6124 v
	{802  	,19},	// < 0.6464 v
	{845  	,20},	// < 0.6804 v
	{887  	,21},	// < 0.7144 v
	{929  	,22},	// < 0.7485 v
	{971  	,23},	// < 0.7825 v
	{1013 	,24},	// < 0.8165 v
	{1056 	,25},	// < 0.8505 v
	{1098 	,26},	// < 0.8845 v
	{1140 	,27},	// < 0.9186 v
	{1182 	,28},	// < 0.9526 v
	{1225 	,29},	// < 0.9866 v
	{1267 	,30},	// < 1.0206 v
	{1309 	,31},	// < 1.0546 v
	{1351 	,32},	// < 1.0887 v
	{1393 	,33},	// < 1.1227 v
	{1436 	,34},	// < 1.1567 v
	{1478 	,35},	// < 1.1907 v
	{1520 	,36},	// < 1.2247 v
	{1562 	,37},	// < 1.2588 v
	{1605 	,38},	// < 1.2928 v
	{1647 	,39},	// < 1.3268 v
	{1689 	,40},	// < 1.3608 v
	{1731 	,41},	// < 1.3948 v
	{1774 	,42},	// < 1.4289 v
	{1816 	,43},	// < 1.4629 v
	{1858 	,44},	// < 1.4969 v
	{1900 	,45},	// < 1.5309 v
	{1942 	,46},	// < 1.5649 v
	{1985 	,47},	// < 1.5990 v
	{2027 	,48},	// < 1.6330 v
	{2069 	,49},	// < 1.6670 v
	{2111 	,50},	// < 1.7010 v
	{2154 	,51},	// < 1.7351 v
	{2196 	,52},	// < 1.7691 v
	{2238 	,53},	// < 1.8031 v
	{2280 	,54},	// < 1.8371 v
	{2322 	,55},	// < 1.8711 v
	{2365 	,56},	// < 1.9052 v
	{2407 	,57},	// < 1.9392 v
	{2449 	,58},	// < 1.9732 v
	{2491 	,59},	// < 2.0072 v
	{2534 	,60},	// < 2.0412 v
	{2576 	,61},	// < 2.0753 v
	{2618 	,62},	// < 2.1093 v
	{2660 	,63},	// < 2.1433 v
	{2703 	,64},	// < 2.1773 v
	{2745 	,65},	// < 2.2113 v
	{2787 	,66},	// < 2.2454 v
	{2829 	,67},	// < 2.2794 v
	{2871 	,68},	// < 2.3134 v
	{2914 	,69},	// < 2.3474 v
	{2956 	,70},	// < 2.3814 v
	{2998 	,71},	// < 2.4155 v
	{3040 	,72},	// < 2.4495 v
	{3083 	,73},	// < 2.4835 v
	{3125 	,74},	// < 2.5175 v
	{3167 	,75},	// < 2.5515 v
	{3209 	,76},	// < 2.5856 v
	{3251 	,77},	// < 2.6196 v
	{3294 	,78},	// < 2.6536 v
	{3336 	,79},	// < 2.6876 v
	{3378 	,80},	// < 2.7216 v
	{3420 	,81},	// < 2.7557 v
	{3463 	,82},	// < 2.7897 v
	{3505 	,83},	// < 2.8237 v
	{3547 	,84},	// < 2.8577 v
	{3589 	,85},	// < 2.8918 v
	{3632 	,86},	// < 2.9258 v
	{3674 	,87},	// < 2.9598 v
	{3716 	,88},	// < 2.9938 v
	{3758 	,89},	// < 3.0278 v
	{3800 	,90},	// < 3.0619 v
	{3843 	,91},	// < 3.0959 v
	{3885 	,92},	// < 3.1299 v
	{3927 	,93},	// < 3.1639 v
	{3969 	,94},	// < 3.1979 v
	{4012 	,95},	// < 3.2320 v
	{4054 	,96},	// < 3.2660 v
	{4096 	,97}, 	// < 3.3000 v //(2^12 = 4096 -1 = 4095)
};
#ifdef TEMPERATURE_100STEP_ENABLE				//not used
//RKB Temp. Table
const int ConvertTableTemp81[][2] = {			//Temp. 19..99
	{937  	,20},	// < 0.755 v
	{974  	,21},	// < 0.785 v
	{1005  	,22},	// < 0.810 v
	{1040  	,23},	// < 0.838 v
	{1082  	,24},	// < 0.872 v
	{1129 	,25},	// < 0.910 v
	{1179 	,26},	// < 0.950 v
	{1220 	,27},	// < 0.983 v
	{1265 	,28},	// < 1.019 v
	{1304 	,29},	// < 1.051 v
	{1349 	,30},	// < 1.087 v
	{1395 	,31},	// < 1.124 v
	{1437 	,32},	// < 1.158 v
	{1477 	,33},	// < 1.190 v
	{1534 	,34},	// < 1.236 v
	{1585 	,35},	// < 1.277 v
	{1637	,36},	// < 1.319 v
	{1690 	,37},	// < 1.362 v
	{1746 	,38},	// < 1.407 v
	{1791 	,39},	// < 1.443 v
	{1839 	,40},	// < 1.482 v
	{1887 	,41},	// < 1.522 v
	{1935 	,42},	// < 1.559 v
	{1983 	,43},	// < 1.598 v
	{2032 	,44},	// < 1.637 v
	{2076 	,45},	// < 1.673 v
	{2119 	,46},	// < 1.707 v
	{2172 	,47},	// < 1.750 v
	{2225 	,48},	// < 1.793 v
	{2278 	,49},	// < 1.835 v
	{2338 	,50},	// < 1.884 v
	{2379 	,51},	// < 1.917 v
	{2427 	,52},	// < 1.955 v	<======== FAN_DISABLE_TEMPERATURE
	{2476 	,53},	// < 1.995 v
	{2520 	,54},	// < 2.030 v
	{2570 	,55},	// < 2.071 v	<======== FAN_ENABLE_TEMPERATURE
	{2611 	,56},	// < 2.104 v
	{2656 	,57},	// < 2.140 v
	{2710 	,58},	// < 2.183 v
	{2758 	,59},	// < 2.222 v
	{2805 	,60},	// < 2.260 v
	{2855 	,61},	// < 2.300 v
	{2904 	,62},	// < 2.340 v
	{2945 	,63},	// < 2.373 v
	{2991 	,64},	// < 2.410 v
	{3030 	,65},	// < 2.441 v
	{3069 	,66},	// < 2.473 v
	{3114 	,67},	// < 2.509 v
	{3155 	,68},	// < 2.542 v
	{3197 	,69},	// < 2.576 v
	{3242 	,70},	// < 2.612 v
	{3279 	,71},	// < 2.642 v
	{3311 	,72},	// < 2.668 v
	{3340 	,73},	// < 2.691 v
	{3385 	,74},	// < 2.727 v
	{3427 	,75},	// < 2.761 v
	{3458 	,76},	// < 2.786 v
	{3496 	,77},	// < 2.817 v
	{3525 	,78},	// < 2.840 v
	{3558 	,79},	// < 2.867 v
	{3592 	,80},	// < 2.894 v
	{3627 	,81},	// < 2.922 v
	{3659 	,82},	// < 2.948 v
	{3690 	,83},	// < 2.973 v
	{3717 	,84},	// < 2.995 v
	{3743 	,85},	// < 3.016 v
	{3773 	,86},	// < 3.040 v
	{3798 	,87},	// < 3.060 v
	{3828 	,88},	// < 3.085 v
	{3856 	,89},	// < 3.107 v
	{3876 	,90},	// < 3.123 v
	{3906 	,91},	// < 3.147 v
	{3935 	,92},	// < 3.170 v
	{3947 	,93},	// < 3.180 v
	{3982 	,94},	// < 3.208 v
	{4009 	,95},	// < 3.230 v
	{4034 	,96},	// < 3.250 v
	{4046 	,97},	// < 3.263 v
	{4069 	,98},	// < 3.278 v
	{4084 	,99},	// < 3.290 v
	{4096 	,100}	// < 3.3000 v	//(2^12 = 4096 -1 = 4095)
};
#endif
//
//
//--------------------------------------------------------------------------------------------------
#ifdef TEMPERATURE_2299STEP_ENABLE
//OKI san table :(1052+38) 		: 20130731
//add temp table 22..86 		: 20130903
//add temp table 22..99 		: 20131024
//add temp table 22..99..Max99 	: 20131029
//revised temp table 22..99..Max99 	: 20131128
//revised temp table 21..95..Max95 	: 20131204 : for PP unit
//--------------------------------------------------------------------------------------------------
//#ifdef V11A_TEMPERATURE_2199STEP_ENABLE      	//20131211
#define	TEMP_ALPHA1		0
#define	TEMP_MALPHA		0
const int ConvertTableTemp2199_11A[][2] = {			//Temp. [ 21..99 ]
	{1034+TEMP_ALPHA1-TEMP_MALPHA 	,22},	
	{1072+TEMP_ALPHA1-TEMP_MALPHA 	,23},	
	{1093+TEMP_ALPHA1-TEMP_MALPHA 	,24},	
	{1121+TEMP_ALPHA1-TEMP_MALPHA 	,25},	
	{1162+TEMP_ALPHA1-TEMP_MALPHA 	,26},	
	{1202+TEMP_ALPHA1-TEMP_MALPHA 	,27},	
	{1237+TEMP_ALPHA1-TEMP_MALPHA 	,28},	
	{1278+TEMP_ALPHA1-TEMP_MALPHA 	,29},	
	{1310+TEMP_ALPHA1-TEMP_MALPHA 	,30},	
	{1360+TEMP_ALPHA1-TEMP_MALPHA 	,31},	
	{1411+TEMP_ALPHA1-TEMP_MALPHA 	,32},	
	{1452+TEMP_ALPHA1-TEMP_MALPHA 	,33},	
	{1503+TEMP_ALPHA1-TEMP_MALPHA 	,34},	
	{1542+TEMP_ALPHA1-TEMP_MALPHA 	,35},	
	{1576+TEMP_ALPHA1-TEMP_MALPHA 	,36},	
	{1636+TEMP_ALPHA1-TEMP_MALPHA 	,37},	
	{1690+TEMP_ALPHA1-TEMP_MALPHA 	,38},	
	{1736+TEMP_ALPHA1-TEMP_MALPHA 	,39},	
	{1785+TEMP_ALPHA1-TEMP_MALPHA 	,40},

	{1844+TEMP_ALPHA1-TEMP_MALPHA 	,41},	
	{1891+TEMP_ALPHA1-TEMP_MALPHA 	,42},	
	{1934+TEMP_ALPHA1-TEMP_MALPHA 	,43},	
	{1982+TEMP_ALPHA1-TEMP_MALPHA 	,44},	
	{2041+TEMP_ALPHA1-TEMP_MALPHA 	,45},
	{2066+TEMP_ALPHA1-TEMP_MALPHA 	,46},	
	{2130+TEMP_ALPHA1-TEMP_MALPHA 	,47},	
	{2143+TEMP_ALPHA1-TEMP_MALPHA 	,48},	
	{2225+TEMP_ALPHA1-TEMP_MALPHA 	,49},	
	{2270+TEMP_ALPHA1-TEMP_MALPHA 	,50},	
	{2335+TEMP_ALPHA1-TEMP_MALPHA 	,51},	
	{2382+TEMP_ALPHA1-TEMP_MALPHA 	,52},	
	{2422+TEMP_ALPHA1-TEMP_MALPHA 	,53},	
	{2470+TEMP_ALPHA1-TEMP_MALPHA 	,54},	
	{2522+TEMP_ALPHA1-TEMP_MALPHA 	,55},	
	{2578+TEMP_ALPHA1-TEMP_MALPHA 	,56},	
	{2637+TEMP_ALPHA1-TEMP_MALPHA 	,57},	
	{2668+TEMP_ALPHA1-TEMP_MALPHA 	,58},	
	{2719+TEMP_ALPHA1-TEMP_MALPHA 	,59},	
	{2763+TEMP_ALPHA1-TEMP_MALPHA 	,60},	

	{2812+TEMP_ALPHA1-TEMP_MALPHA 	,61},	
	{2866+TEMP_ALPHA1-TEMP_MALPHA 	,62},	
	{2917+TEMP_ALPHA1-TEMP_MALPHA 	,63},	
	{2975+TEMP_ALPHA1-TEMP_MALPHA 	,64},	
	{3001+TEMP_ALPHA1-TEMP_MALPHA 	,65},	
	{3040+TEMP_ALPHA1-TEMP_MALPHA 	,66},	
	{3085+TEMP_ALPHA1-TEMP_MALPHA 	,67},	
	{3142+TEMP_ALPHA1-TEMP_MALPHA 	,68},	
	{3170+TEMP_ALPHA1-TEMP_MALPHA 	,69},	
	{3223+TEMP_ALPHA1-TEMP_MALPHA 	,70},	
	{3255+TEMP_ALPHA1-TEMP_MALPHA 	,71},	
	{3308+TEMP_ALPHA1-TEMP_MALPHA 	,72},	
	{3344+TEMP_ALPHA1-TEMP_MALPHA 	,73},	
	{3378+TEMP_ALPHA1-TEMP_MALPHA 	,74},	
	{3413+TEMP_ALPHA1-TEMP_MALPHA 	,75},
	{3450+TEMP_ALPHA1-TEMP_MALPHA 	,76},	
	{3495+TEMP_ALPHA1-TEMP_MALPHA 	,77},	
	{3518+TEMP_ALPHA1-TEMP_MALPHA 	,78},	
	{3556+TEMP_ALPHA1-TEMP_MALPHA 	,79},	
	{3593+TEMP_ALPHA1-TEMP_MALPHA 	,80},	

	{3624+TEMP_ALPHA1-TEMP_MALPHA 	,81},	
	{3649+TEMP_ALPHA1-TEMP_MALPHA 	,82},	
	{3682+TEMP_ALPHA1-TEMP_MALPHA 	,83},	
	{3713+TEMP_ALPHA1-TEMP_MALPHA 	,84},	
	{3740+TEMP_ALPHA1-TEMP_MALPHA 	,85},	
	{3769+TEMP_ALPHA1-TEMP_MALPHA 	,86},	
	{3799+TEMP_ALPHA1-TEMP_MALPHA 	,87},	
	{3832+TEMP_ALPHA1-TEMP_MALPHA 	,88},	
	{3864+TEMP_ALPHA1-TEMP_MALPHA 	,89},	
	{3890+TEMP_ALPHA1-TEMP_MALPHA 	,90},	
	{3912+TEMP_ALPHA1-TEMP_MALPHA 	,91},	
	{3933+TEMP_ALPHA1-TEMP_MALPHA 	,92},	
	{3961+TEMP_ALPHA1-TEMP_MALPHA 	,93},	
	{3984+TEMP_ALPHA1-TEMP_MALPHA 	,94},	
	{4021+TEMP_ALPHA1-TEMP_MALPHA 	,95},
	{4035+TEMP_ALPHA1-TEMP_MALPHA 	,96},	
	{4050+TEMP_ALPHA1-TEMP_MALPHA 	,97},	
	{4065+TEMP_ALPHA1-TEMP_MALPHA 	,98},	
	{4080+TEMP_ALPHA1-TEMP_MALPHA 	,99},	
	{4096							,100}	// < 3.300 v : Max is always need because when value is over table, val is 0.  This is problem!!!!!
};
//--------------------------------------------------------------------------------------------------
//#ifdef V1tF_TEMPERATURE_2199STEP_ENABLE      	//20131209
#define	TEMP_ALPHA1		0
#define	TEMP_MALPHA		0
const int ConvertTableTemp2199[][2] = {			//Temp. [ 21..95 ]
	{1034+TEMP_ALPHA1-TEMP_MALPHA 	,22},	
	{1072+TEMP_ALPHA1-TEMP_MALPHA 	,23},	
	{1093+TEMP_ALPHA1-TEMP_MALPHA 	,24},	
	{1121+TEMP_ALPHA1-TEMP_MALPHA 	,25},	
	{1162+TEMP_ALPHA1-TEMP_MALPHA 	,26},	
	{1202+TEMP_ALPHA1-TEMP_MALPHA 	,27},	
	{1237+TEMP_ALPHA1-TEMP_MALPHA 	,28},	
	{1278+TEMP_ALPHA1-TEMP_MALPHA 	,29},	
	{1310+TEMP_ALPHA1-TEMP_MALPHA 	,30},	
	{1361+TEMP_ALPHA1-TEMP_MALPHA 	,31},	
	{1398+TEMP_ALPHA1-TEMP_MALPHA 	,32},	
	{1446+TEMP_ALPHA1-TEMP_MALPHA 	,33},	
	{1478+TEMP_ALPHA1-TEMP_MALPHA 	,34},	
	{1539+TEMP_ALPHA1-TEMP_MALPHA 	,35},	
	{1591+TEMP_ALPHA1-TEMP_MALPHA 	,36},	
	{1632+TEMP_ALPHA1-TEMP_MALPHA 	,37},	
	{1690+TEMP_ALPHA1-TEMP_MALPHA 	,38},	
	{1729+TEMP_ALPHA1-TEMP_MALPHA 	,39},	
	{1790+TEMP_ALPHA1-TEMP_MALPHA 	,40},

	{1840+TEMP_ALPHA1-TEMP_MALPHA 	,41},	
	{1888+TEMP_ALPHA1-TEMP_MALPHA 	,42},	
	{1934+TEMP_ALPHA1-TEMP_MALPHA 	,43},	
	{1980+TEMP_ALPHA1-TEMP_MALPHA 	,44},	
	{2040+TEMP_ALPHA1-TEMP_MALPHA 	,45},
	{2095+TEMP_ALPHA1-TEMP_MALPHA 	,46},	
	{2145+TEMP_ALPHA1-TEMP_MALPHA 	,47},	
	{2200+TEMP_ALPHA1-TEMP_MALPHA 	,48},	
	{2250+TEMP_ALPHA1-TEMP_MALPHA 	,49},	
	{2300+TEMP_ALPHA1-TEMP_MALPHA 	,50},	
	{2330+TEMP_ALPHA1-TEMP_MALPHA 	,51},	
	{2360+TEMP_ALPHA1-TEMP_MALPHA 	,52},	
	{2390+TEMP_ALPHA1-TEMP_MALPHA 	,53},	
	{2404+TEMP_ALPHA1-TEMP_MALPHA 	,54},	
	{2450+TEMP_ALPHA1-TEMP_MALPHA 	,55},	
	{2479+TEMP_ALPHA1-TEMP_MALPHA 	,56},	
	{2540+TEMP_ALPHA1-TEMP_MALPHA 	,57},	
	{2576+TEMP_ALPHA1-TEMP_MALPHA 	,58},	
	{2616+TEMP_ALPHA1-TEMP_MALPHA 	,59},	
	{2662+TEMP_ALPHA1-TEMP_MALPHA 	,60},	

	{2715+TEMP_ALPHA1-TEMP_MALPHA 	,61},	
	{2762+TEMP_ALPHA1-TEMP_MALPHA 	,62},	
	{2791+TEMP_ALPHA1-TEMP_MALPHA 	,63},	
	{2811+TEMP_ALPHA1-TEMP_MALPHA 	,64},	
	{2890+TEMP_ALPHA1-TEMP_MALPHA 	,65},	
	{2919+TEMP_ALPHA1-TEMP_MALPHA 	,66},	
	{2980+TEMP_ALPHA1-TEMP_MALPHA 	,67},	
	{2998+TEMP_ALPHA1-TEMP_MALPHA 	,68},	
	{3014+TEMP_ALPHA1-TEMP_MALPHA 	,69},	
	{3047+TEMP_ALPHA1-TEMP_MALPHA 	,70},	
	{3072+TEMP_ALPHA1-TEMP_MALPHA 	,71},	
	{3117+TEMP_ALPHA1-TEMP_MALPHA 	,72},	
	{3175+TEMP_ALPHA1-TEMP_MALPHA 	,73},	
	{3209+TEMP_ALPHA1-TEMP_MALPHA 	,74},	
	{3272+TEMP_ALPHA1-TEMP_MALPHA 	,75},
	{3293+TEMP_ALPHA1-TEMP_MALPHA 	,76},	
	{3324+TEMP_ALPHA1-TEMP_MALPHA 	,77},	
	{3361+TEMP_ALPHA1-TEMP_MALPHA 	,78},	
	{3380+TEMP_ALPHA1-TEMP_MALPHA 	,79},	
	{3415+TEMP_ALPHA1-TEMP_MALPHA 	,80},	

	{3450+TEMP_ALPHA1-TEMP_MALPHA 	,81},	
	{3480+TEMP_ALPHA1-TEMP_MALPHA 	,82},	
	{3520+TEMP_ALPHA1-TEMP_MALPHA 	,83},	
	{3551+TEMP_ALPHA1-TEMP_MALPHA 	,84},	
	{3592+TEMP_ALPHA1-TEMP_MALPHA 	,85},	
	{3623+TEMP_ALPHA1-TEMP_MALPHA 	,86},	
	{3648+TEMP_ALPHA1-TEMP_MALPHA 	,87},	
	{3669+TEMP_ALPHA1-TEMP_MALPHA 	,88},	
	{3706+TEMP_ALPHA1-TEMP_MALPHA 	,89},	
	{3741+TEMP_ALPHA1-TEMP_MALPHA 	,90},	
	{3771+TEMP_ALPHA1-TEMP_MALPHA 	,91},	
	{3798+TEMP_ALPHA1-TEMP_MALPHA 	,92},	
	{3824+TEMP_ALPHA1-TEMP_MALPHA 	,93},	
	{3854+TEMP_ALPHA1-TEMP_MALPHA 	,94},	
	{3889+TEMP_ALPHA1-TEMP_MALPHA 	,95},
	{3908+TEMP_ALPHA1-TEMP_MALPHA 	,96},	
	{3932+TEMP_ALPHA1-TEMP_MALPHA 	,97},	
	{3945+TEMP_ALPHA1-TEMP_MALPHA 	,98},	
	{3959+TEMP_ALPHA1-TEMP_MALPHA 	,99},	
	{4096							,100}	// < 3.300 v : Max is always need because when value is over table, val is 0.  This is problem!!!!!
};
//--------------------------------------------------------------------------------------------------
//#ifdef V1tC_TEMPERATURE_2299STEP_ENABLE
#define	TEMP_ALPHA1		0
#define	TEMP_MALPHA		0
const int ConvertTableTemp2195[][2] = {			//Temp. [ 21..95 ]
	{1034+TEMP_ALPHA1-TEMP_MALPHA 	,22},	
	{1072+TEMP_ALPHA1-TEMP_MALPHA 	,23},	
	{1093+TEMP_ALPHA1-TEMP_MALPHA 	,24},	
	{1121+TEMP_ALPHA1-TEMP_MALPHA 	,25},	
	{1162+TEMP_ALPHA1-TEMP_MALPHA 	,26},	
	{1202+TEMP_ALPHA1-TEMP_MALPHA 	,27},	
	{1237+TEMP_ALPHA1-TEMP_MALPHA 	,28},	
	{1278+TEMP_ALPHA1-TEMP_MALPHA 	,29},	
	{1310+TEMP_ALPHA1-TEMP_MALPHA 	,30},	
	{1361+TEMP_ALPHA1-TEMP_MALPHA 	,31},	
	{1398+TEMP_ALPHA1-TEMP_MALPHA 	,32},	
	{1446+TEMP_ALPHA1-TEMP_MALPHA 	,33},	
	{1478+TEMP_ALPHA1-TEMP_MALPHA 	,34},	
	{1539+TEMP_ALPHA1-TEMP_MALPHA 	,35},	
	{1591+TEMP_ALPHA1-TEMP_MALPHA 	,36},	
	{1632+TEMP_ALPHA1-TEMP_MALPHA 	,37},	
	{1690+TEMP_ALPHA1-TEMP_MALPHA 	,38},	
	{1729+TEMP_ALPHA1-TEMP_MALPHA 	,39},	
	{1790+TEMP_ALPHA1-TEMP_MALPHA 	,40},

	{1840+TEMP_ALPHA1-TEMP_MALPHA 	,41},	
	{1888+TEMP_ALPHA1-TEMP_MALPHA 	,42},	
	{1934+TEMP_ALPHA1-TEMP_MALPHA 	,43},	
	{1980+TEMP_ALPHA1-TEMP_MALPHA 	,44},	
	{2040+TEMP_ALPHA1-TEMP_MALPHA 	,45},
	{2095+TEMP_ALPHA1-TEMP_MALPHA 	,46},	
	{2145+TEMP_ALPHA1-TEMP_MALPHA 	,47},	
	{2200+TEMP_ALPHA1-TEMP_MALPHA 	,48},	
	{2250+TEMP_ALPHA1-TEMP_MALPHA 	,49},	
	{2300+TEMP_ALPHA1-TEMP_MALPHA 	,50},	
	{2357+TEMP_ALPHA1-TEMP_MALPHA 	,51},	
	{2405+TEMP_ALPHA1-TEMP_MALPHA 	,52},	
	{2460+TEMP_ALPHA1-TEMP_MALPHA 	,53},	
	{2510+TEMP_ALPHA1-TEMP_MALPHA 	,54},	
	{2575+TEMP_ALPHA1-TEMP_MALPHA 	,55},	
	{2615+TEMP_ALPHA1-TEMP_MALPHA 	,56},	
	{2660+TEMP_ALPHA1-TEMP_MALPHA 	,57},	
	{2715+TEMP_ALPHA1-TEMP_MALPHA 	,58},	
	{2760+TEMP_ALPHA1-TEMP_MALPHA 	,59},	
	{2840+TEMP_ALPHA1-TEMP_MALPHA 	,60},	

	{2910+TEMP_ALPHA1-TEMP_MALPHA 	,61},	
	{2960+TEMP_ALPHA1-TEMP_MALPHA 	,62},	
	{3010+TEMP_ALPHA1-TEMP_MALPHA 	,63},	
	{3055+TEMP_ALPHA1-TEMP_MALPHA 	,64},	
	{3105+TEMP_ALPHA1-TEMP_MALPHA 	,65},	
	{3160+TEMP_ALPHA1-TEMP_MALPHA 	,66},	
	{3200+TEMP_ALPHA1-TEMP_MALPHA 	,67},	
	{3240+TEMP_ALPHA1-TEMP_MALPHA 	,68},	
	{3285+TEMP_ALPHA1-TEMP_MALPHA 	,69},	
	{3320+TEMP_ALPHA1-TEMP_MALPHA 	,70},	
	{3370+TEMP_ALPHA1-TEMP_MALPHA 	,71},	
	{3410+TEMP_ALPHA1-TEMP_MALPHA 	,72},	
	{3450+TEMP_ALPHA1-TEMP_MALPHA 	,73},	
	{3500+TEMP_ALPHA1-TEMP_MALPHA 	,74},	
	{3530+TEMP_ALPHA1-TEMP_MALPHA 	,75},
	{3550+TEMP_ALPHA1-TEMP_MALPHA 	,76},	
	{3590+TEMP_ALPHA1-TEMP_MALPHA 	,77},	
	{3629+TEMP_ALPHA1-TEMP_MALPHA 	,78},	
	{3660+TEMP_ALPHA1-TEMP_MALPHA 	,79},	
	{3695+TEMP_ALPHA1-TEMP_MALPHA 	,80},	

	{3720+TEMP_ALPHA1-TEMP_MALPHA 	,81},	
	{3755+TEMP_ALPHA1-TEMP_MALPHA 	,82},	
	{3790+TEMP_ALPHA1-TEMP_MALPHA 	,83},	
	{3810+TEMP_ALPHA1-TEMP_MALPHA 	,84},	
	{3830+TEMP_ALPHA1-TEMP_MALPHA 	,85},	
	{3860+TEMP_ALPHA1-TEMP_MALPHA 	,86},	
	{3900+TEMP_ALPHA1-TEMP_MALPHA 	,87},	
	{3920+TEMP_ALPHA1-TEMP_MALPHA 	,88},	
	{3950+TEMP_ALPHA1-TEMP_MALPHA 	,89},	
	{3970+TEMP_ALPHA1-TEMP_MALPHA 	,90},	
	{3995+TEMP_ALPHA1-TEMP_MALPHA 	,91},	
	{4020+TEMP_ALPHA1-TEMP_MALPHA 	,92},	
	{4045+TEMP_ALPHA1-TEMP_MALPHA 	,93},	
	{4070+TEMP_ALPHA1-TEMP_MALPHA 	,94},	
	{4090+TEMP_ALPHA1-TEMP_MALPHA 	,95},
	{4096+TEMP_ALPHA1-TEMP_MALPHA 	,96},	
	{4096 							,96}	// < 3.300 v : Max is always need because when value is over table, val is 0.  This is problem!!!!!
};
//--------------------------------------------------------------------------------------------------
#ifdef V109_TEMPERATURE_2299STEP_ENABLE
#define	TEMP_ALPHA	10
#define	TEMP_MALPHA	0
const int ConvertTableTemp2299[][2] = {			//Temp. [ 22..86..99 ]
	{1034+TEMP_ALPHA-TEMP_MALPHA 	,22},	
	{1072+TEMP_ALPHA-TEMP_MALPHA 	,23},	
	{1093+TEMP_ALPHA-TEMP_MALPHA 	,24},	
	{1121+TEMP_ALPHA-TEMP_MALPHA 	,25},	
	{1156+TEMP_ALPHA-TEMP_MALPHA 	,26},	
	{1202+TEMP_ALPHA-TEMP_MALPHA 	,27},	
	{1237+TEMP_ALPHA-TEMP_MALPHA 	,28},	
	{1278+TEMP_ALPHA-TEMP_MALPHA 	,29},	
	{1310+TEMP_ALPHA-TEMP_MALPHA 	,30},	
	{1339+TEMP_ALPHA-TEMP_MALPHA 	,31},	
	{1371+TEMP_ALPHA-TEMP_MALPHA 	,32},	
	{1417+TEMP_ALPHA-TEMP_MALPHA 	,33},	
	{1444+TEMP_ALPHA-TEMP_MALPHA 	,34},	
	{1493+TEMP_ALPHA-TEMP_MALPHA 	,35},	
	{1530+TEMP_ALPHA-TEMP_MALPHA 	,36},	
	{1576+TEMP_ALPHA-TEMP_MALPHA 	,37},	
	{1603+TEMP_ALPHA-TEMP_MALPHA 	,38},	
	{1633+TEMP_ALPHA-TEMP_MALPHA 	,39},	
	{1711+TEMP_ALPHA-TEMP_MALPHA 	,40},		//<FAN_OFF
	{1752+TEMP_ALPHA-TEMP_MALPHA 	,41},	
	{1786+TEMP_ALPHA-TEMP_MALPHA 	,42},	
	{1831+TEMP_ALPHA-TEMP_MALPHA 	,43},	
	{1857+TEMP_ALPHA-TEMP_MALPHA 	,44},	
	{1916+TEMP_ALPHA-TEMP_MALPHA 	,45},		//<FAN_ON
	{1959+TEMP_ALPHA-TEMP_MALPHA 	,46},	
	{2020+TEMP_ALPHA-TEMP_MALPHA 	,47},	
	{2070+TEMP_ALPHA-TEMP_MALPHA 	,48},	
	{2140+TEMP_ALPHA-TEMP_MALPHA 	,49},	
	{2212+TEMP_ALPHA-TEMP_MALPHA 	,50},	
	{2253+TEMP_ALPHA-TEMP_MALPHA 	,51},	
	{2290+TEMP_ALPHA-TEMP_MALPHA 	,52},	
	{2320+TEMP_ALPHA-TEMP_MALPHA 	,53},	
	{2371+TEMP_ALPHA-TEMP_MALPHA 	,54},	
	{2434+TEMP_ALPHA-TEMP_MALPHA 	,55},	
	{2500+TEMP_ALPHA-TEMP_MALPHA 	,56},	
	{2559+TEMP_ALPHA-TEMP_MALPHA 	,57},	
	{2612+TEMP_ALPHA-TEMP_MALPHA 	,58},	
	{2625+TEMP_ALPHA-TEMP_MALPHA 	,59},	
	{2673+TEMP_ALPHA-TEMP_MALPHA 	,60},	
	{2731+TEMP_ALPHA-TEMP_MALPHA 	,61},	
	{2754+TEMP_ALPHA-TEMP_MALPHA 	,62},	
	{2800+TEMP_ALPHA-TEMP_MALPHA 	,63},	
	{2823+TEMP_ALPHA-TEMP_MALPHA 	,64},	
	{2860+TEMP_ALPHA-TEMP_MALPHA 	,65},	
	{2890+TEMP_ALPHA-TEMP_MALPHA 	,66},	
	{2920+TEMP_ALPHA-TEMP_MALPHA 	,67},	
	{2972+TEMP_ALPHA-TEMP_MALPHA 	,68},	
	{3042+TEMP_ALPHA-TEMP_MALPHA 	,69},	
	{3068+TEMP_ALPHA-TEMP_MALPHA 	,70},	
//add 20130903
	{3105+TEMP_ALPHA-TEMP_MALPHA 	,71},	
	{3148+TEMP_ALPHA-TEMP_MALPHA 	,72},	
	{3170+TEMP_ALPHA-TEMP_MALPHA 	,73},	
	{3224+TEMP_ALPHA-TEMP_MALPHA 	,74},	
	{3280+TEMP_ALPHA-TEMP_MALPHA 	,75},		//<SHUTDOWN
	{3309+TEMP_ALPHA-TEMP_MALPHA 	,76},	
	{3343+TEMP_ALPHA-TEMP_MALPHA 	,77},	
	{3372+TEMP_ALPHA-TEMP_MALPHA 	,78},	
	{3404+TEMP_ALPHA-TEMP_MALPHA 	,79},	
	{3467+TEMP_ALPHA-TEMP_MALPHA 	,80},	
	{3509+TEMP_ALPHA-TEMP_MALPHA 	,81},	
	{3527+TEMP_ALPHA-TEMP_MALPHA 	,82},	
	{3578+TEMP_ALPHA-TEMP_MALPHA 	,83},	
	{3598+TEMP_ALPHA-TEMP_MALPHA 	,84},	
	{3621+TEMP_ALPHA-TEMP_MALPHA 	,85},	
	{3651+TEMP_ALPHA-TEMP_MALPHA 	,86},	
//add 20131024
	{3683+TEMP_ALPHA-TEMP_MALPHA 	,87},	
	{3706+TEMP_ALPHA-TEMP_MALPHA 	,88},	
	{3741+TEMP_ALPHA-TEMP_MALPHA 	,89},	
	{3771+TEMP_ALPHA-TEMP_MALPHA 	,90},	
	{3798+TEMP_ALPHA-TEMP_MALPHA 	,91},	
	{3824+TEMP_ALPHA-TEMP_MALPHA 	,92},	
	{3854+TEMP_ALPHA-TEMP_MALPHA 	,93},	
	{3889+TEMP_ALPHA-TEMP_MALPHA 	,94},	
	{3908+TEMP_ALPHA-TEMP_MALPHA 	,95},		//<SMPS_SHUTDOWN
	{3932+TEMP_ALPHA-TEMP_MALPHA 	,96},	
	{3945+TEMP_ALPHA-TEMP_MALPHA 	,97},	
	{3959+TEMP_ALPHA-TEMP_MALPHA 	,98},	
	{3975+TEMP_ALPHA-TEMP_MALPHA 	,99},	
	{4096 							,99}	// < 3.300 v : Max is always need because when value is over table, val is 0.  This is problem!!!!!
};
#endif	//V109_TEMPERATURE_2299STEP_ENABLE
//--------------------------------------------------------------------------------------------------
#ifdef V108_TEMPERATURE_2299STEP_ENABLE
#define	TEMP_ALPHA	10
#define	TEMP_MALPHA	0
const int ConvertTableTemp2299[][2] = {			//Temp. [ 22..86..99 ]
	{1052+TEMP_ALPHA-TEMP_MALPHA 	,22},	// < 0.8470 v
	{1090+TEMP_ALPHA-TEMP_MALPHA 	,23},	// < 0.8710 v
	{1111+TEMP_ALPHA-TEMP_MALPHA 	,24},	// < 0.8880 v
	{1139+TEMP_ALPHA-TEMP_MALPHA 	,25},	// < 0.9100 v
	{1174+TEMP_ALPHA-TEMP_MALPHA 	,26},	// < 0.9370 v
	{1220+TEMP_ALPHA-TEMP_MALPHA 	,27},	// < 0.9670 v
	{1255+TEMP_ALPHA-TEMP_MALPHA 	,28},	// < 0.9960 v
	{1296+TEMP_ALPHA-TEMP_MALPHA 	,29},	// < 1.0300 v
	{1328+TEMP_ALPHA-TEMP_MALPHA 	,30},	// < 1.0610 v
	{1377+TEMP_ALPHA-TEMP_MALPHA 	,31},	// < 1.0950 v
	{1412+TEMP_ALPHA-TEMP_MALPHA 	,32},	// < 1.1230 v
	{1445+TEMP_ALPHA-TEMP_MALPHA 	,33},	// < 1.1610 v
	{1507+TEMP_ALPHA-TEMP_MALPHA 	,34},	// < 1.1970 v
	{1555+TEMP_ALPHA-TEMP_MALPHA 	,35},	// < 1.2310 v
	{1584+TEMP_ALPHA-TEMP_MALPHA 	,36},	// < 1.2660 v
	{1638+TEMP_ALPHA-TEMP_MALPHA 	,37},	// < 1.3010 v
	{1683+TEMP_ALPHA-TEMP_MALPHA 	,38},	// < 1.3360 v
	{1719+TEMP_ALPHA-TEMP_MALPHA 	,39},	// < 1.3710 v
	{1759+TEMP_ALPHA-TEMP_MALPHA 	,40},	// < 1.4060 v
	{1810+TEMP_ALPHA-TEMP_MALPHA 	,41},	// < 1.4400 v
	{1835+TEMP_ALPHA-TEMP_MALPHA 	,42},	// < 1.4720 v
	{1884+TEMP_ALPHA-TEMP_MALPHA 	,43},	// < 1.5060 v
	{1929+TEMP_ALPHA-TEMP_MALPHA 	,44},	// < 1.5360 v
	{1959+TEMP_ALPHA-TEMP_MALPHA 	,45},	// < 1.5680 v	
	{1989+TEMP_ALPHA-TEMP_MALPHA 	,46},	// < 1.5880 v
	{2021+TEMP_ALPHA-TEMP_MALPHA 	,47},	// < 1.6170 v
	{2051+TEMP_ALPHA-TEMP_MALPHA 	,48},	// < 1.6450 v
	{2097+TEMP_ALPHA-TEMP_MALPHA 	,49},	// < 1.6740 v
	{2135+TEMP_ALPHA-TEMP_MALPHA 	,50},	// < 1.7040 v
	{2173+TEMP_ALPHA-TEMP_MALPHA 	,51},	// < 1.7350 v
	{2211+TEMP_ALPHA-TEMP_MALPHA 	,52},	// < 1.7660 v	<======== FAN_OFF_TEMPERATURE
	{2249+TEMP_ALPHA-TEMP_MALPHA 	,53},	// < 1.7960 v
	{2287+TEMP_ALPHA-TEMP_MALPHA 	,54},	// < 1.8270 v
	{2325+TEMP_ALPHA-TEMP_MALPHA 	,55},	// < 1.8570 v	<======== FAN_ON_TEMPERATURE
	{2363+TEMP_ALPHA-TEMP_MALPHA 	,56},	// < 1.8880 v
	{2401+TEMP_ALPHA-TEMP_MALPHA 	,57},	// < 1.9190 v
	{2439+TEMP_ALPHA-TEMP_MALPHA 	,58},	// < 1.9490 v
	{2477+TEMP_ALPHA-TEMP_MALPHA 	,59},	// < 1.9800 v
	{2515+TEMP_ALPHA-TEMP_MALPHA 	,60},	// < 2.0100 v
	{2553+TEMP_ALPHA-TEMP_MALPHA 	,61},	// < 2.0410 v
	{2591+TEMP_ALPHA-TEMP_MALPHA 	,62},	// < 2.0720 v
	{2629+TEMP_ALPHA-TEMP_MALPHA 	,63},	// < 2.1020 v
	{2667+TEMP_ALPHA-TEMP_MALPHA 	,64},	// < 2.1330 v
	{2705+TEMP_ALPHA-TEMP_MALPHA 	,65},	// < 2.1640 v	
	{2743+TEMP_ALPHA-TEMP_MALPHA 	,66},	// < 2.1940 v
	{2781+TEMP_ALPHA-TEMP_MALPHA 	,67},	// < 2.2250 v
	{2819+TEMP_ALPHA-TEMP_MALPHA 	,68},	// < 2.2550 v
	{2857+TEMP_ALPHA-TEMP_MALPHA 	,69},	// < 2.2860 v
	{2895+TEMP_ALPHA-TEMP_MALPHA 	,70},	// < 2.3170 v
//add 20130903
	{2933+TEMP_ALPHA-TEMP_MALPHA 	,71},	// < 2.340 v
	{2971+TEMP_ALPHA-TEMP_MALPHA 	,72},	// < 2.373 v
	{3009+TEMP_ALPHA-TEMP_MALPHA 	,73},	// < 2.410 v
	{3047+TEMP_ALPHA-TEMP_MALPHA 	,74},	// < 2.441 v
	{3085+TEMP_ALPHA-TEMP_MALPHA 	,75},	// < 2.473 v
	{3123+TEMP_ALPHA-TEMP_MALPHA 	,76},	// < 2.509 v
	{3161+TEMP_ALPHA-TEMP_MALPHA 	,77},	// < 2.542 v
	{3199+TEMP_ALPHA-TEMP_MALPHA 	,78},	// < 2.576 v
	{3237+TEMP_ALPHA-TEMP_MALPHA 	,79},	// < 2.612 v
	{3275+TEMP_ALPHA-TEMP_MALPHA 	,80},	// < 2.642 v
	{3313+TEMP_ALPHA-TEMP_MALPHA 	,81},	// < 2.668 v
	{3351+TEMP_ALPHA-TEMP_MALPHA 	,82},	// < 2.691 v
	{3389+TEMP_ALPHA-TEMP_MALPHA 	,83},	// < 2.727 v
	{3427+TEMP_ALPHA-TEMP_MALPHA 	,84},	// < 2.761 v
	{3465+TEMP_ALPHA-TEMP_MALPHA 	,85},	// < 2.786 v	<======== MAIN HEAT SINK TEMPERATURE
	{3503+TEMP_ALPHA-TEMP_MALPHA 	,86},	// < 2.817 v
//add 20131024
	{3541+TEMP_ALPHA-TEMP_MALPHA 	,87},	// < 2.340 v
	{3579+TEMP_ALPHA-TEMP_MALPHA 	,88},	// < 2.373 v
	{3617+TEMP_ALPHA-TEMP_MALPHA 	,89},	// < 2.410 v
	{3655+TEMP_ALPHA-TEMP_MALPHA 	,90},	// < 2.441 v
	{3693+TEMP_ALPHA-TEMP_MALPHA 	,91},	// < 2.473 v
	{3731+TEMP_ALPHA-TEMP_MALPHA 	,92},	// < 2.509 v
	{3769+TEMP_ALPHA-TEMP_MALPHA 	,93},	// < 2.542 v
	{3807+TEMP_ALPHA-TEMP_MALPHA 	,94},	// < 2.576 v
	{3845+TEMP_ALPHA-TEMP_MALPHA 	,95},	// < 2.612 v
	{3883+TEMP_ALPHA-TEMP_MALPHA 	,96},	// < 2.642 v
	{3921+TEMP_ALPHA-TEMP_MALPHA 	,97},	// < 2.668 v
	{3959+TEMP_ALPHA-TEMP_MALPHA 	,98},	// < 2.691 v
	{3997+TEMP_ALPHA-TEMP_MALPHA 	,99},	// < 2.727 v	<======== SMPS HEAT SINK TEMPERATURE
	{4096 							,99}	// < 3.300 v : Max is always need because when value is over table, val is 0.  This is problem!!!!!
};
#endif	//V108_TEMPERATURE_2299STEP_ENABLE
#endif	//TEMPERATURE_2299STEP_ENABLE
/*-----------------------------------------------------------------------------
 Function Name  : adc_KeyInit
 Description    : initialize for header and tail
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void adc_KeyInit(void)
{
    byte i;
    
    for (i=0;i<CHNOS;i++) ADC_Val[i] = 0;
	memset(&key.KeyScan,0,sizeof(key.KeyScan));
	memset(&key.KeyImsi,0,sizeof(key.KeyImsi));
	memset(&key.KeyBuf,0,sizeof(key.KeyBuf));
	memset(&key.KeyData,0,sizeof(key.KeyData));
	memset(&key.KeyMode,KM_READY,sizeof(key.KeyMode));
	memset(&key.KeyOk,0,sizeof(key.KeyOk));
	memset(&key.KeyHoldTime,0,sizeof(key.KeyHoldTime));
	key.KeyStatus1.Byte = 0;
	key.KeyStatus2.Byte = 0;
	key.KeyStatus3.Byte = 0;
	key.KeyStatus4.Byte = 0;
	key.KeyStatus5.Byte = 0;
	key.KeyStatus6.Byte = 0;
}
/*-----------------------------------------------------------------------------
 Function Name  : adc_IRQHandler
 Description    : dma channel1 interrupt handler
 Input          : ADC_Val - 4byte
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void adc_DMA1_Channel1_IRQHandler(void)
{
	/* Clear DMA1 TC1 pending interrupt bit */
	DMA_ClearITPendingBit(DMA1_IT_TC1);	
}
/*-----------------------------------------------------------------------------
 Function Name  : adc_KeyRead
 Description    : 
 Input          : ADC_Val - 6byte
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void adc_KeyRead(unsigned char Lines)
//void adc_KeyRead(void)
{
	unsigned char i,value;
	unsigned int j;

  	/* Start ADC1 Software Conversion */ 
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	while (DMA_GetITStatus(DMA1_IT_TC1) == RESET);
	//if (DMA_GetITStatus(DMA1_IT_TC1) == RESET) return;
	DMA_ClearFlag(DMA1_IT_TC1);

	memset(&key.KeyScan,0,sizeof(key.KeyScan));
	
	/* scan ADC : 6 Line , ? Key */
	//--------------------------------------------------------
	for (i=0;i<Lines;i++) {
		value = 0;
		if ( (i == 0)||(i == 1)||(i == 2)||(i == 3) ) {
			//--------------------------------------------------------
			//Trim 0..96
			for(j=0;j<97;j++) {
				if ( ADC_Val[i] < ConvertTableTrim97[j][0] ) {
					value = ConvertTableTrim97[j][1]-1;				//[0..96]
					current_ADC_Val[i] = ADC_Val[i];				//16bit ADC value saving
					key.KeyScan[i] = value;			//saving
					break;
				}
			}
//			if (value != 0) {
//				key.KeyScan[i] = value;			//saving
//			}
			//--------------------------------------------------------
		}else{
			//--------------------------------------------------------
			//PA0 : NTC-L : ADC_Val[4]
			//PA1 : NTC-R : ADC_Val[5]
			//--------------------------------------------------------
#ifdef TEMPERATURE_100STEP_ENABLE								//not used
			//Temp [20..100/1STEP]
			for(j=0;j<81;j++) {
				if ( ADC_Val[i] < ConvertTableTemp81[j][0] ) {
					value = ConvertTableTemp81[j][1]-1;			//[19..99]
					current_ADC_Val[i] = ADC_Val[i];				//16bit ADC value saving
					key.KeyScan[i] = value;			//saving
					break;
				}
			}
//			if (value != 0) {
//				key.KeyScan[i] = value;			//saving
//			}
			//--------------------------------------------------------
#endif
			//--------------------------------------------------------
#ifdef TEMPERATURE_2299STEP_ENABLE
			//20131029 add temp [71..86..99..Max99]
			if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2)
				//Temperature range [22..86/5STEP]
				for(j=0;j<(65+14);j++) {
					if ( ADC_Val[i] < ConvertTableTemp2299[j][0] ) {
						value = ConvertTableTemp2299[j][1];			//[22..86]
						current_ADC_Val[i] = ADC_Val[i];				//16bit ADC value saving
						key.KeyScan[i] = value;			//saving
						break;
					}
				}
			}else{
				//! PP unit
				//Temperature range [22..99/5STEP]
				//20131024 : add SMPS Heat Sink sensor [ 31..99 ]
				//20131204 : adjust temp table [ 21..95 ]
				//20131209 : adjust temp table [ 21..99 ]
				//20131211 : adjust temp table [ 21..99 ]
				for(j=0;j<(79);j++) {
					if ( ADC_Val[i] < ConvertTableTemp2199_11A[j][0] ) {
						value = ( ConvertTableTemp2199_11A[j][1] - 1 );		//[21..99]
						current_ADC_Val[i] = ADC_Val[i];				//16bit ADC value saving
						key.KeyScan[i] = value;			//saving
						break;
					}
				}
			}
//			if (value != 0) {
//				key.KeyScan[i] = value;			//saving
//			}
			//--------------------------------------------------------
#endif
		}
	}
#ifdef UART1_DEBUG_x
	rs232_printf("ADC v=>%d %d %d %d %d %d\n",ADC_Val[0],ADC_Val[1],ADC_Val[2],ADC_Val[3],ADC_Val[4],ADC_Val[5],ADC_Val[6],ADC_Val[7]);
	rs232_printf("ADC k=>%d %d %d %d %d %d\n",key.KeyScan[0],key.KeyScan[1],key.KeyScan[2],key.KeyScan[3],key.KeyScan[4],key.KeyScan[5],key.KeyScan[6],key.KeyScan[7]);
#endif
#ifdef UART1_DEBUG_x
	rs232_printf("ADC v=>%d %d\n",ADC_Val[4],ADC_Val[5],ADC_Val[6],ADC_Val[7]);
	rs232_printf("ADC k=>%d %d\n",key.KeyScan[4],key.KeyScan[5],key.KeyScan[6],key.KeyScan[7]);
#endif
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : adc_KeyMake
INPUT        :
OUTPUT       :
DESCRIPTION  : chattering & get key data
COMMENTS     : 
    //Trim
	key.KeyScan[0] = 0..96
	key.KeyScan[1] = 0..96
	key.KeyScan[2] = 0..96
	key.KeyScan[3] = 0..96
    //Temp
	key.KeyScan[4] = 22..86
	key.KeyScan[5] = 22..86
	key.KeyScan[6] = 22..100
	key.KeyScan[7] = 22..100
-----------------------------------------------------------------------------*/
void adc_KeyMake(unsigned char Lines)
{
	unsigned char i;
	byte result = SUCCESS;
	
	i = Lines;		//0..7
	switch(key.KeyMode[i]) {
		case KM_READY:
			key.KeyImsi[i] = key.KeyScan[i];
			key.KeyData[i] = key.KeyScan[i];
			key.KeyHoldTime[i] = TIME10MSTO30MS;
			key.KeyMode[i] = KM_HOLD;
			break;
		case KM_HOLD:
			if (key.KeyImsi[i] != key.KeyScan[i]) {
				result = FAILED;
			}
			if (result == SUCCESS) {
				if (--key.KeyHoldTime[i] <= 0) {
					key.KeyBuf[i] = key.KeyImsi[i];
					key.KeyOk[i] = ON;
					key.KeyMode[i] = KM_KEYOFF;
					
#ifdef UART1_DEBUG_x
					rs232_printf("keyData %d : =>%x\n",i, key.KeyData[i]);
					//debug((_MN_"Input front key data is [%d]\n",key.KeyData));
#endif
				}
			} else {
				//Fail ! change mode : KM_READY
				adc_KeyRestart(i);
			}
			break;
		case KM_KEYOFF:
			if (key.KeyScan[i] != key.KeyImsi[i]) {
				result = FAILED;
			}
			if (result == SUCCESS) {
			} else {
				//Fail ! change mode : KM_READY
				adc_KeyRestart(i);
			}
			break;
		default:
			//Fail ! change mode : KM_READY
			adc_KeyRestart(i);
			break;
	}
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : adc_KeyRestart
INPUT        : 6EA ADC , each other key line
OUTPUT       :
DESCRIPTION  : restart key check
COMMENTS     : 
-----------------------------------------------------------------------------*/
void adc_KeyRestart(unsigned char i)
{
	key.KeyOk[i] = OFF;
	key.KeyMode[i] = KM_READY;
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : adc_KeyBoardPer10ms
INPUT        :
OUTPUT       :
DESCRIPTION  : update data for vfd display
COMMENTS     : 
-----------------------------------------------------------------------------*/
void adc_KeyBoardPer10ms(void)
{
	byte i;
	
    if (mode_PowerStatus >= POWER_ON2_ING) {			//20131216 : used
    	//power off 시, ADC 기능 없기 때문에 return 안됨 그래서 삭제함...
		if (RKB_mode_PP_MP == 0) {						//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2)
    		//ADC Tri(4) + ADC Temp(2)    
	    	adc_KeyRead(6);
			for (i=0;i<6;i++) adc_KeyMake(i);
		}else{	    
    		//ADC Tri(4) + ADC Temp(2) + ADC Temp(2)	//add SMPS Heat Sink sensor
	    	adc_KeyRead(8);
			for (i=0;i<8;i++) adc_KeyMake(i);
		}
	}
}


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/
