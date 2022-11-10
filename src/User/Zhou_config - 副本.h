/*
 * @LastEditors: Dark white
 * @LastEditTime: 2022-07-16 09:37:41
 * @FilePath: \LQ_TC264DA_CameraC1224\src\User\Zhou_config.h
 * @Description: 便于配置参数故把各参数集中于此处 方便调试
 *
 * Copyright (c) 2022 by Dark white, All Rights Reserved.
 */
/********************************************* 元素判断流程图******************************** */
/*
正常循迹：自动对控制行单边丢线的情况自动补线
这里用了先补线再识别的方法 既提高了识别率也防止了意外的跑出
R_0(正常循迹)1---不丢线------>R_1(正常循迹)2---拐点--->R_2(左边补线)进弯---->圆环后续处理
                                    \
                                    \--里程与弯道判断-->T-->出T
                                    \
(正常循迹)-------------------------->\--(双丢)--->Y

                由流程图得T字的部分时间处于圆环速度控制
*/
/********************************************* 元素判断流程图******************************** */

#ifndef _ZHOU_CONFIG_H_
#define _ZHOU_CONFIG_H_

#define ROAD_START_ROW 59 /**  @brief    使用起始行  */
#define ROAD_END_ROW 5    /**  @brief    使用结束行  */

/* ****************************通用速度决策******************************************* */
#define Bang_arry 200             //棒棒控制阈值
//#define Decoefficient 0       //可以是小数根据打角的电机减速系数 duty/8* Decoefficient 

#define initspeed 150         //初始速度 单位cm/s
#define jiasu_judge  g_ucFlagRoundabout == NO  && g_ucFlagZebra == 0 && g_ucFlagT == 0&&RoadIsStraight(ImageSide) && straightline >= 40
#define jiasu_speed 200       //直道加速
/* 防止跑出 */
#define despeed 0             //电机根据直道长度减速的系数可以是小数
#define despeed_threshold 0  //直道长度少于此值才开始减速
/* 防止跑出 */

/* 阿克曼 */
#define AKMQZ_xishu  1//阿克曼前瞻系数
#define ZHOUCHANG 15//15cm轴长
#define Diffspeed 0.1           //差速系数
/* 阿克曼 */
/* ****************************通用速度决策******************************************* */

/* ****************************元素速度决策******************************************* */
/* 元素速度同样受到通用速度决策的影响 */
#define Round_speed_S 150  //环岛处于直道的速度
#define Round_speed_D 150  //环岛进出的速度
#define Round_speed_W 150  //环岛处于弯道的速度
#define Y_speed 150        //进Y和出Y速度决策
#define T_speed_C 150      //出T速度决策
#define T_speed_Z 150      // T中速度决策
#define wandao_speed 150   //弯道速度决策
/* ****************************元素速度决策******************************************* */

/************************速度环pid ************************************/
#define MOTOR_P 100
#define MOTOR_I 0
#define MOTOR_D 0
#define MOTOR_T 
/**********************速度环pid ************************************/

/************************舵机pid ************************************/
#define Servo_P_default 15  //前方直道舵机pd
#define Servo_D_default 0
#define Servo_P_2 17 //小弯道
#define Servo_D_2 10
#define Servo_P_wan 18  //前方弯道舵机pd
#define Servo_D_wan 10

#define Servo_P_jinhuan_L 18 //进环
#define Servo_D_jinhuan_L 0
#define Servo_P_jinhuan_R 18 //进环
#define Servo_D_jinhuan_R 0
#define Servo_P_chuhuan_L 18 //出环
#define Servo_D_chuhuan_L 10
#define Servo_P_chuhuan_R 18 //出环
#define Servo_D_chuhuan_R 10

#define Servo_P_chut 15 //出T
#define Servo_D_chut 0
#define Servo_P_Y 10 //进出Y
#define Servo_D_Y 0
/************************舵机pid ************************************/

/************************补线行越小预判越强 ************************************/
#define Round_ROW 35      //环岛进入和环岛出环直行补线
#define Round_ROW_MID 35  //环中
#define Round_ROW_JIN 35  //进环
#define Round_ROW_CHU 35  //出环 
#define Y_ROW 35          // y字
#define T_ROW 45          // T字
#define CURVE_ROW 35      //弯道
#define ROAD_MAIN_ROW 35  /**  @brief    主跑行  */
/************************补线行 ************************************/

/************************补线宽度越小打角越狠 ************************************/
/* ***********************正常情况是控制行的赛道宽度约为47 ***********************/
#define Round_WIDTH 47      //环岛进入和环岛出环直行补线
#define Round_WIDTH_MID 47  //环中
#define Round_WIDTH_JIN 47  //进环
#define Round_WIDTH_CHU 47  //出环
#define Y_WIDTH 47          // y字
#define T_WIDTH 47          // T字
#define CURVE_WIDTH 47      //弯道
#define ROAD_MAIN_WIDTH 47  /**  @brief    主跑行  */
/************************补线宽度越小打角越狠 ************************************/

