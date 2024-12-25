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

static lv_disp_drv_t disp_drv;                         /*显示驱动的描述符*/

void lv_port_disp_init(void)
{
    /*-------------------------
     * 初始化你的显示器
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * 创建绘图缓冲区
     *----------------------------*/

    /**
     * LVGL需要一个缓冲区，在其中内部绘制小部件。
     * 后来这个缓冲区将被传递给你的显示驱动的`flush_cb`，以将内容复制到你的显示器上。
     * 缓冲区必须大于1行显示。
     *
     * 有3种缓冲配置：
     * 1. 创建一个缓冲区：
     *      LVGL将在此缓冲区中绘制显示内容并将其写入你的显示器。
     *
     * 2. 创建两个缓冲区：
     *      LVGL将在一个缓冲区中绘制显示内容并将其写入显示器。
     *      你应该使用DMA将缓冲区的内容写入显示器。
     *      这将使LVGL能够在第一缓冲区的数据发送过程中，将屏幕的下一部分绘制到另一个缓冲区。它使渲染和刷新并行。
     *
     * 3. 双缓冲：
     *      设置2个屏幕大小的缓冲区，并设置disp_drv.full_refresh = 1。
     *      这样，LVGL将在`flush_cb`中始终提供整个渲染屏幕，你只需更改帧缓冲区的地址。
     */

#define LV_VER_RES_MAX 240

#define MODE 2

   #if MODE == 1
	/* 示例 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10];                          /*一个10行的缓冲区*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10);   /*初始化显示缓冲区*/
	#endif
	
	#if MODE == 2
     /* 示例 2) */
     static lv_disp_draw_buf_t draw_buf_dsc_2;
     static lv_color_t buf_2_1[MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10];                        /*一个10行的缓冲区*/
     static lv_color_t buf_2_2[MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10];                        /*另一个10行的缓冲区*/
     lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES *MY_DISP_VER_RES/ 10);   /*初始化显示缓冲区*/
	#endif
	
	#if MODE == 3
     /* 示例 3) 同时设置 disp_drv.full_refresh = 1 */
     static lv_disp_draw_buf_t draw_buf_dsc_3;
     static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*一个屏幕大小的缓冲区*/
     static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*另一个屏幕大小的缓冲区*/
     lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                           MY_DISP_VER_RES * LV_VER_RES_MAX);   /*初始化显示缓冲区*/
	#endif
    /*-----------------------------------
     * 在LVGL中注册显示器
     *----------------------------------*/

    
	
    lv_disp_drv_init(&disp_drv);                    /*基本初始化*/

    /*设置访问你的显示器的函数*/

    /*设置显示器的分辨率*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*用于将缓冲区的内容复制到显示器*/
    disp_drv.flush_cb = disp_flush;

    /*设置显示缓冲区*/
    disp_drv.draw_buf = &draw_buf_dsc_2;

    /*用于示例 3)*/
    //disp_drv.full_refresh = 1;

    /* 如果你有GPU，可以用颜色填充内存数组。
     * 注意，在lv_conf.h中可以启用LVGL内置支持的GPU。
     * 但如果你有不同的GPU，可以使用这个回调。*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*最后注册驱动*/
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
