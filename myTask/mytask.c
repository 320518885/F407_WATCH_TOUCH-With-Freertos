
/* ���ߣ�LiZiMing */



#include "mytask.h"

#include <string.h>
#include "delay.h"
#include "lcd.h"
#include "lcd_init.h"
#include "CST816.h"
#include "key.h"

#include "adc.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demo_keypad_encoder.h"

#include <stdio.h>
#include <string.h>
#include "gui_guider.h"
#include "events_init.h"
   
#include "FreeRTOS.h"
#include "tim.h"
#include "cmsis_os.h"

lv_ui guider_ui;

#define LD0 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)   
#define LD1 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)
uint8_t adc_AD8232;
uint8_t L1,L0;


extern void update_label_3_with_value(lv_ui *ui, int new_value);


void mytask(void const * argument)
{
  /* USER CODE BEGIN mytask */
	LCD_Init();
  
  extern CST816_Info CST816_Instance;
  CST816_Init();
  CST816_RESET();
  
  lv_init();
  lv_port_disp_init();  // lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
  lv_port_indev_init(); // lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���

	
  setup_ui(&guider_ui);
  events_init(&guider_ui);
	
  HAL_TIM_Base_Start_IT(&htim3);
	
  /* Infinite loop */
  while(1)
  {
	
		lv_task_handler(); // lvgl��������
    osDelay(1);
  }
  /* USER CODE END mytask */
}

uint16_t ADC_IN_1(void) //ADC�ɼ�����
{
    
 	HAL_ADC_Start(&hadc1);//��ʼADC�ɼ�
    
 	HAL_ADC_PollForConversion(&hadc1,500);//�ȴ��ɼ�����
    
 	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))//��ȡADC��ɱ�־λ
    
 	{
    
 		return HAL_ADC_GetValue(&hadc1);//����ADC��ֵ
    
 	}
    
 	return 0;
    
}
 
void ADC_Task(void *argument)
{
  /* USER CODE BEGIN ADC_Task */

  /* Infinite loop */
  for(;;)
  {
		L0=LD0;
		L1=LD1;
		if(LD0==1||LD1==1){
    
// 			printf("error");		
    
 		}
    
 		else{
    
 				adc_AD8232 = ADC_IN_1()/10;
				update_label_3_with_value(&guider_ui,adc_AD8232);
// 				printf("%d",adc_AD8232);
    
 		}
    osDelay(500);
  }
  /* USER CODE END ADC_Task */
}


void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
	
	
	lv_tick_inc(1); // LVGL��1ms�ж�
}

