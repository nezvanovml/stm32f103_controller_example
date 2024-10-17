/*
 File:	startup.c
 Date:	17.04.2011

*/

extern unsigned long _data_flash;
extern unsigned long _data_begin;
extern unsigned long _data_end;
extern unsigned long _bss_begin;
extern unsigned long _bss_end;
extern unsigned long _stack_end;

extern int main(void);
extern void SystemInit();
extern void TIM2_IRQHandler(void);
extern void USART1_IRQHandler();

//-----------------------------------------------------------------------------
void handler_reset(void)
{
	unsigned long *source;
	unsigned long *destination;

	// копируем данные из флэш в ОЗУ
	source = &_data_flash;
	for (destination = &_data_begin; destination < &_data_end;)
	{
	  *(destination++) = *(source++);
	}

	// обнуляем
	for (destination = &_bss_begin; destination < &_bss_end;)
	{
	  *(destination++) = 0;
	}

	SystemInit();
	// переход в основную программу
	main();
}

//-----------------------------------------------------------------------------
void handler_default(void)
{
	while (1) {  }
}

//-----------------------------------------------------------------------------
__attribute__ ((section(".interrupt_vector")))
void (* const table_interrupt_vector[])(void) =
{
		(void *) &_stack_end, // записывем указатель на конец стека
		handler_reset,		// 1
		handler_default,	// 2
		handler_default,	// 3
		handler_default,	// 4
		handler_default,	// 5
		handler_default,	// 6
		0,			// 7
		0,			// 8
		0,			// 9
		0,			// 10
		handler_default,	// 11
		handler_default,	// 12
		0,			// 13
		handler_default,	// 14
		handler_default,	// 15
		// периферия
		handler_default,	// 0
		handler_default,	// 1
		handler_default,	// 2
		handler_default,	// 3
		handler_default,	// 4
		handler_default,	// 5
		handler_default,	// 6
		handler_default,	// 7
		handler_default,	// 8
		handler_default,	// 9
		handler_default,	// 10
		handler_default,	// 11
		handler_default,	// 12
		handler_default,	// 13
		handler_default,	// 14
		handler_default,	// 15
		handler_default,	// 16
		handler_default,	// 17
		handler_default,	// 18
		handler_default,	// 19
		handler_default,	// 20
		handler_default,	// 21
		handler_default,	// 22
		handler_default,	// 23
		handler_default,	// 24
		handler_default,	// 25
		handler_default,	// 26
		handler_default,	// 27

		TIM2_IRQHandler,	// 28

		handler_default,	// 29
		handler_default,	// 30
		handler_default,	// 31
		handler_default,	// 32
		handler_default,	// 33
		handler_default,	// 34
		handler_default,	// 35
		handler_default,	// 36

		handler_default,	// 37
//		USART1_IRQHandler,

		handler_default,	// 38
		handler_default,	// 39
		handler_default,	// 40
		handler_default,	// 41
		handler_default,	// 42
		handler_default,	// 43
		handler_default,	// 44
		handler_default,	// 45
		handler_default,	// 46
		handler_default,	// 47
		handler_default,	// 48
		handler_default,	// 49
		handler_default,	// 50
		handler_default,	// 51
		handler_default,	// 52
		handler_default,	// 53
		handler_default,	// 54
		handler_default,	// 55
		handler_default,	// 56
		handler_default,	// 57
		handler_default,	// 58
		handler_default,	// 59
		handler_default 	// 60
};

/* КОНЕЦ */
