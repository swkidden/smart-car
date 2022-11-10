// �߽����
#include "balingyu.h"
// imageΪ��ֵ����ͼ��image_borderΪ�����õ�ͼ��64*94
/************************************************************
���������ơ�Eight_neighborhood
����    �ܡ��������㷨��߽�
����    ����Ѱ���ӷ�ʽ
���� �� ֵ����
��ʵ    ����Eight_neighborhood(0);
��ע�����
        1*���㷨��Ҫ�㴫��Ķ�ֵ�����飬�Ǿ�����ֵ��֮������飬��ɫΪ1  ��ɫΪ0
        2*���㷨�����ͼ����Ҫ���Լ�����һ�����ֵ������ߴ���ͬ�Ķ�ά����
        3*����ĺ궨����뻻���Լ�ʵ�ʵ�ͼ������ߴ�Ͷ�ά������
        4*�ǵ������ú���
************************************************************/

//�������������ͱ�������Ҫ�Ŀ��������߸ĳ����Լ�����������		��:  u8   uint8_t   unsigned char
// typedef unsigned char uint8;
// typedef char int8; 
// typedef unsigned int uint32;

//���ﻻ�����ͼ��ߴ�
#define USER_SIZE_H 60
#define USER_SIZE_W 94

//���ﻻ�����ֵ��֮���ͼ������
#define User_Image	Bin_Image
//�����ǰ��������������Ҫ�Լ�����һ����ԭͼ����ͬ�ߴ�Ķ�ά����
#define Edge_arr	Edge_arr

