#include <intrins.h>
#include <STC8.H>
void Delay1us (unsigned int b) 		//@22.1184MHz
{
	while(b)
	{
		unsigned char i;

		_nop_();
		i = 5;
		while (--i);
		b--;
	}
}

void Delay1ms (unsigned int a) 		//@22.1184MHz
{
	while(a)
	{
		unsigned char i, j;

		i = 29;
		j = 183;
		do
		{
			while (--j);
		} while (--i);
		a--;
	}
}

