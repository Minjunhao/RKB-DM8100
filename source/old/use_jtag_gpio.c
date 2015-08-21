//
// use JTAG pin => gpio
//
//
void use_jtag_pin_gpio(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // Enable AFIO clock

 // Full SWJ Disabled (JTAG-DP + SW-DP)
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
 // Configure PA.13 (JTMS/SWDAT), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as input
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 // Configure PB.03 (JTDO) and PB.04 (JTRST) as input
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
}



