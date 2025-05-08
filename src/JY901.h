// JY901.h
#ifndef JY901_h
#define JY901_h

#include "JY901_regs.h"
#include <Wire.h>
 
struct STime
{
	unsigned char ucYear;
	unsigned char ucMonth;
	unsigned char ucDay;
	unsigned char ucHour;
	unsigned char ucMinute;
	unsigned char ucSecond;
	unsigned short usMiliSecond;
};
struct SAcc
{
	short a[3];
	short T;
};
struct SGyro
{
	short w[3];
	short T;
};
struct SAngle
{
	short Angle[3];
	short T;
};
struct SMag
{
	short h[3];
	short T;
};
 
struct SDStatus
{
	short sDStatus[4];
};
 
struct SPress
{
	long lPressure;
	long lAltitude;
};
 
struct SLonLat
{
	long lLon;
	long lLat;
};
 
struct SGPSV
{
	short sGPSHeight;
	short sGPSYaw;
	long lGPSVelocity;
};
struct SQuater
{
	short q0;
	short q1;
	short q2;
	short q3;
};
struct SSN
{
	short sSVNum;
	short sPDOP;
	short sHDOP;
	short sVDOP;
};
class CJY901 
{
  public: 
	struct STime		stcTime;
	struct SAcc 		stcAcc;
	struct SGyro 		stcGyro;
	struct SAngle 		stcAngle;
	struct SMag 		stcMag;
	struct SDStatus 	stcDStatus;
	struct SPress 		stcPress;
	struct SLonLat 		stcLonLat;
	struct SGPSV 		stcGPSV;
	struct SQuater		stcQuater;
	struct SSN 		stcSN;
	
    CJY901 (); 
	void StartIIC();
	void StartIIC(unsigned char ucAddr);
    void CopeSerialData(unsigned char ucData);
	short ReadWord(unsigned char ucAddr);
	void WriteWord(unsigned char ucAddr,short sData);
	void ReadData(unsigned char ucAddr,unsigned char ucLength,char chrData[]);
	void GetTime();
	void GetAcc();
	void GetGyro();
	void GetAngle();
	void GetMag();
	void GetPress();
	void GetDStatus();
	void GetLonLat();
	void GetGPSV();
	
  private: 
	unsigned char ucDevAddr; 
	void readRegisters(unsigned char deviceAddr,unsigned char addressToRead, unsigned char bytesToRead, char * dest);
	void writeRegister(unsigned char deviceAddr,unsigned char addressToWrite,unsigned char bytesToRead, char *dataToWrite);
};
extern CJY901 JY901;
#endif