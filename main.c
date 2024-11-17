#define DWT_CYCCNT *(volatile uint32_t *)0xE0001004
#define DWT_CONTROL *(volatile uint32_t *)0xE0001000
#define SCB_DEMCR *(volatile uint32_t *)0xE000EDFC
#include <string.h>
#include <stm32f10x.h>
#include "stm32f10x_spi.h"
#include "stm32f10x_iwdg.h"

/*
V_Button 1 - Reset MCU
IO BTN 5 - Reset MCU
*/

/* Enable iwdg */
#define USE_WATCHDOG 1

/* Connection of  I/O Module. */
#define IOModuleConnection 2 // 1 - relay, sensors, leaks; 2 - ventilation, light, access, watering

/* Connection of  W5500 Module. */
#define W5500Connection 1 // all controllers

/* Connection of  DS18B20 (DALLAS) Module. */
// #define DS18B20Connection 2 // 1 - relay, sensors; 2 - ventilation

/* Connection of relays */
#define RelayConnection 4 // 1 - relay; 2 - ventilation; 3 - lights; 4 - access, watering; 5 - waterleaks

/* Connection of inputs */
#define InputConnection 1 // 1 - access, watering; 2 - waterleaks; 3 - FSCD, move

/* Virtual entities */
#define VIRTUAL_NUM_OF_SWITCH 0
#define VIRTUAL_NUM_OF_BUTTON 1
#define VIRTUAL_NUM_OF_BINARY_SENSOR 2
#define VIRTUAL_NUM_OF_NUMERIC 1

/* Setting diapason for numeric (MIN/MAX_1...10)*/
#define VIRTUAL_NUMERIC_MIN_1 20
#define VIRTUAL_NUMERIC_MAX_1 50

#define W5500_NETWORK 172, 16, 30, 0
#define W5500_NETMASK 255, 255, 255, 0

#define DEVICE_INDEX 48

/* device_index - used for ip/mac assigning, system_loaded - indicates mcu are loaded, need_mcu_reset - if set to 1 MCU will reset */
uint8_t device_index = 0, system_loaded = 0, need_mcu_reset = 0;

GPIO_InitTypeDef Init_PORT;
SPI_InitTypeDef SPI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
ErrorStatus HSEStartUpStatus;
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

#include "modules/init.c"

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		modules_interruptions(); // interruptions for modules
		if (system_loaded)
		{ // user interruptions here
			;
		}
	}
}

/* ====================== MAIN ====================*/
int main(void)
{
// Initializing MCU
#include "maginit.c"
	// Initializing modules
	modules_init();

	struct HttpRequest http_request[SOCKET_NUM];
	for(uint8_t sn = 0; sn < SOCKET_NUM; sn++){
		http_request[sn].status_code = 200;
		http_request[sn].request[0] = '\0';
		http_request[sn].response[0] = '\0';
	}

	while (1)
	{
#ifndef W5500_NETWORK
		//  dhcp processing
		DHCP_run();
#endif

		if (virtual_button_get_state(1))
			need_mcu_reset = 1;
		if (io_module_button_pressed(5))
			NVIC_SystemReset(); // Reset MCU



		for(uint8_t sn = 0; sn < SOCKET_NUM; sn++){
			switch (http_server_process(sn, 80, &http_request[sn]))
			{
			case HTTP_RECEIVED:
				// Here you can parse request & make response
				LEDON(14);
				// check request type: if(strcmp(http_request.method, "POST") == 0) {}
				if (strcmp(http_request[sn].method, "POST") == 0)
				{
					if (strcmp(http_request[sn].path, "/relay") == 0)
					{
						char key[20], value[64];
						uint16_t res = 0, start = 0;
						// parsing body args
						xsprintf(http_request[sn].response, "{\"error\":null}");
						while ((res = get_param(http_request[sn].args + start, key, value)) > 0)
						{
							start += res;
							uint16_t channel = get_int_from_str(key);
							if (value[0] == '1')
							{
								RELAY_ON(channel);
							}
							else if (value[0] == '0')
							{
								RELAY_OFF(channel);
							}
							else if (value[0] == 'i')
							{
								RELAY_IMPULSE(channel);
							}
						}
						http_request[sn].status_code = 200;
					}
					else if (strcmp(http_request[sn].path, "/v_numeric") == 0)
					{
						char key[20], value[64];
						uint16_t res = 0, start = 0;
						// parsing body args
						xsprintf(http_request[sn].response, "{\"error\":null}");
						while ((res = get_param(http_request[sn].args + start, key, value)) > 0)
						{
							start += res;
							uint16_t channel = get_int_from_str(key);
							virtual_numeric_set_value(channel, get_int_from_str(value));
						}
						http_request[sn].status_code = 200;
					}
					else if (strcmp(http_request[sn].path, "/v_switch") == 0)
					{
						char key[20], value[64];
						uint16_t res = 0, start = 0;
						// parsing body args
						xsprintf(http_request[sn].response, "{\"error\":null}");
						while ((res = get_param(http_request[sn].args + start, key, value)) > 0)
						{
							start += res;
							uint16_t channel = get_int_from_str(key);
							if (value[0] == '1')
							{
								VIRTUAL_SWITCH_ON(channel);
							}
							else if (value[0] == '0')
							{
								VIRTUAL_SWITCH_OFF(channel);
							}
						}
						http_request[sn].status_code = 200;
					}
					else if (strcmp(http_request[sn].path, "/v_button") == 0)
					{
						char key[20], value[64];
						uint16_t res = 0, start = 0;
						// parsing body args
						xsprintf(http_request[sn].response, "{\"error\":null}");
						while ((res = get_param(http_request[sn].args + start, key, value)) > 0)
						{
							start += res;
							uint16_t channel = get_int_from_str(key);
							if (value[0] == '1')
							{
								VIRTUAL_BUTTON_PRESS(channel);
							}
						}
						http_request[sn].status_code = 200;
					}
					else
					{
						xsprintf(http_request[sn].response, "{\"error\":\"NOT_FOUND\"}");
						http_request[sn].status_code = 404;
					}
				}
				else if (strcmp(http_request[sn].method, "GET") == 0)
				{
					if (strcmp(http_request[sn].path, "/system_info") == 0)
					{
						collect_system_info(http_request[sn].response);
						http_request[sn].status_code = 200;
					}
					else if (strcmp(http_request[sn].path, "/state") == 0)
					{
						collect_state(http_request[sn].response);
						http_request[sn].status_code = 200;
					}
					else
					{
						xsprintf(http_request[sn].response, "{\"error\":\"NOT_FOUND\"}");
						http_request[sn].status_code = 404;
					}
				}
				else
				{
					xsprintf(http_request[sn].response, "{\"error\":\"METHOD_NOT_ALLOWED\"}");
					http_request[sn].status_code = 405;
				}
				break;
			case HTTP_SENT:
				// Here you can reset counters etc if data sent successfully
				if (need_mcu_reset)
					NVIC_SystemReset(); // Reset MCU
				LEDON(14);
				break;
			case HTTP_ERROR:
				LEDBLINKFAST(14);
				xsprintf(http_request[sn].response, "{\"error\":\"BAD_REQUEST\"}");
				http_request[sn].status_code = 400;
				break;
			case HTTP_IDLE:
				// No request
				LEDBLINK(14);
				break;
			default:
				break;
			}
		}
	}
};
