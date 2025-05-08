#ifndef ENCODER_h
#define ENCODER_h

class CEncoder 
{
    public:
        CEncoder();
        long Get_Count();
        void Count_Clear();
        int Encoder_A = 17;//编码器A相引脚，可自行修改
        int Encoder_B = 16;//编码器B相引脚，可自行修改
    private:
        short Encoder_A_V;
        short Encoder_B_V;
        short flag = 0;
        short CW_1 = 0;
        short CW_2 = 0;
        long Encoder_Count = 0;
};
extern CEncoder Encoder;//用于外部调用的编码器名称
#endif