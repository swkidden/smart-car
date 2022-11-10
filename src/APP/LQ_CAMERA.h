/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】北�?龙邱智能科技TC264DA核心�?
【编    写】ZYF/chiusir
【E-mail  】chiusir@163.com
【软件版�?】V1.1 版权所有，单位使用请先联系授权
【最后更新�?2020�?10�?28�?
【相关信�?参考下列地址�?
【网    站】http://www.lqist.cn
【淘宝店铺】http://longqiu.taobao.com
------------------------------------------------
【dev.env.】AURIX Development Studio1.2.2及以上版�?
【Target �? TC264DA/TC264D
【Crystal�? 20.000Mhz
【SYS PLL�? 200MHz
________________________________________________________________
基于iLLD_1_0_1_11_0底层程序,

使用例程的时候，建�??采用没有空格的英文路径，
除了CIF为TC264DA�?有�?�，其它的代码兼容TC264D
�?库默认初始化了EMEM�?512K，�?�果用户使用TC264D，注释掉EMEM_InitConfig()初�?�化函数�?
工程下\Libraries\iLLD\TC26B\Tricore\Cpu\CStart\IfxCpu_CStart0.c�?164行左右�?
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#ifndef __LQ_CAMERA_H
#define __LQ_CAMERA_H

#include "LQ_MT9V034.h"


#define LQMT9V034   //默�?�为神眼摄像�?
#define USETFT1_8
//#define USEOLED    //使用OLED或者TFT1.8模块，默认为TFT1.8

#define IMAGEH  MT9V034_IMAGEH   /*!< 摄像头采集高�? */
#define IMAGEW  MT9V034_IMAGEW   /*!< 摄像头采集�?�度 */

/* 使用数组宽高 �?改这里即�? */
#ifdef USEOLED
	#define LCDH    60  /*!< OLED显示高度（用户使�?）高�? */
	#define LCDW    80  /*!< OLED显示宽度（用户使�?）�?�度 */
#else
	#define LCDH    60  /*!< TFT显示高度（用户使�?）高�? */
	#define LCDW    94  /*!< TFT显示宽度（用户使�?）�?�度 */
#endif

//#define SMALLIMAGE  // 使用小尺寸显�?60*94
#define MAX_ROW   LCDH
#define MAX_COL   LCDW

/** 图像原�?�数�?存放 */
extern unsigned char Image_Data[IMAGEH][IMAGEW];

/** 压缩后之后用于存放屏幕显示数�?  */
extern unsigned char Image_Use[LCDH][LCDW];

/** 二值化后用于OLED显示的数�? */
extern unsigned char Bin_Image[LCDH][LCDW];
extern unsigned char Edge_arr[LCDH][LCDW]; 

/*!
  * @brief    串口上报上位�?
  *
  * @param    �?
  *
  * @return   �?
  *
  * @note     上位机的帧头�?能有所区别 
  *
  * @see      CAMERA_Reprot();
  *
  * @date     2019/9/24 星期�?
  */
void CAMERA_Reprot(void);



/*!
  * @brief    摄像头测试例�?
  *
  * @param    fps:  帧率 
  *
  * @return   �?
  *
  * @note     摄像头的一些参数，在LQ_MT9V034.c�?的宏定义�?�?�?
  *
  * @see      CAMERA_Init(50);   //初�?�化MT9V034  50�? 注意使用白色带与非门版转接座
  *
  * @date     2019/10/22 星期�?
  */
void CAMERA_Init(unsigned char fps);


/**
  * @brief    获取需要使用的图像数组
  *
  * @param    �?
  *
  * @return   �?
  *
  * @note     �?
  *
  * @see      Get_Use_Image();
  *
  * @date     2019/6/25 星期�?
  */
void Get_Use_Image(void);


/**
  * @brief    二值化
  *
  * @param    mode  �?  0：使用大津法阈�?    1：使用平均阈�?
  *
  * @return   �?
  *
  * @note     �?
  *
  * @example  Get_Bin_Image(0); //使用大津法二值化
  *
  * @date     2019/6/25 星期�?
  */
void Get_Bin_Image(unsigned char mode);



/*!
  * @brief    大津法求阈值大�? 
  *
  * @param    tmImage �? 图像数据
  *
  * @return   阈�?
  *
  * @note     参考：https://blog.csdn.net/zyzhangyue/article/details/45841255
  * @note     https://www.cnblogs.com/moon1992/p/5092726.html
  * @note     https://www.cnblogs.com/zhonghuasong/p/7250540.html     
  * @note     Ostu方法又名最大类间差方法，通过统�?�整�?图像的直方图特性来实现全局阈值T的自动选取，其算法步�?�为�?
  * @note     1) 先�?�算图像的直方图，即将图像所有的像素点按�?0~255�?256个bin，统计落在每个bin的像素点数量
  * @note     2) 归一化直方图，也即将每个bin�?像素点数量除以总的像素�?
  * @note     3) i表示分类的阈值，也即一�?灰度级，�?0开始迭�?	1
  * @note     4) 通过归一化的直方图，统�??0~i 灰度级的像素(假�?�像素值在此范围的像素�?做前�?像素) 所占整幅图像的比例w0，并统�?�前�?像素的平均灰�?u0；统�?i~255灰度级的像素(假�?�像素值在此范围的像素�?做背�?像素) 所占整幅图像的比例w1，并统�?�背�?像素的平均灰�?u1�?
  * @note     5) 计算前景像素和背�?像素的方�? g = w0*w1*(u0-u1) (u0-u1)
  * @note     6) i++；转�?4)，直到i�?256时结束迭�?
  * @note     7) 将最�?g相应的i值作为图像的全局阈�?
  * @note     缺陷:OSTU算法在�?�理光照不均匀的图像的时候，效果会明显不好，因为利用的是全局像素信息�?
  * @note     解决光照不均匀  https://blog.csdn.net/kk55guang2/article/details/78475414
  * @note     https://blog.csdn.net/kk55guang2/article/details/78490069
  * @note     https://wenku.baidu.com/view/84e5eb271a37f111f0855b2d.html
  *
  * @see      GetOSTU(Image_Use);//大津法阈�?
  *
  * @date     2019/6/25 星期�?
  */ 
short GetOSTU(unsigned char tmImage[LCDH][LCDW]);


/*!
  * @brief    摄像头测试例�?
  *
  * @param
  *
  * @return
  *
  * @note     测试MT9V034  注意需要使�?  带与非门版（白色）转接座
  *
  * @example
  *
  * @date     2019/10/22 星期�?
  */
void Test_CAMERA(void);


/*!
  * @brief    基于soble边沿检测算子的一种边沿�?��?
  *
  * @param    imageIn    输入数组
  *           imageOut   输出数组      保存的二值化后的边沿信息
  *           Threshold  阈�?
  *
  * @return
  *
  * @note
  *
  * @example
  *
  * @date     2020/5/15
  */
void lq_sobel(unsigned char imageIn[LCDH][LCDW], unsigned char imageOut[LCDH][LCDW], unsigned char Threshold);


/*!
  * @brief    基于soble边沿检测算子的一种自动阈值边沿�?��?
  *
  * @param    imageIn    输入数组
  *           imageOut   输出数组      保存的二值化后的边沿信息
  *
  * @return
  *
  * @note
  *
  * @example
  *
  * @date     2020/5/15
  */
void lq_sobelAutoThreshold(unsigned char imageIn[LCDH][LCDW], unsigned char imageOut[LCDH][LCDW]);
void Seek_Road(void);
void Bin_Image_Filter(void);
#endif



