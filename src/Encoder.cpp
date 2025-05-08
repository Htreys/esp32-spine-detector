#include "Encoder.h"
#include "string.h"
#include "Arduino.h"

/**@brief   构析函数，初始化
-- @param   无
-- @auther  庄文标
-- @date    2024/3/26
**/
CEncoder :: CEncoder()
{
  pinMode(Encoder_A, INPUT); //输入上拉模式
  pinMode(Encoder_B, INPUT); //输入上拉模式
}

/**@brief   获取编码器脉冲数
-- @param   无
-- @return  long Encoder_Count 编码器脉冲数
-- @auther  庄文标
-- @date    2024/3/26
**/
long CEncoder :: Get_Count()
{
    Encoder_A_V = digitalRead(Encoder_A);
    Encoder_B_V = digitalRead(Encoder_B);
    if(flag == 0 && Encoder_A_V == 0)
    {
        CW_1 = Encoder_B_V;
        flag = 1;
    }
    if(flag && Encoder_A_V)
    {
        CW_2 = !Encoder_B_V;
        if (CW_1 && CW_2) 
        {
            Encoder_Count += 1;
        }
        if (CW_1 == 0 && CW_2 == 0) 
        {
            Encoder_Count -= 1;
        }
        flag = 0;
    }
    return Encoder_Count;
}

/**@brief   编码器计数值清零
-- @param   无
-- @auther  庄文标
-- @date    2024/3/26
**/
void CEncoder :: Count_Clear()
{
    Encoder_Count = 0;
}

CEncoder Encoder = CEncoder();
