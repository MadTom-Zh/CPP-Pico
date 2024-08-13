#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cmath>

#include "pico/util/queue.h"
#include "pico/stdlib.h"

#include <vector>

using namespace std;

// #define xxxx
// #define xxxx
// #define xxxx
// #define xxxx

namespace MadTomDev
{
    namespace Motor
    {
        class StepperMotorControl
        {
        private:
            void checkLooseAndThrow();
            //uint32_t getTotalHeap(void);
            //uint32_t getFreeHeap(void);
            static const int _tickSubdivsLength = 64;

        public:
            const double MATH_PI = 3.141592653589793;

            typedef struct
            {
                int subdivs[_tickSubdivsLength];
                int subdivsLength = _tickSubdivsLength;
                int repeatTimes = 1;
            } TickPackage;

        protected:
            int _id;
            int _timerInterval;

        private:

            TickPackage _pkgStandard;     // 用于细分时钟间距，精确到1/n个间隔；
            int _tickSubdiv;

            int _flexSqsStepCount = 0;
            vector<TickPackage> _flexSqsStart;
            vector<TickPackage> _flexSqsEnd;

            const string helpString =
                "Stepper Motor Control  by MadTom 2023 1221"
                "Update at 2024 0202"

                "Steps to use:"
                ""
                "1, Init:"
                "StepperMotorControl() //设定电机的3个控制线号；"
                "GetPin###() //获取信号线序号；"
                "Set/GetStepsPerRound() //设定获取每圈步数；"
                ""
                "2, Turn, constant speed"
                "Set/GetSpeed###() //设定读取恒定转速；"
                "Turn###() //恒速转动；"
                ""
                "3, Turn, softly"
                "Set/GetFlex###() //设定缓速转动参数；"
                "FlexTurn###() //缓速转动；"
                ""
                "4, Other operations"
                "ResetPosition() //转动到原点位置；"
                "Brake() //急停；"
                "Loose() //释放；"
                ""
                "5, Testing"
                "SuddenReverse() //双向急速反转；"
                ""
                "6, Position info:"
                "GetStepCount() //当前步数；"
                "ResetCounter() //重置计数器，将当前位置设为原点；"
                "GetTurnCount() //总圈数；"
                "GetAngleCount(bool isTotal) //当前总角度，当前角度；";

            // 变量
            string _moterName;
            int _pinPUL;     // 接线脉冲，需要5V，但客服说3.3v也能用？；
            int _pinDIR;     // 接线方向(需先于脉冲5us变动)
            int _pinENA;     // 接线使能(高电平时停用，释放电机)

            int _stepsPerRound; // 每圈步数；

            double _topSpeedSPS= 0; // 用户设定的恒定速度，steps per second；

            double _flexAccelerationSPSS = 0; // 缓动加速度；
            double _flexJerkSPSSS = 0;

            bool _SetSPSSubdivs(double speedSPS, TickPackage & pkg);
            void _EnqueueSubdivs(vector<TickPackage> &sequence);

            void _GetSubdivs(TickPackage & pkg, int steps,vector<TickPackage> & result);
            void _GetSubdivs(vector<TickPackage> &sequence, int steps, bool backwards,vector<TickPackage> & result);            
            void _GetSubdivStep(vector<TickPackage> &sequence,int stepIndex, int & outDiv1, int & outDiv2);

            void _SetDirection(bool isClockwise);
            void _SetGPIO(int pin, bool onOrOff);            

        public:
            int GetID();
            /// @brief 即时控制，0-正常运行，1-转向正向，2-转向反向，3-急停，4-释放
            int dtControlCode = 0;
            TickPackage dtIndexTPkg = {{},0,0};
            TickPackage dtPCurrentTPkg = {{},0,0};
            int dtWaitCountdown = 0;
            queue_t queueTickPackage;
            bool _direction; // 转动方向，true(默认)-顺时针-低电平；false-逆时针-高电平；
            long stepCount = 0; // 当前步数；

            /// @brief 初始化一个电机控制实力
            /// @param pMoterName 电机名称
            /// @param pinPUL 脉冲信号线号
            /// @param pinDIR 方向信号线号
            /// @param pinENA 失能信号线号
            /// @param stepsPreRound 每圈的步进数
            /// @param tickSubdivsLength 步进细分，最大32，时钟较慢时可提升精度；
            void Init(int id, string pMoterName,
                                int pinPUL, int pinDIR, int pinENA,
                                int stepsPerRound);
            void DeInit();
            void operator=(StepperMotorControl & motorControl);

            void SetTimerInterval(int microSec);
            int DTGetSubdiv();

            void DTSetGPIO_PLU(bool onOrOff);
            void DTSetGPIO_DIR(bool onOrOff);
            void DTSetGPIO_ENA(bool onOrOff);

            // 变量
            string GetMoterName();
            int GetPinPUL();
            int GetPinDIR();
            int GetPinENA();

            // 设置接线
            // 三根线分别是 脉冲，方向(需先于脉冲5us变动)，使能(高电平时停用，释放电机)

