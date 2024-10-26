/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
/* RCC system reset(for debug purpose) */
RCC_DeInit();
RCC_HSEConfig(RCC_HSE_ON);                  /* Enable HSE */
HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready */
if (HSEStartUpStatus == SUCCESS)
{
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); /* Enable Prefetch Buffer */
  FLASH_SetLatency(FLASH_Latency_2);                    /* Flash 2 wait state */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);                      /* HCLK = SYSCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);                       /* PCLK2 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div2);                       /* PCLK1 = HCLK/2 */
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  /* PLLCLK = 8MHz * 9 = 72 MHz */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);               /* Select HSE as system clock source */
  RCC_PLLCmd(ENABLE);                                   /* Enable PLL */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  };                                         /* Wait till PLL is ready */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); /* Select PLL as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }; /* Wait till PLL is used as system clock source */
}
else
{ /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */
  while (1)
  {
  } /* Go to infinite loop */
}
RCC_ADCCLKConfig(RCC_PCLK2_Div2);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // ���������� ������
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

/*==================== ���������� ������ ������������ ===================*/
TIM_TimeBaseStructure.TIM_Period = 50 - 1;      // 1 ��������.
TIM_TimeBaseStructure.TIM_Prescaler = 1440 - 1; // ����� = 20 ���
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
TIM_ARRPreloadConfig(TIM2, ENABLE);
/*====================== ��������� ���������� ======================*/
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
TIM_Cmd(TIM2, ENABLE); /* TIM enable counter */

/*====================== DWT ======================*/
SCB_DEMCR |= 0x01000000; // ��������� ������������ �������
DWT_CYCCNT = 0;          // �������� �������� �������� ��������
DWT_CONTROL |= 1;        // ��������� �������
