#pragma once

#include "StepperMotorControl.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

bool MadTomDev_Motor_DriveTimer_TimerTick(struct repeating_timer *t);

namespace MadTomDev
{
    namespace Motor
    {
        class DriveTimer
        {
        protected:
        

        private:
            struct repeating_timer timer;
            int _intervalMicroSec;

        public:

            bool _hadLoosen = false;
            bool isBusy = false;
            int syncMissingCount = 0;
        
            StepperMotorControl  motorList[8];
            int motorCountMax = 8;
            int motorLastIndex = -1;

            DriveTimer(int intervalMicroSec);
            ~DriveTimer();       

            int GetTimerIntervalMicroSec();
            double GetMaxFrequency();
            StepperMotorControl * AddMotor(string moterName,
                                         int pinPUL, int pinDIR, int pinENA,
                                         int stepsPerRound);
            bool RemoveMotor(int moterId);

            bool TimerStart();
            bool TimerStop();
            
            void ClearQueue(queue_t & motorTickQueue);

        };

    }
}