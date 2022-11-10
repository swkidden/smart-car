// 边界跟踪
#include "balingyu.h"
// image为二值化后图像，image_border为计算后得到图像64*94
/************************************************************
【函数名称】Eight_neighborhood
【功    能】八邻域算法求边界
【参    数】寻种子方式
【返 回 值】无
【实    例】Eight_neighborhood(0);
【注意事项】
        1*该算法需要你传入的二值化数组，是经过二值化之后的数组，白色为1  黑色为0
        2*该算法输出的图像需要你自己定义一个与二值化数组尺寸相同的二维数组
        3*下面的宏定义必须换成自己实际的图像数组尺寸和二维数组名
        4*记得声明该函数
************************************************************/

//这里是数据类型别名，需要的开启，或者改成你自己的数据类型		如:  u8   uint8_t   unsigned char
// typedef unsigned char uint8;
// typedef char int8; 
// typedef unsigned int uint32;

//这里换上你的图像尺寸
#define USER_SIZE_H 60
#define USER_SIZE_W 94

//这里换上你二值化之后的图像数组
#define User_Image	Bin_Image
//这里是八邻域输出数组需要自己定义一个与原图像相同尺寸的二维数组
#define Edge_arr	Edge_arr

void Eight_neighborhood(uint8 flag) {
  uint8 i, j;
  //核子中心坐标  起始方向初始为6
  uint8 core_x, core_y, start;
  //方向
  char direction;
  uint32 length = 0;
  //清空边界数据
  for (i = 0; i < USER_SIZE_H; i++)
    for (j = 0; j < USER_SIZE_W; j++) Edge_arr[i][j] = 0;
  if (flag == 0)  //从里向外找种子
  {
    start = 6;
    //如果中间为白
    if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 1) {
      for (i = USER_SIZE_W / 2; i >= 0; i--) {
        if (User_Image[USER_SIZE_H - 1][i - 1] == 0 || i == 0) {  //将左下第一个边界点作为种子
          core_x = i;
          core_y = USER_SIZE_H - 1;
          break;
        }
      }
    }  //如果中间为黑则去两边找
    else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 0) {
      if (User_Image[USER_SIZE_H - 1][5] == 1)
        for (i = 5; i >= 0; i--) {
          if (User_Image[USER_SIZE_H - 1][i - 1] == 0 || i == 0) {  //将左下第一个边界点作为种子
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W - 5] == 1)
        for (i = USER_SIZE_W - 5; i >= 0; i--) {
          if (User_Image[USER_SIZE_H - 1][i - 1] == 0 || i == 0) {  //将左下第一个边界点作为种子
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else  //否则将视为无效图像不做处理
        return;
    }
  } else if (flag == 1) {
    start = 6;
    for (i = 0; i < USER_SIZE_W; i++) {
      if (User_Image[USER_SIZE_H - 1][i] == 1 || i == USER_SIZE_W - 1) {  //将左下第一个边界点作为种子
        core_x = i;
        core_y = USER_SIZE_H - 1;
        break;
      }
    }
  } else if (flag == 2) {
    start = 2;
    //如果中间为白
    if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 1) {
      for (i = USER_SIZE_W / 2; i < USER_SIZE_W; i++) {
        if (User_Image[USER_SIZE_H - 1][i + 1] == 0 || i == USER_SIZE_W - 1) {
          //将右下第一个边界点作为种子
          core_x = i;
          core_y = USER_SIZE_H - 1;
          break;
        }
      }
    }  //如果中间为黑则去两边找
    else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W / 2] == 0) {
      if (User_Image[USER_SIZE_H - 1][5] == 1)
        for (i = 5; i < USER_SIZE_W; i++) {
          if (User_Image[USER_SIZE_H - 1][i + 1] == 0 || i == USER_SIZE_W - 1) {  //将右下第一个边界点作为种子
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else if (User_Image[USER_SIZE_H - 1][USER_SIZE_W - 5] == 1)
        for (i = USER_SIZE_W - 5; i < USER_SIZE_W; i++) {
          if (User_Image[USER_SIZE_H - 1][i + 1] == 0 || i == USER_SIZE_W - 1) {  //将右下第一个边界点作为种子
            core_x = i;
            core_y = USER_SIZE_H - 1;
            break;
          }
        }
      else  //否则将视为无效图像不做处理
        return;
    }
  } else if (flag == 3) {
    start = 2;
    for (i = USER_SIZE_W - 1; i >= 0; i--) {
      if (User_Image[USER_SIZE_H - 1][i] == 1 || i == 0) {  //将右下第一个边界点作为种子
        core_x = i;
        core_y = USER_SIZE_H - 1;
        break;
      }
    }
  }
  //寻找边缘
  while (1) {
    direction = start;
    Edge_arr[core_y][core_x] = 1;
    /* 添加的边界存入部分 */
   // if(abs(core_x-USER_SIZE_W/2)<Image_Data[core_y][])
     /* 添加的边界存入部分 */
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
