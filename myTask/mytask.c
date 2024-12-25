
/* 作者：LiZiMing */



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
  lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面
  lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面

	
  setup_ui(&guider_ui);
  events_init(&guider_ui);
	
  HAL_TIM_Base_Start_IT(&htim3);
	
  /* Infinite loop */
  while(1)
  {
	
		lv_task_handler(); // lvgl的事务处理
    osDelay(1);
  }
  /* USER CODE END mytask */
}

uint16_t ADC_IN_1(void) //ADC采集程序
{
    
 	HAL_ADC_Start(&hadc1);//开始ADC采集
    
 	HAL_ADC_PollForConversion(&hadc1,500);//等待采集结束
    
 	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))//读取ADC完成标志位
    
 	{
    
 		return HAL_ADC_GetValue(&hadc1);//读出ADC数值
    
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
	
	
	lv_tick_inc(1); // LVGL的1ms中断
}

