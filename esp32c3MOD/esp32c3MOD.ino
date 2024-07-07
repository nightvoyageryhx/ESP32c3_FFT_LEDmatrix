#include <LedControl.h>
#include <binary.h>
#include "arduinoFFT.h"

#define MIC 1//ADC,麦克风信号 max9814模块，out接A0
#define NUM_LEDS 64 //led数量
#define SAMPLES 64 //采样个数
#define SAMPLING_FREQ  10000 //采样频率
#define NUM_BANDS 8
#define AMPLITUDE  1000  //声音强度调整倍率（柱状高度倍率）
#define MATRIX_SIDE   8 //每个矩阵的边长
#define NOISE 1770 //噪声


int DIN = 12;//SPI模式，MOSI主设备输出，从设备输入
int CS =  10;//SPI模式，CS片选
int CLK = 9;//SPI模式，CLK时钟

//const uint16_t samples = NUM_LEDS / 4;//采样点数，需要是2的次幂
//const byte halfsamples = samples / 2;  //取采样的一半数量
int value;                             //存放读取到声音的adc值

double vReal[SAMPLES];                 //存放FFT数据必要的数组，实部
double vImag[SAMPLES];                 //存放FFT数据必要的数组 ，虚部

arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);//创建FFT对象
/*****************************************************/
int uValue;                 //存放fft增长的变量
int dValue;                 //存放fft减小的变量
int tValue;                 //存放顶点下降的变量
int toDown = 0;             //柱子下降的数据
uint8_t toDownSpeed = 3;    //柱子下降的速度
int pointDown = 0;          //顶端点下降的变量
uint8_t pointDownSpeed = 9; //顶端点下降的速度
/*****************单次循环setyp函数*********************/

int bandValues[] = {0,0,0,0,0,0,0,0}; //频带幅值
int oldBarHeights[] = {0,0,0,0,0,0,0,0};//上一次频带显示的幅值
unsigned int sampling_period_us; //采样周期
unsigned long starTime;//计时使用
int col[9] = {B11111111,B01111111,B00111111,B00011111,B00001111,B00000111,B00000011,B00000001,B00000000}; //行列的九种显示，每一位对应一个led
//int col[9]={255,127,63,31,15,7,3,1,0};
byte show[8];//8行高度数据合并
/*
*/

LedControl lc = LedControl(DIN, CLK, CS, 0);//新建一个类对象

void setup()
{  
  delay(100); 
  Serial.begin(9600);             //上电延迟
  Serial.println("Ready"); //响应结束发送准备好
  lc.shutdown(0, false);       //初始化时设置点阵为正常使用模式
  lc.setIntensity(0, 0);       //设置亮度值，范围0~15
  lc.clearDisplay(0);          //点阵清屏
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));//采样周期
  pinMode(MIC,INPUT);
}

void loop()
{
  for (int i = 0; i < NUM_BANDS; i++){
    bandValues[i] = 0; //频带高度置零
  }

  for (int i = 0; i < SAMPLES; i++) {
    starTime = micros();//获取系统时间
    vReal[i] = analogRead(MIC);//读取麦克风的电平数据
    vImag[i] = 0;//虚部置0
    // Serial.println(micros() - starTime);
    while ((micros() - starTime) < sampling_period_us) { /* chill */ }//空转延迟，micros()函数获取系统时间
    // Serial.println(vReal[i]);
  }

  //fft变换
  FFT.DCRemoval();//去除直流
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  

   for (int i = 0; i < (SAMPLES/2); i++){
    if (vReal[i] > NOISE) {
      // Serial.println(vReal[i]);
      // 去除低频杂音和一些高频尖叫
      if (i>=0    && i<=1   ) bandValues[0]   += (int)vReal[i];
      if (i>1    && i<=3   ) bandValues[1]   += (int)vReal[i];
      if (i>3    && i<=5  ) bandValues[2]   += (int)vReal[i];
      if (i>5   && i<=7  ) bandValues[3]   += (int)vReal[i];//1kHz
      if (i>7   && i<=12  ) bandValues[4]   += (int)vReal[i];
      if (i>12   && i<=17  ) bandValues[5]   += (int)vReal[i];
      if (i>17   && i<=22  ) bandValues[6]   += (int)vReal[i];
      if (i>22   && i<=27  ) bandValues[7]   += (int)vReal[i];
    }
  }

  for (byte band = 0; band < NUM_BANDS; band++) {
    // 根据倍率缩放条形图高度
    int barHeight = bandValues[band] / AMPLITUDE;
    if (barHeight > MATRIX_SIDE) barHeight = MATRIX_SIDE;
    // 旧的高度值和新的高度值平均一下
    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;
    Serial.println(barHeight);
    show[7-band] = 255>>(7-barHeight);

        //Serial.println(uValue);
      
        //delay(100);


        oldBarHeights[band] = barHeight;
    }

    printByte(show);
}

/*
  for(int j=0;j<8;j++)//row
  {
    for(int i=0;i<9;i++)//colunm
    {
     lc.setRow(0,j,col[i]);
     delay(500);
    }
  }

  */
 

void printByte(byte character [])
{
  int i = 0;
  for (i = 0; i < 8; i++)
  {
    //lc.setRow(0, i, character[i]);//设置点阵单行8个LED状态，垂直显示
    lc.setColumn(0,i, character[i]); //水平显示  
  }
}
