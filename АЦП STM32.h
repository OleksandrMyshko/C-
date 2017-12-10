#include"stm32f0xx.h"
#include"stm32f0xx_rcc.h"
#include"stm32f0xx_gpio.h"
#include"stm32f0xx_tim.h"
#include"stm32f0xx_misc.h"
#include"stm32f0xx_adc.h"
#include"stm32f0xx_syscfg.h"
#include"stm32f0xx_exti.h"

//оголошення та ініціалізація глобальних змінних, що використовуються в подальшому
uint16_tPrescalerValue = 0;
uint8_t seven_seg_codes[10]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
uint16_t seven_seg_data;
uint8_t digit_num=1;
int n=0, m=0;
uint16_t ADC1ConvertedValue = 0;
float ADC1ConvertedVoltage = 0;

//оголошення прототипів методів для конфігурації задіяних частин МК та периферії (таймери, індикатори, АЦП, переривання, розбиттянарозряди)
voidTIM_Config(void);
voidLEDsInit(void);
voidADC_init(void);
voidEXTI0_Config(void);
uint8_tGetDigit(int data, uint8_t digit_num);

intmain(void) //основна програма
{
//виклик методів конфігурації частин МК і периферії
TIM_Config();
ADC_init();
LEDsInit();
  EXTI0_Config();

while (1) //нескінченний цикл
  {
  }
} //кінець основної програми

voidEXTI0_Config(void) //налаштування переривання
{
	EXTI_InitTypeDefEXTI_InitStructure; //оголошення структур для регістрів переривань, портувведення\виведення
	GPIO_InitTypeDefGPIO_InitStructure;
	NVIC_InitTypeDefNVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE); //увімкення периферії
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //призначення піна №2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//налаштування режимуроботи піна
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//налаштування «підтяжки» доплюса
	GPIO_Init(GPIOD, &GPIO_InitStructure);//ініціалізація структури для регістру GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//увімкнення перифіерії
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;//призначення переривання на другу лінію
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//налаштування переривання
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//ініціалізація структури для регістру EXTI
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;//налаштування каналу переривання
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;//пріорітет переривання
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//увімкнення переривання
	NVIC_Init(&NVIC_InitStructure);//ініціалізація структури для регістру NVIC
}

voidTIM_Config(void)//налашутвання таймера
{
	TIM_TimeBaseInitTypeDefTIM_TimeBaseStructure; //оголошення структур для таймерів
	NVIC_InitTypeDefNVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //увімкнення периферії
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//встановлення каналу таймера
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;//пріорітет переривання
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);//ініціалізація структури для NVIC
	PrescalerValue = (uint16_t) (SystemCoreClock  / 40000) - 1;//допоміжна змінна для частоти таймера
	TIM_TimeBaseStructure.TIM_Period = 100-1;//встановлення частоти таймера
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//ініціалізація структури для таймера
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //увімкнення переривання таймера
	TIM_Cmd(TIM2, ENABLE);//увімкнення лічильника таймера
}

voidLEDsInit(void)//метод дляналаштування семисегментних індикаторів
{
GPIO_InitTypeDefGPIO_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
GPIO_InitStructure.GPIO_Pin = 0xFFF;//задіяння всіх індикаторів
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//частота опитування
GPIO_Init(GPIOA, &GPIO_InitStructure);
}

voidADC_init(void)//метод для налаштування АЦП
{
//оголошення структур для роботи з АЦП
	ADC_InitTypeDefADC_InitStructure;
	GPIO_InitTypeDefGPIO_InitStructure;
	TIM_TimeBaseInitTypeDefTIM_TimeBaseStructure;
	TIM_OCInitTypeDefTIM_OCInitStructure;
	NVIC_InitTypeDefNVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);//увімкнення необхідної периферії
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//увімкнення переривання АЦП 
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//налаштування каналу 1 АЦП як аналового входу
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//налаштування таймера 3
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_OCStructInit(&TIM_OCInitStructure);
	PrescalerValue = (uint16_t) (SystemCoreClock  / 10000) - 1;
	TIM_TimeBaseStructure.TIM_Period = 100-1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//налаштування тригера для таймера
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	ADC_DeInit(ADC1);

	//налаштування АЦП з розрядністю 12 біт в постійному режимі та спрацюванням за тригером
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure); //ініціалізація структури для роботи з АЦП
	ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_28_5Cycles);//встановлення частоти дискретизації АЦП
	ADC_GetCalibrationFactor(ADC1);
	ADC_WaitModeCmd(ADC1, ENABLE);
	ADC_AutoPowerOffCmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); //очікування флагу
	TIM_Cmd(TIM3, ENABLE);
	ADC_ITConfig  ( ADC1,ADC_IT_EOC,ENABLE);
	ADC_StartOfConversion(ADC1); //запуск АЦП
}

voidADC1_COMP_IRQHandler (void)//метод налаштування обробника переривання АЦП
{
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) != RESET)//очікування флагу
	{
		ADC1ConvertedValue = ADC_GetConversionValue(ADC1);//зчитування значення напруги з потенціометра і переведення його у код АЦП
		ADC1ConvertedVoltage = (ADC1ConvertedValue * 3300)/0xFFF;//переведення коду АЦП в одиниці напруги
		ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);//очищення флагу переривання
	}
}
voidEXTI2_3_IRQHandler(void)//метод налаштування переривання для роботи кнопки
{
	if(EXTI_GetFlagStatus(EXTI_Line2) != RESET)//очікування флагу
		{
		if (n==0) {
			TIM_Cmd(TIM2, ENABLE);//увімкнення роботи таймера
			n=1;
		}
		elseif (n==1) {
			GPIO_Write(GPIOA, 0);//вимкнення індикаторів (всі індикатори погасли)
			TIM_Cmd(TIM2, DISABLE);//вимкнення таймера

			n=0;
		}

		EXTI_ClearITPendingBit(EXTI_Line2);//очищення флагу переривання
		}
}

uint8_tGetDigit(int data, uint8_t digit_number)//метод для визначення розрядів і розбиття отриманих кодів АЦП на розряди
{
	if(((data>=0)&&(data<=9999))&&((digit_number>=1)&&(digit_number<=4)))
	{
		uint8_t d1,d2,d3,d4;
		d4=0;
		d3=0;
		d2=0;
		d1=0;
		d1=data/1000;
		d2=(data%1000)/100;
		d3=(data%100)/10;
		d4=(data%10);
		switch(digit_number)//переключення для виведення кожного розряду
		{
			case 1:
			{
				return d1;
				break;
			}
			case 2:
			{
				return d2;
				break;
			}
			case 3:
			{
				return d3;
				break;
			}
			case 4:
			{
				return d4;
				break;
			}
		}
	}
}


voidTIM2_IRQHandler(void)//метод для опрацювання переривання таймера 2
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET)//очікування флагу
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//очищення флагу
		seven_seg_data=seven_seg_codes[GetDigit(ADC1ConvertedValue, digit_num) ]| ((1<<(digit_num))<<7);//запис до масиву порозрядно розбитого виміряного значення кодів напруги з АЦП
		GPIO_Write(GPIOA, seven_seg_data);//виведення на індикатори виміряного поточного значення
		digit_num++;//інкремент розряду
		if(digit_num>4)//якщо розряд останній, то перейти знову на перший
			digit_num = 1;
	}
}