void Eight_neighborhood(uint8 flag) {
  uint8 i, j;
  //������������  ��ʼ�����ʼΪ6
  uint8 core_x, core_y, start;
  //����
  char direction;
  uint32 length = 0;
  //��ձ߽�����
  for (i = 0; i < USER_SIZE_H; i++)
    for (j = 0; j < USER_SIZE_W; j++) Edge_arr[i][j] = 0;
  if (flag == 0)  //��������������
  {
    start = 6;
    //����м�Ϊ��
    if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 1) {
      for (i = USER_SIZE_W / 2; i >= 0; i--) {
        if (User_Image[USER_SIZE_H - 1][i - 1] == 0 || i == 0) {  //�����µ�һ���߽����Ϊ����
          core_x = i;
          core_y = USER_SIZE_H - 1;
          break;
        }
      }
    }  //����м�Ϊ����ȥ������
    else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 0) {
      if (User_Image[USER_SIZE_H - 1][5] == 1)
        for (i = 5; i >= 0; i--) {
          if (User_Image[USER_SIZE_H - 1][i - 1] == 0 || i == 0) {  //�����µ�һ���߽����Ϊ����
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W - 5] == 1)
        for (i = USER_SIZE_W - 5; i >= 0; i--) {
          if (User_Image[USER_SIZE_H - 1][i - 1] == 0 || i == 0) {  //�����µ�һ���߽����Ϊ����
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else  //������Ϊ��Чͼ��������
        return;
    }
  } else if (flag == 1) {
    start = 6;
    for (i = 0; i < USER_SIZE_W; i++) {
      if (User_Image[USER_SIZE_H - 1][i] == 1 || i == USER_SIZE_W - 1) {  //�����µ�һ���߽����Ϊ����
        core_x = i;
        core_y = USER_SIZE_H - 1;
        break;
      }
    }
  } else if (flag == 2) {
    start = 2;
    //����м�Ϊ��
    if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 1) {
      for (i = USER_SIZE_W / 2; i < USER_SIZE_W; i++) {
        if (User_Image[USER_SIZE_H - 1][i + 1] == 0 || i == USER_SIZE_W - 1) {
          //�����µ�һ���߽����Ϊ����
          core_x = i;
          core_y = USER_SIZE_H - 1;
          break;
        }
      }
    }  //����м�Ϊ����ȥ������
    else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 0) {
      if (User_Image[USER_SIZE_H - 1][5] == 1)
        for (i = 5; i < USER_SIZE_W; i++) {
          if (User_Image[USER_SIZE_H - 1][i + 1] == 0 || i == USER_SIZE_W - 1) {  //�����µ�һ���߽����Ϊ����
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W - 5] == 1)
        for (i = USER_SIZE_W - 5; i < USER_SIZE_W; i++) {
          if (User_Image[USER_SIZE_H - 1][i + 1] == 0 || i == USER_SIZE_W - 1) {  //�����µ�һ���߽����Ϊ����
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else  //������Ϊ��Чͼ��������
        return;
    }
  } else if (flag == 3) {
    start = 2;
    for (i = USER_SIZE_W - 1; i >= 0; i--) {
      if (User_Image[USER_SIZE_H - 1][i] == 1 || i == 0) {  //�����µ�һ���߽����Ϊ����
        core_x = i;
        core_y = USER_SIZE_H - 1;
        break;
      }
    }
  }
  //Ѱ�ұ�Ե
  while (1) {
    direction = start;
    Edge_arr[core_y][core_x] = 1;
    /* ��ӵı߽���벿�� */
   // if(abs(core_x-USER_SIZE_W/2)<Image_Data[core_y][])
     /* ��ӵı߽���벿�� */
    if (flag == 0 || flag == 1) {
      for (i = 0; i < 8; i++) {
        if (direction == 0) {
          if (core_y != 0) {
            if (User_Image[core_y - 1][core_x] == 1) {
              core_y--;
              start = 6;
              break;
            }
          }
        }
        if (direction == 1) {
          if (core_y != 0 && core_x != USER_SIZE_W - 1) {
            if (User_Image[core_y - 1][core_x + 1] == 1) {
              core_y--;
              core_x++;
              start = 6;
              break;
            }
          }
        }
        if (direction == 2) {
          if (core_x != USER_SIZE_W - 1) {
            if (User_Image[core_y][core_x + 1] == 1) {
              core_x++;
              start = 0;
              break;
            }
          }
        }
        if (direction == 3) {
          if (core_y != USER_SIZE_H - 1 && core_x != USER_SIZE_W - 1) {
            if (User_Image[core_y + 1][core_x + 1] == 1) {
              core_y++;
              core_x++;
              start = 0;
              break;
            }
          }
        }
        if (direction == 4) {
          if (core_y != USER_SIZE_H - 1) {
            if (User_Image[core_y + 1][core_x] == 1) {
              core_y++;
              start = 2;
              break;
            }
          }
        }
        if (direction == 5) {
          if (core_y != USER_SIZE_H - 1 && core_x != 0) {
            if (User_Image[core_y + 1][core_x - 1] == 1) {
              core_y++;
              core_x--;
              start = 2;
              break;
            }
          }
        }
        if (direction == 6) {
          if (core_x != 0) {
            if (User_Image[core_y][core_x - 1] == 1) {
              core_x--;
              start = 4;
              break;
            }
          }
        }
        if (direction == 7) {
          if (core_y != 0 && core_x != 0) {
            if (User_Image[core_y - 1][core_x - 1] == 1) {
              core_y--;
              core_x--;
              start = 4;
              break;
            }
          }
        }
        direction++;
        if (direction > 7) direction = 0;
      }
    } else if (flag == 2 || flag == 3) {
      for (i = 0; i < 8; i++) {
        if (direction == 0) {
          if (core_y != 0) {
            if (User_Image[core_y - 1][core_x] == 1) {
              core_y--;
              start = 2;
              break;
            }
          }
        }
        if (direction == 1) {
          if (core_y != 0 && core_x != USER_SIZE_W - 1) {
            if (User_Image[core_y - 1][core_x + 1] == 1) {
              core_y--;
              core_x++;
              start = 4;
              break;
            }
          }
        }
        if (direction == 2) {
          if (core_x != USER_SIZE_W - 1) {
            if (User_Image[core_y][core_x + 1] == 1) {
              core_x++;
              start = 4;
              break;
            }
          }
        }
        if (direction == 3) {
          if (core_y != USER_SIZE_H - 1 && core_x != USER_SIZE_W - 1) {
            if (User_Image[core_y + 1][core_x + 1] == 1) {
              core_y++;
              core_x++;
              start = 6;
              break;
            }
          }
        }
        if (direction == 4) {
          if (core_y != USER_SIZE_H - 1) {
            if (User_Image[core_y + 1][core_x] == 1) {
              core_y++;
              start = 6;
              break;
            }
          }
        }
        if (direction == 5) {
          if (core_y != USER_SIZE_H - 1 && core_x != 0) {
            if (User_Image[core_y + 1][core_x - 1] == 1) {
              core_y++;
              core_x--;
              start = 0;
              break;
            }
          }
        }
        if (direction == 6) {
          if (core_x != 0) {
            if (User_Image[core_y][core_x - 1] == 1) {
              core_x--;
              start = 0;
              break;
            }
          }
        }
        if (direction == 7) {
          if (core_y != 0 && core_x != 0) {
            if (User_Image[core_y - 1][core_x - 1] == 1) {
              core_y--;
              core_x--;
              start = 2;
              break;
            }
          }
        }
        direction--;
        if (direction == -1) direction = 7;
      }
    }
    if (core_y ==  USER_SIZE_H - 1 && length > 5) {
      Edge_arr[core_y][core_x] = 1;
      break;
    }
    length++;
  }
  if (flag == 0 && length < 80) {
    Eight_neighborhood(1);
  }
  if (flag == 1 && length < 80) {
    Eight_neighborhood(2);
  }
  if (flag == 2 && length < 80) {
    Eight_neighborhood(3);
  }
}