            // 位置信息
            void ResetCounter();                  // 重置计数器，将当前位置设为原点；
            long GetStepCount();                  // 当前步数；
            double GetRoundCount();               // 总圈数；
            double GetAngleCount(bool isTotal);   // 当前总角度，当前角度；
            double GetRadiansCount(bool isTotal); // 当前总弧度，当前弧度；

            const string Help(); // 给出帮助信息；

            // 每圈步数
            // void SetStepsPerRound(int stepsPerRound);
            int GetStepsPerRound();

            // 获取推荐的恒定速度
            double GetRecommendSPS(double sps);
            double GetRecommendRPS(double rps);
            double GetRecommendRPM(double rpm);

            // 设定恒定速度
            bool SetSpeedSPS(double sps); // 设置恒定转速，每秒步数；
            bool SetSpeedRPS(double rps); // 设置恒定转速，每秒转数；
            bool SetSpeedRPM(double rpm); // 设置恒定转速，每分钟转数；
            double GetSpeedSPS();
            double GetSpeedRPS();
            double GetSpeedRPM();

            // void SetSpeedStepFrequence(double frequency); // 设置恒定速度，以频率为基准；
            // double GetSpeedStepFrequence();


            void SetFlexAccelerationSPSS(double spss); // 设定欢动加速度，每秒变动的步进数；
            void SetFlexAccelerationRPSS(double rpss); // 设定缓动加速度，每秒变动的每秒圈数；
            void SetFlexAccelerationRPMM(double rpmm); // 同上，(rounds per minute) per minute；
            double GetFlexAccelerationSPSS();

            void SetFlex2JerkSPSSS(double spsss);
            double GetFlex2JerkSPSSS();

            // 恒速转动
            bool TurnSteps(long steps, double rps, bool setAsDefaultSpeed = false);    // 转动一定步数，使用临时转速，可决定是否改变设定的恒速；
            void TurnSteps(long steps);                                                     // 转动一定步数，使用上次设定的速度；
            bool TurnRounds(float rounds, double rps, bool setAsDefaultSpeed = false); // 转动一定圈数（粗略），其他参数如上；
            void TurnRounds(float rounds);
            /// @brief 转动一定角度
            /// @param angels 角度，一周为360
            /// @param speedRPS 以此速度转动
            /// @param setAsDefaultSpeed 是否将此速度设定为默认速度
            bool TurnAngles(float angels, double rps, bool setAsDefaultSpeed = false); // 转动一定角度（粗略）；
            void TurnAngles(float angels);
            /// @brief 转动一定弧度
            /// @param radians 弧度，一周为 (2*Pi / 360) * (360)；半轴为 (2*Pi / 360) * (180)
            /// @param speedRPS
            /// @param setAsDefaultSpeed
            bool TurnRadians(float radians, double rps, bool setAsDefaultSpeed = false); // 转动一定弧度（粗略）；
            void TurnRadians(float radians);

            // 缓动转动(恒定加速度)
            void FlexTurnSteps(long steps);      // 缓动转动一定步数；
            void FlexTurnRounds(float rounds);   // 缓动转动一定圈数（粗略）；
            void FlexTurnAngles(float angels);   // 缓动转动一定角度（粗略）；
            void FlexTurnRadians(float radians); // 缓动转动一定角度（粗略）；

            // 缓动转动（渐变加速度）
            void Flex2TurnSteps(long steps);
            void Flex2TurnRounds(float rounds);
            void Flex2TurnAngles(float angels);
            void Flex2TurnRadians(float radians);


            // 其他
            void ResetPosition(bool isFullReset, bool flexTurn = true); // 电机转动到0位置，fullReset意味着按转动步数一直回到真原点，否则只在当前圈回到0角度；
            void Brake();                                               // 急停（可能错位？）；
            void Loose();                                               // 释放，电机停电，任由惯性转动停止；
            void Regain();                                              // 重新控制；

            // 速度转换
            double ConvertSpeed_RPS_toSPS(double rps);
            double ConvertSpeed_SPS_toRPS(double sps);
            double ConvertSpeed_RPM_toSPS(double rpm);
            double ConvertSpeed_SPS_toRPM(double sps);

            double ConvertQty_Angels_toRadians(double angels);
            double ConvertQty_Radians_toAngels(double radians);
            int ConvertQty_Angels_toSteps(double angels);
            double ConvertQty_Steps_toAngels(int steps);
            int ConvertQty_Radians_toSteps(double radians);
            double ConvertQty_Steps_toRadians(int steps);

            // 测试
            void SuddenReverse(int totalSteps, double rps, bool setAsDefaultSpeed = false); // 转动一定步数后，突然反向再转动一定步数，用于检查电机可承受的最大可变向速度（注意负载情况）；

            // 回馈
            // Start(long atSteps)   // 电机开始转动，事件发生时的电机位置；
            // Stopped(long atSteps) // 电机已经停止；

        };
    }
}