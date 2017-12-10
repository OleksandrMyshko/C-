#include<reg51.h>
#include<math.h>
unsigned char xdata *DAC = 0x8000;/  об’явлення вказівника для роботи з ЦАП
unsigned char xdata *LI = 0xB000;
unsigned char xdata *HI = 0xA000;
unsigned char xdata *PRT = 0x8003; / забезпечує роботу із платою розширення стенду
int y, s, choose = 0, mode;
int i, z;
unsigned char sig[70]; / масив з 70 елементів
void preriv (void)interrupt 0 using	1
{
	choose ++;
	for(z=0;z<2000;z++){}
		if (choose>3)
			{choose=0;} / реалізація кнопки для зміни розрядності псевдовипадкового числа
	}
	void preriv1 (void)interrupt 2 using	1
	{
		mode ++;
		for(z=0;z<2000;z++){}
		if (mode >4)
			{ mode =0;} / реалізація кнопки для зміни частоти 
	}

	void timer0 (void) interrupt 1 using 1
	{
		TH0 = 0;
		TL0 = 0;
		y=rand();
		for(i=0;i<70;i++)
		{
			sig[i]=y,
			i++,
			if(i>70)
				{i=0;}
		}
		} / підпрограма преривання 
	void main()
	{
		int k1, k2;
		IE=0x8F;
		TMOD=0x80;
		*PRT=0x80;
		TR0=1;
		while(1)
		{
		switch(mode) / структура для кнопки зміни частоти
		{
			case 0;
			mode=2000;
			break;
			case 1;
			mode=5000;
			break;
			case 2;
			mode=10000;
			break;
			case 3;
			mode=20000;
			break;
		}
		switch(choose) / структура для зміни кнопки розрядності числа
		{
			case 0;
			for(i=0;i<70;i++){
				k1=sig[i]/100;
				k2= sig[i]%100;
				*H1=k1;
				*L1=k2;
				*DAC= sig[i];
				for(z=0;z<mode;z++){}
			}
			break;
			case 1;
			for(i=0;i<70;i++){ 
				sig[i]= sig[i]%100;
				k1=sig[i]/100;
				k2= sig[i]%100;
				*H1=k1;
				*L1=k2;
				*DAC= sig[i];
				for(z=0;z<mode;z++){}
			}
			break;
			case 2;
			for(i=0;i<70;i++){ 
				sig[i]= sig[i]%10;
				k1=sig[i]/100;
				k2= sig[i]%100;
				*H1=k1;
				*L1=k2;
				*DAC= sig[i];
				for(z=0;z<mode;z++){}
			}
			break;
		}
	}
}
