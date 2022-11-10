/*
 * @LastEditors: Dark white
 * @LastEditTime: 2022-07-22 16:11:49
 * @FilePath: \jingsu\src\User\Zhou_config.h
 * @Description: �������ò����ʰѸ����������ڴ˴� �������
 *
 * Copyright (c) 2022 by Dark white, All Rights Reserved.
 */
/********************************************* Ԫ���ж�����ͼ******************************** */
/*
����ѭ�����Զ��Կ����е��߶��ߵ�����Զ�����
���������Ȳ�����ʶ��ķ��� �������ʶ����Ҳ��ֹ��������ܳ�
R_0(����ѭ��)1---������------>R_1(����ѭ��)2---�յ�--->R_2(��߲���)����---->Բ����������
                                    \
                                    \--���������ж�-->T-->��T
                                    \
(����ѭ��)-------------------------->\--(˫��)--->Y

                ������ͼ��T�ֵĲ���ʱ�䴦��Բ���ٶȿ���
*/
/********************************************* Ԫ���ж�����ͼ******************************** */

#ifndef _ZHOU_CONFIG_H_
#define _ZHOU_CONFIG_H_

#define ROAD_START_ROW 59 /**  @brief    ʹ����ʼ��  */
#define ROAD_END_ROW 5    /**  @brief    ʹ�ý�����  */

/* ****************************ͨ���ٶȾ���******************************************* */
#define Bang_arry 200  //����������ֵ
//#define Decoefficient 0       //������С�����ݴ�ǵĵ������ϵ�� duty/8* Decoefficient

#define initspeed 220  //��ʼ�ٶ� ��λcm/s
#define jiasu_judge abs(drv_err2 / 10) < 15&&abs(drv_err / 10) < 15 && g_ucFlagRoundabout == NO&& g_ucFlagZebra == 0 && g_ucFlagT == 0  /* && RoadIsStraight(ImageSide)  */ && straightline >= 40
#define jiasu_speed 330  //ֱ������
/* ��ֹ�ܳ� */
#define despeed 0            //�������ֱ�����ȼ��ٵ�ϵ��������С��
#define despeed_threshold 0  //ֱ���������ڴ�ֵ�ſ�ʼ����
/* ��ֹ�ܳ� */

/* ������ */
#define AKMQZ_xishu 1   //������ǰհϵ��
#define ZHOUCHANG 15    // 15cm�᳤
#define Diffspeed 0.15  //����ϵ��
/* ������ */
/* ****************************ͨ���ٶȾ���******************************************* */

/* ****************************Ԫ���ٶȾ���******************************************* */
/* Ԫ���ٶ�ͬ���ܵ�ͨ���ٶȾ��ߵ�Ӱ�� */
#define Round_speed_S 220  //��������ֱ�����ٶ�
#define Round_speed_D 220  //�����������ٶ�
#define Round_speed_W 220  //��������������ٶ�
#define Y_speed 180        //��Y�ͳ�Y�ٶȾ���
#define T_speed_C 140      //��T�ٶȾ���
#define T_speed_Z 180      // T���ٶȾ���
#define wandao_speed 180   //����ٶȾ���
#define chuku_speed 150    //�����ٶ�
#define jinku_speed 100    //�����ٶ�
/* ****************************Ԫ���ٶȾ���******************************************* */

/************************�ٶȻ�pid ************************************/
#define MOTOR_P 100
#define MOTOR_I 0
#define MOTOR_D 0
/**********************�ٶȻ�pid ************************************/

/************************���pid ************************************/
#define Servo_P_default 7  //ǰ��ֱ�����pd
#define Servo_D_default 0
#define Servo_P_2 25  //С���
#define Servo_D_2 5
#define Servo_P_wan 40  //ǰ��������pd
#define Servo_D_wan 15

#define Servo_P_jinhuan_L 20  //����
#define Servo_D_jinhuan_L 4
#define Servo_P_jinhuan_R 20  //����
#define Servo_D_jinhuan_R 4
#define Servo_P_chuhuan_L 20  //����
#define Servo_D_chuhuan_L 4
#define Servo_P_chuhuan_R 20  //����
#define Servo_D_chuhuan_R 4

#define Servo_P_chuhuanhou_R 15  //��������ʱ
#define Servo_D_chuhuanhou_R 2
#define Servo_P_chuhuanhou_L 15  //��������ʱ
#define Servo_D_chuhuanhou_L 2
#define Servo_P_chut 1000  //��T ����ǳ�ǰ���Ƹ���
#define Servo_D_chut 10

#define Servo_P_Y 20  //����Y
#define Servo_D_Y 4
// #define Servo_P_chuku 100  //����
// #define Servo_D_chuku 0
#define Servo_P_jinku 100  //����
#define Servo_D_jinku 0
/************************���pid ************************************/

/************************������ԽСԤ��Խǿ ************************************/
#define Round_ROW 35      //��������ͻ�������ֱ�в���
#define Round_ROW_MID 35  //����
#define Round_ROW_JIN 35  //����
#define Round_ROW_CHU 35  //����
#define Y_ROW 35          // y��
#define T_ROW 35          // T��
#define T_ROW_MID 35          // T��
#define CURVE_ROW 30      //���
#define ROAD_MAIN_ROW 35  /**  @brief    ������  */
//#define jinku_Row 55      //����
//#define ku_saomiao 30     //����ɨ����
//#define ku_buxian 20      //�ⲹ��
/************************������ ************************************/

