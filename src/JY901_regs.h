#ifndef JY901_REGS_H
#define JY901_REGS_H

// 命令寄存器地址
#define JY901_SAVE 			0x00
#define JY901_CALSW 		0x01
#define JY901_RSW 			0x02
#define JY901_RRATE			0x03
#define JY901_BAUD 			0x04
#define JY901_AXOFFSET	    0x05
#define JY901_AYOFFSET	    0x06
#define JY901_AZOFFSET	    0x07
#define JY901_GXOFFSET	    0x08
#define JY901_GYOFFSET	    0x09
#define JY901_GZOFFSET	    0x0a
#define JY901_HXOFFSET	    0x0b
#define JY901_HYOFFSET	    0x0c
#define JY901_HZOFFSET	    0x0d
#define JY901_D0MODE		0x0e
#define JY901_D1MODE		0x0f
#define JY901_D2MODE		0x10
#define JY901_D3MODE		0x11
#define JY901_D0PWMH		0x12
#define JY901_D1PWMH		0x13
#define JY901_D2PWMH		0x14
#define JY901_D3PWMH		0x15
#define JY901_D0PWMT		0x16
#define JY901_D1PWMT		0x17
#define JY901_D2PWMT		0x18
#define JY901_D3PWMT		0x19
#define JY901_IICADDR		0x1a
#define JY901_LEDOFF 		0x1b
#define JY901_GPSBAUD		0x1c
 
// 数据寄存器地址
#define JY901_YYMM			0x30
#define JY901_DDHH			0x31
#define JY901_MMSS			0x32
#define JY901_MS			0x33
#define JY901_AX			0x34
#define JY901_AY			0x35
#define JY901_AZ			0x36
#define JY901_GX_REG		0x37  // 重命名为JY901_GX_REG避免冲突
#define JY901_GY_REG		0x38  // 重命名为JY901_GY_REG避免冲突
#define JY901_GZ			0x39
#define JY901_HX			0x3a
#define JY901_HY			0x3b
#define JY901_HZ			0x3c			
#define JY901_ROLL			0x3d
#define JY901_PITCH			0x3e
#define JY901_YAW			0x3f
#define JY901_TEMP			0x40
#define JY901_D0STATUS		0x41
#define JY901_D1STATUS		0x42
#define JY901_D2STATUS		0x43
#define JY901_D3STATUS		0x44
#define JY901_PRESSUREL		0x45
#define JY901_PRESSUREH		0x46
#define JY901_HEIGHTL		0x47
#define JY901_HEIGHTH		0x48
#define JY901_LONL			0x49
#define JY901_LONH			0x4a
#define JY901_LATL			0x4b
#define JY901_LATH			0x4c
#define JY901_GPSHEIGHT     0x4d
#define JY901_GPSYAW        0x4e
#define JY901_GPSVL			0x4f
#define JY901_GPSVH			0x50

// DIO模式定义
#define JY901_DIO_MODE_AIN  0
#define JY901_DIO_MODE_DIN  1
#define JY901_DIO_MODE_DOH  2
#define JY901_DIO_MODE_DOL  3
#define JY901_DIO_MODE_DOPWM 4
#define JY901_DIO_MODE_GPS  5

#endif // JY901_REGS_H 