#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <Wire.h>
#include <JY901.h>
#include <string.h>
#include <Encoder.h>
#include <OneButton.h>
#include "cz.h"
#include "dog.h"
#include "SpineDetectorWebSocket.h"
#include "config.h"

#define KEY1 22  //1
#define KEY2 35 //2
#define KEY3 21 //3

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
uint32_t updateTime = 0; // time for next update
OneButton button1(KEY1, true);
OneButton button2(KEY2, true);
OneButton button3(KEY3, true);

// 创建WebSocket实例
SpineDetectorWebSocket webSocket;

int oldi = 0;
long i = 0;   //编码器计数值
int mainpage = 0, testpage = 0, userpage = 0, datapage = 0, standpage = 0, testpage1 = 0, testnum = 0;
int t = 0;
float jy901data = 0, testdata = 0, maxdata = 0;
float angle_data[5] = {0}; // 保存5个位置的角度数据

bool testInProgress = false; // 跟踪测试是否正在进行

void Encoder_Callback()//编码器采集中断
{
  i = Encoder.Get_Count();
}


void play(){
  if(t == 0 ){
    tft.fillScreen(TFT_WHITE);
  }
  if(testpage == 1 && t!=0){
    tft.pushImage(0,0,320,240,test_page);
  }else if(testpage == 2){
     tft.pushImage(0,0,320,240,user_page);
  }else if(testpage1 == 1){
    tft.pushImage(0,0,320,240,calibration);
  }else{
    if(mainpage == 0){
      tft.pushImage(0,0,320,240,main_page);
    }else if(mainpage == 1){
      tft.pushImage(0,0,320,240,main_page1);
    }else if(mainpage == 2){
      tft.pushImage(0,0,320,240,main_page2);
    }
  }
}
void click1()
{
  Serial.println("singleclick");
  testdata=(float)JY901.stcAngle.Angle[1]/32768*180;
  if(testpage1 == 1 && testdata < 5){
    // 校准开始
    webSocket.sendStatus(STATUS_CALIBRATION_STARTED);
    
    jy901data=testdata;
    tft.setCursor(25,140);
    tft.setTextSize(4);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    tft.print("Successful!");
    
    // 校准成功
    webSocket.sendStatus(STATUS_CALIBRATION_SUCCESS);
    webSocket.sendLog("info", "设备已校准完成");
  }else if(testpage1 == 1 && testdata > 5){
    // 校准失败
    webSocket.sendStatus(STATUS_CALIBRATION_FAILED);
    
    tft.setCursor(80,140);
    tft.setTextSize(4);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    tft.print("Failed!");
    
    // 发送失败日志
    webSocket.sendLog("error", "设备校准失败");
  }else if(testpage == 1 && testnum < 5){
    // 如果是第一个测量位置，发送测试开始事件
    if(testnum == 0 && !testInProgress) {
      webSocket.sendStatus(STATUS_TEST_STARTED);
      testInProgress = true;
      maxdata = 0; // 重置最大角度
    }
    
    testdata-=jy901data;
    if(testdata < 0){testdata=-testdata;}
    
    // 保存角度数据
    angle_data[testnum] = testdata;
    
    // 发送角度数据
    webSocket.sendAngleData(testnum + 1, testdata);
    
    tft.setTextSize(2);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    switch (testnum){
      case 0:tft.setCursor(180,100);tft.printf("angle1:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 1:tft.setCursor(180,120);tft.printf("angle2:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 2:tft.setCursor(180,140);tft.printf("angle3:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 3:tft.setCursor(180,160);tft.printf("angle4:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 4:tft.setCursor(180,180);tft.printf("angle5:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
    }
    testnum++;
    
    // 如果测试完成，发送结果
    if(testnum == 5) {
      webSocket.sendStatus(STATUS_TEST_COMPLETE);
      webSocket.sendTestComplete(maxdata);
      testInProgress = false;
    }
  }
  if(mainpage == 1 && testpage == 0){
      testpage = 1;
      play();
    }else if(mainpage == 2 && testpage1 == 0){
      testpage1 = 1;
      play();
    }
    
    
} 
void doubleclick1()
{
  Serial.println("doubleclick");
  testdata=(float)JY901.stcAngle.Angle[1]/32768*180;
  if(testpage1 == 1 && testdata < 5){
    // 校准开始
    webSocket.sendStatus(STATUS_CALIBRATION_STARTED);
    
    jy901data=testdata;
    tft.setCursor(25,140);
    tft.setTextSize(4);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    tft.print("Successful!");
    
    // 校准成功
    webSocket.sendStatus(STATUS_CALIBRATION_SUCCESS);
    webSocket.sendLog("info", "设备已校准完成");
  }else if(testpage1 == 1 && testdata > 5){
    // 校准失败
    webSocket.sendStatus(STATUS_CALIBRATION_FAILED);
    
    tft.setCursor(80,140);
    tft.setTextSize(4);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    tft.print("Failed!");
    
    // 发送失败日志
    webSocket.sendLog("error", "设备校准失败");
  }else if(testpage == 1 && testnum < 5){
    // 如果是第一个测量位置，发送测试开始事件
    if(testnum == 0 && !testInProgress) {
      webSocket.sendStatus(STATUS_TEST_STARTED);
      testInProgress = true;
      maxdata = 0; // 重置最大角度
    }
    
    testdata-=jy901data;
    if(testdata < 0){testdata=-testdata;}
    
    // 保存角度数据
    angle_data[testnum] = testdata;
    
    // 发送角度数据
    webSocket.sendAngleData(testnum + 1, testdata);
    
    tft.setTextSize(2);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    switch (testnum){
      case 0:tft.setCursor(180,100);tft.printf("angle1:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 1:tft.setCursor(180,120);tft.printf("angle2:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 2:tft.setCursor(180,140);tft.printf("angle3:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 3:tft.setCursor(180,160);tft.printf("angle4:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
      case 4:tft.setCursor(180,180);tft.printf("angle5:%.1f",testdata);if(testdata > maxdata){maxdata=testdata;}break;
    }
    testnum++;
    
    // 如果测试完成，发送结果
    if(testnum == 5) {
      webSocket.sendStatus(STATUS_TEST_COMPLETE);
      webSocket.sendTestComplete(maxdata);
      testInProgress = false;
    }
  }
}
void longclick1()
{
  Serial.println("longclick");
  delay(100);
  digitalWrite(19,0);
}
void click2()
{
  Serial.println("singleclick");
  if(testpage == 0 && testpage1 == 0){
      mainpage++;
      if(mainpage>2){
        mainpage=0;
      }
    }else if(mainpage == 0  && testpage1 == 0){
      testpage++;
    }else if(mainpage == 0 && testpage == 0){
      testpage1++;
    }else if(testpage == 1){
      if(testnum != 0){
        testpage++;
      }else{
        testpage=0;
      }
      
    }
    play();
} 
void doubleclick2()
{
  Serial.println("doubleclick");
}
void longclick2()
{
  Serial.println("longclick");
}
void click3()
{
  Serial.println("singleclick");
  testpage = 0;
  testpage1 = 0;
  mainpage = 0;
  testnum = 0;
  maxdata = 0;
  play();
} 
void doubleclick3()
{
  Serial.println("doubleclick");
}
void longclick3()
{
  Serial.println("longclick");
}

void setup(void)
{
  delay(500);
  pinMode(19,OUTPUT);
  tft.init();//初始化
  tft.fillScreen(TFT_WHITE);//屏幕颜色
  tft.setRotation(1);     //设置屏幕方向，0为0°，1为90°，2为180°，3为270°
  tft.setSwapBytes(true);
  tft.invertDisplay(0);
  
  // tft.pushImage(0,0,320,240,main_page);
  // play();
  Serial.begin(9600);  
  Serial1.begin(115200,SERIAL_8N1,25,26);

  attachInterrupt(Encoder.Encoder_A, Encoder_Callback, CHANGE); //使能编码器采集中断，双沿触发


  pinMode(KEY1,INPUT_PULLUP);
  pinMode(KEY2,INPUT_PULLUP);
  pinMode(KEY3,INPUT_PULLUP);
  button1.attachClick(click1);//关联单击事件
  button1.attachDoubleClick(doubleclick1);//关联双击事件
  button1.attachLongPressStart(longclick1);//关联长按事件
  button2.attachClick(click2);
  button2.attachDoubleClick(doubleclick2);
  button2.attachLongPressStart(longclick2);
  button3.attachClick(click3);
  button3.attachDoubleClick(doubleclick3);
  button3.attachLongPressStart(longclick3);
  
  // 初始化WebSocket服务器
  if (webSocket.begin(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("WebSocket服务器初始化成功");
  } else {
    Serial.println("WebSocket服务器初始化失败");
  }
}
 
void loop()
{
  // tft.fillRect(0, 0, 50, 50, TFT_RED);    //画实心矩形
  // tft.drawFastHLine(0, 20, 320, TFT_RED);//画线
  // tft.fillRect(0, 20, 320, 2, TFT_RED);
  // tft.pushImage(-5,21,200,231,gImage_demo_image3);
  button1.tick();//按键扫描
  button2.tick();
  button3.tick();
  
  // 处理WebSocket事件
  webSocket.loop();

  if(digitalRead(KEY1) == 0 && t == 0){
    digitalWrite(19,1);
    while(digitalRead(KEY1) == 0){tft.pushImage(0,0,320,240,main_page);}
    t++;
  }
  
  // Serial.print("Angle:");Serial.print((float)JY901.stcAngle.Angle[0]/32768*180);Serial.print(" ");Serial.print((float)JY901.stcAngle.Angle[1]/32768*180);Serial.print(" ");Serial.println((float)JY901.stcAngle.Angle[2]/32768*180);
  // Serial.println(i);
  int sensorValue=analogRead(34);
   //将模拟读数（从 0 - 1023）转换为电压（0 - 5V）：
   float voltage=sensorValue * (6.6/4096.0);
   //打印出您读取的值：
   Serial.println(voltage);
  delay(10);
  while (Serial1.available()) 
  {
    JY901.CopeSerialData(Serial1.read()); //Call JY901 data cope function
  }

  if(testpage1 == 4){
    tft.setCursor(150,115);
    tft.setTextSize(3);//1~7
    tft.setTextColor(TFT_BLACK,0xef5d);
    tft.print((float)JY901.stcAngle.Angle[1]/32768*180);
  }
  if(testpage == 2){
    tft.setCursor(210,67);
    tft.setTextSize(3);//1~7
    tft.setTextColor(TFT_RED,0xef5d);
    tft.print(maxdata);
  }
  if(t == 0){
    tft.setCursor(100,100);
    tft.setTextSize(5);//1~7
    tft.setTextColor(TFT_BLACK,TFT_WHITE);
    tft.print((voltage-3.5)/0.5*100);
    delay(1000);
  }
}