/************************���߿��ԽС���Խ�� ************************************/
/* ***********************��������ǿ����е��������ԼΪ47 ***********************/
#define Round_WIDTH 47      //��������ͻ�������ֱ�в���
#define Round_WIDTH_MID 47  //����
#define Round_WIDTH_JIN 47  //����
#define Round_WIDTH_CHU 47  //����
#define Y_WIDTH 47          // y��
#define T_WIDTH 47          // T��
#define T_WIDTH_MId 47          // T��
#define CURVE_WIDTH 47      //���
#define ROAD_MAIN_WIDTH 47  /**  @brief    ������  */
#define jinku_Row_WIDTH 47  //���
#define ku_buxian_WIDTH 47
/************************���߿��ԽС���Խ�� ************************************/

/* ********************************Ԫ���жϷָ���************************************************ */
/* ͨ�ò��� */
// #define angle_arry 1  //�ǵ��жϷ�Χ
/* ͨ�ò��� */
/* �������߶����ж� */
#define R_Round_0 rightdown_y > 30 && rightdown_x > 40 && num_left_turn <= 1 && num_right_lost > 15&& num_right_turn >= 1
#define L_Round_0 leftdown_y > 30 && leftdown_x<50 && num_right_turn <= 1 && num_left_lost> 15&& num_left_turn >= 1
/* �������߶����ж� */

/* �����м䲻���ж� */
#define R_Round_1 num_left_lost == 0 && num_right_lost < 5 && num_left_turn <= 1
#define L_Round_1 num_right_lost == 0 && num_left_lost < 5 && num_right_turn <= 1
/* �����м䲻���ж� */

/* ��������*/
#define R_Round_2 g_ucFlagW == 0&&num_right_lost > 5 && num_right_turn >= 1 && num_up_turn > 0
#define L_Round_2 g_ucFlagW == 0&&num_left_lost > 5 && num_left_turn >= 1 && num_up_turn > 0
/* ��������*/

/* ��������ɹ�*/
#define R_Round_3 g_ucFlagW == 1 && num_up_turn <= 1 && RAllPulse >= 1500
#define L_Round_3 g_ucFlagW == 2 && num_up_turn <= 1 && RAllPulse >= 1500
/* ��������ɹ�*/

/* �����ж� */
#define R_Round_4 leftdown_turn_flag&& RAllPulse >= 500 && num_left_lost > 10 && num_right_lost > 10
#define L_Round_4 rightdown_turn_flag&& RAllPulse >= 500 && num_left_lost > 10 && num_right_lost > 10
/* �����ж� */

/* ���������гɹ��ж�  */
#define R_Round_5 RAllPulse >= 10 && straightline > 35 && num_left_lost == 0
#define L_Round_5 RAllPulse >= 10 && straightline > 35 && num_right_lost == 0
/* ���������гɹ��ж�  */

/* �������������ճɹ��ж�  */
#define R_Round_6 num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn == 0||RAllPulse>5000
#define L_Round_6 num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn == 0||RAllPulse>5000
/* �������������ճɹ��ж�  */

/* Y���ж� */
#define Y_0  g_ucFlagW == 0/* && UpdowmSide[0][up_turn[0]]<30 && UpdowmSide[0][up_turn[0]]> 5 && up_turn[0] > 20 && up_turn[0] < 74
     */
    #define Y_YIJING 0 //1��ʱ��y�������ж�
/* Y���ж� */

/* ��Y�ֳɹ��ж� */
#define Y_1 RAllPulse > 300 && num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn == 0&&num_left_turn<=1&&num_right_turn<=1
/* ��Y�ֳɹ��ж� */

/* ��Y���ж� */
#define Y_2 RAllPulse > 1000 && UpdowmSide[0][up_turn[0]]<30 && UpdowmSide[0][up_turn[0]]> 5 && up_turn[0] > 20 && up_turn[0] < 74 && straightline<50 && num_left_turn != 0 && num_right_turn != 0 && (num_right_lost + num_left_lost)> 20
/* ��Y���ж� */

/* ��Y�ֳɹ��ж� */
#define Y_3 RAllPulse > 300 && num_left_lost <= 1 && num_right_lost <= 1 && num_up_turn <= 1&&num_left_turn<=1&&num_right_turn<=1
/* ��Y�ֳɹ��ж� */

/* t�ֽ����ж�ǰ���ܽ��뻷�������߽׶� */
#define L_T_1 RAllPulse > 5000 && g_ucFlagW == 2
#define R_T_1 RAllPulse > 5000 && g_ucFlagW == 1
/* t�ֽ����ж� */

/* ��t���ж� */
#define T_2 num_left_lost >= 8 && num_right_lost >= 8 && num_left_turn&& num_right_turn&&((rightdown_y > 30 && rightdown_x > 40)||(leftdown_y > 30 && leftdown_x<50))
/* ��t���ж� */

/* �ɹ���t���ж� */
#define R_T_2 RAllPulse > 100  && num_right_lost <= 8 && num_left_lost <= 8  && num_up_turn <= 1&& num_left_turn<=1&&num_right_turn<=1||RAllPulse>2000
#define L_T_2 RAllPulse > 100  && num_right_lost <= 8 && num_left_lost <= 8  && num_up_turn <= 1&&num_right_turn<=1&& num_left_turn<=1||RAllPulse>2000
/* �ɹ���t���ж� */

/* ���� */
#define right_chuku RAllPulse > 2000
#define left_chuku RAllPulse > 2000
#define chukujiao 70
/* ���� */
/*  */
/* ���� */
// �궨���·���������������Զ�ѭ��
#define cheku_1 0
/* ���� */

/* ���ж� */
//#define cheku_flag num_black_white >= 5 && num_up_turn >= 5
/* ���� */
#define jinkujiao 70
/* ���� */
#endif
