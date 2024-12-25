/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "lcd.h"
#include "dma.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 280 is used for now.
    #define MY_DISP_HOR_RES    280
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static lv_disp_drv_t disp_drv;                         /*��ʾ������������*/

void lv_port_disp_init(void)
{
    /*-------------------------
     * ��ʼ�������ʾ��
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * ������ͼ������
     *----------------------------*/

    /**
     * LVGL��Ҫһ�����������������ڲ�����С������
     * ��������������������ݸ������ʾ������`flush_cb`���Խ����ݸ��Ƶ������ʾ���ϡ�
     * �������������1����ʾ��
     *
     * ��3�ֻ������ã�
     * 1. ����һ����������
     *      LVGL���ڴ˻������л�����ʾ���ݲ�����д�������ʾ����
     *
     * 2. ����������������
     *      LVGL����һ���������л�����ʾ���ݲ�����д����ʾ����
     *      ��Ӧ��ʹ��DMA��������������д����ʾ����
     *      �⽫ʹLVGL�ܹ��ڵ�һ�����������ݷ��͹����У�����Ļ����һ���ֻ��Ƶ���һ������������ʹ��Ⱦ��ˢ�²��С�
     *
     * 3. ˫���壺
     *      ����2����Ļ��С�Ļ�������������disp_drv.full_refresh = 1��
     *      ������LVGL����`flush_cb`��ʼ���ṩ������Ⱦ��Ļ����ֻ�����֡�������ĵ�ַ��
     */

#define LV_VER_RES_MAX 240

#define MODE 2

   #if MODE == 1
	/* ʾ�� 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10];                          /*һ��10�еĻ�����*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10);   /*��ʼ����ʾ������*/
	#endif
	
	#if MODE == 2
     /* ʾ�� 2) */
     static lv_disp_draw_buf_t draw_buf_dsc_2;
     static lv_color_t buf_2_1[MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10];                        /*һ��10�еĻ�����*/
     static lv_color_t buf_2_2[MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10];                        /*��һ��10�еĻ�����*/
     lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10);   /*��ʼ����ʾ������*/
	#endif
	
	#if MODE == 3
     /* ʾ�� 3) ͬʱ���� disp_drv.full_refresh = 1 */
     static lv_disp_draw_buf_t draw_buf_dsc_3;
     static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*һ����Ļ��С�Ļ�����*/
     static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*��һ����Ļ��С�Ļ�����*/
     lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                           MY_DISP_VER_RES * LV_VER_RES_MAX);   /*��ʼ����ʾ������*/
	#endif
    /*-----------------------------------
     * ��LVGL��ע����ʾ��
     *----------------------------------*/

    
	
    lv_disp_drv_init(&disp_drv);                    /*������ʼ��*/

    /*���÷��������ʾ���ĺ���*/

    /*������ʾ���ķֱ���*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*���ڽ������������ݸ��Ƶ���ʾ��*/
    disp_drv.flush_cb = disp_flush;

    /*������ʾ������*/
    disp_drv.draw_buf = &draw_buf_dsc_2;

    /*����ʾ�� 3)*/
    //disp_drv.full_refresh = 1;

    /* �������GPU����������ɫ����ڴ����顣
     * ע�⣬��lv_conf.h�п�������LVGL����֧�ֵ�GPU��
     * ��������в�ͬ��GPU������ʹ������ص���*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*���ע������*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}


/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
//    if(disp_flush_enabled) {
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

//        int32_t x;
//        int32_t y;
//        for(y = area->y1; y <= area->y2; y++) {
//            for(x = area->x1; x <= area->x2; x++) {
//                /*Put a pixel to the display. For example:*/
//                /*put_px(x, y, *color_p)*/
//				LCD_DrawPoint(x,y,color_p->full);
//                color_p++;
//            }
//        }
//		LCD_Color_Fill(area->x1,area->y1,area->x2,area->y2,color_p);
		LCD_Color_Fill1(area->x1,area->y1,area->x2,area->y2,(u16 *)color_p);
//    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
//    lv_disp_flush_ready(disp_drv);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
	lv_disp_flush_ready(&disp_drv);
}
/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