/* ********************************元素判断分割线************************************************ */
/* 环岛单边丢线判断 */
#define R_Round_0 Aver_right_lost > 30 && straightline > 40 && num_left_lost == 0 && num_right_lost > 15 && num_left_turn == 0 && num_up_turn <= 1 && num_right_turn >= 1
#define L_Round_0 Aver_left_lost > 30 && straightline > 40 && num_right_lost == 0 && num_left_lost > 15 && num_right_turn == 0 && num_up_turn <= 1 && num_left_turn >= 1
/* 环岛单边丢线判断 */

/* 环岛中间不丢判断 */
#define R_Round_1 num_left_lost == 0 && num_right_lost < 5 && num_left_turn <= 1
#define L_Round_1 num_right_lost == 0 && num_left_lost < 5 && num_right_turn <= 1
/* 环岛中间不丢判断 */

/* 环岛进入*/
#define R_Round_2 g_ucFlagW == 0 && num_left_lost == 0 && num_right_lost > 5 && num_left_turn == 0 && num_up_turn >= 1 && num_up_turn <= 3 && num_right_turn >= 1 && num_right_turn <= 3
#define L_Round_2 g_ucFlagW == 0 && num_right_lost == 0 && num_left_lost > 5 && num_right_turn == 0 && num_up_turn >= 1 && num_up_turn <= 3 && num_left_turn >= 1 && num_left_turn <= 3
/* 环岛进入*/

/* 环岛进入成功*/
#define R_Round_3 RoadUpSide_Mono(24, 70, UpdowmSide) && num_left_lost <= 2 && num_left_turn == 0 && num_up_turn <= 1
#define L_Round_3 RoadUpSide_Mono(24, 70, UpdowmSide) && num_right_lost <= 2 && num_right_turn == 0 && num_up_turn <= 1
/* 环岛进入成功*/

/* 出环判断 */
#define R_Round_4 RAllPulse >= 1500 /* && RoadUpSide_Mono(24, 70, UpdowmSide) */ && num_left_lost > 10 && num_right_lost > 10
#define L_Round_4 RAllPulse >= 1500 /* && RoadUpSide_Mono(24, 70, UpdowmSide) */ && num_left_lost > 10 && num_right_lost > 10
/* 出环判断 */

/* 出环到环中成功判断  */
#define R_Round_5 RAllPulse >= 10 && straightline > 45 /* && num_left_lost <= 5 */ && num_left_turn == 0 /* && !RoadUpSide_Mono(24, 70, UpdowmSide) */
#define L_Round_5 RAllPulse >= 10 && straightline > 45 /* && num_right_lost <= 5  */&& num_right_turn == 0 /* && !RoadUpSide_Mono(24, 70, UpdowmSide) */
/* 出环到环中成功判断  */

/* 出环到环外最终成功判断  */
#define R_Round_6 num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn == 0
#define L_Round_6 num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn == 0
/* 出环到环外最终成功判断  */

/* Y字判断 */
#define Y_0  g_ucFlagW==0&&UpdowmSide[0][up_turn[0]]<30 && UpdowmSide[0][up_turn[0]]> 5 && up_turn[0] > 30 && up_turn[0] < 64 && straightline<50 && num_left_turn != 0 && num_right_turn != 0&&(num_right_lost+num_left_lost)>20
/* Y字判断 */

/* 进Y字成功判断 */
#define Y_1 RAllPulse > 100 && num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn <= 1
/* 进Y字成功判断 */

/* 出Y字判断 */
#define Y_2 RAllPulse > 300 && UpdowmSide[0][up_turn[0]]<30 && UpdowmSide[0][up_turn[0]]> 5 && up_turn[0] > 20 && up_turn[0] < 74 && straightline<50 && num_left_turn != 0 && num_right_turn != 0&&(num_right_lost+num_left_lost)>15
/* 出Y字判断 */

/* 出Y字成功判断 */
#define Y_3 RAllPulse > 100 && num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn <= 1
/* 出Y字成功判断 */

/* t字进入判断前提能进入环岛不丢线阶段 */
#define L_T_1 RAllPulse > 5000 && g_ucFlagW == 2
#define R_T_1 RAllPulse > 5000 && g_ucFlagW == 1
/* t字进入判断 */

/* 出t字判断 */
#define T_2 num_left_lost > 10 && num_right_lost > 10&&g_ucFlagW==0
/* 出t字判断 */

/* 成功出t字判断 */
#define R_T_2 RAllPulse > 200 && num_right_lost <= 1 && num_left_lost <= 1 && num_up_turn <= 1
#define L_T_2 RAllPulse > 200 && num_right_lost <= 1 && num_left_lost <= 1 && num_up_turn <= 1
/* 成功出t字判断 */
#endif