#include "DriveTimer.h"

using namespace MadTomDev::Motor;

DriveTimer ::DriveTimer(int intervalMicroSec)
{
    _intervalMicroSec = intervalMicroSec;
}
DriveTimer ::~DriveTimer()
{
    cancel_repeating_timer(&timer);
}


bool MadTomDev_Motor_DriveTimer_TimerTick(struct repeating_timer *t)
{
    if (t->user_data == NULL)
    {
        return false;
    }
    // printf("Timer Tick.");
    DriveTimer *dt = (DriveTimer *)t->user_data;
    if (dt->isBusy)
    {
        dt->syncMissingCount++;
        return false;
    }
    dt->isBusy = true;
    for (int i = 0, iv = dt->motorLastIndex; i <= iv; ++i)
    {
        StepperMotorControl & curMotor = dt->motorList[i];

        if (curMotor.dtWaitCountdown > 0)
        {
            curMotor.dtWaitCountdown--;
            continue;
        }

        switch (curMotor.dtControlCode)
        {
        case 0:
        {
            StepperMotorControl::TickPackage &idxerTPkg = curMotor.dtIndexTPkg;
            if (idxerTPkg.repeatTimes <= 0)
            {
                if (!queue_try_remove(&(curMotor.queueTickPackage), &(curMotor.dtPCurrentTPkg)))
                {
                    // don't have value
                    continue;
                }
                // a = a ?
                // curTPkg = ptrCurMotor->dtPCurrentTPkg;

                /*
                        printf("dequeue a1 %d, a2 %d\n", ptrCurTPkg->subdivs[0], ptrCurTPkg->subdivs[1]);
                        printf("length %d\n", ptrCurTPkg->subdivsLength);
                        printf("repeat %d\n", ptrCurTPkg->repeatTimes);*/
                idxerTPkg.subdivsLength = 0;
                idxerTPkg.repeatTimes = curMotor.dtPCurrentTPkg.repeatTimes;
/*
                // check sub divs
                StepperMotorControl::TickPackage testPkg = curMotor.dtPCurrentTPkg;
                printf("dequeue %d %d\n", testPkg.subdivsLength, testPkg.repeatTimes);
                for (int i = 0, iv = testPkg.subdivsLength; i < iv; ++i)
                {
                    printf("%d ", testPkg.subdivs[i]);
                }
                printf("\n");*/
            }
            StepperMotorControl::TickPackage &curTPkg = curMotor.dtPCurrentTPkg;

            // do switch
            if (idxerTPkg.subdivsLength % 2 == 0)
            {
                // gpio pulse 1
                curMotor.DTSetGPIO_PLU(true);
                if(curMotor._direction)
                {
                    curMotor.stepCount++;
                }
                else
                {
                    curMotor.stepCount--;
                }
            }
            else
            {
                // gpio pulse 0
                curMotor.DTSetGPIO_PLU(false);
            }

            curMotor.dtWaitCountdown = curTPkg.subdivs[idxerTPkg.subdivsLength++] - 1;

            if (idxerTPkg.subdivsLength >= curTPkg.subdivsLength)
            {
                idxerTPkg.subdivsLength = 0;
                --idxerTPkg.repeatTimes;
            }           

            break;
        }
        case 1: // 方向，正向
        {
            // set gpio 0
            curMotor.DTSetGPIO_DIR(false);

            // 按说明书，转动前5us设定信号；
            int itv = dt->GetTimerIntervalMicroSec();
            if (itv < 5)
            {
                curMotor.dtWaitCountdown = 5 / itv + 1;
            }
            else
            {
                curMotor.dtWaitCountdown = 1;
            }
            curMotor.dtControlCode = 0;
            break;
        }
        case 2: // 方向，反向
        {
            // set gpio 1
            curMotor.DTSetGPIO_DIR(true);

            int itv = dt->GetTimerIntervalMicroSec();
            if (itv < 5)
            {
                curMotor.dtWaitCountdown = 5 / itv + 1;
            }
            else
            {
                curMotor.dtWaitCountdown = 1;
            }
            curMotor.dtControlCode = 0;
            break;
        }
        case 3: // 急停
        {
            // 脉冲 低电平
            curMotor.DTSetGPIO_PLU(false);
            // 当前序列；

            curMotor.dtPCurrentTPkg.repeatTimes = 0;

            // 清空队列；
            dt->ClearQueue(curMotor.queueTickPackage);
            curMotor.dtIndexTPkg.repeatTimes = 0;

            curMotor.dtWaitCountdown = 0;
            curMotor.dtControlCode = 0;
            break;
        }
        case 4: // 释放（保持，恢复设0）
        {
            if(dt->_hadLoosen)
            {
                break;
            }
            // 释放 高电平
            curMotor.DTSetGPIO_ENA(true);
            // 脉冲 低电平
            curMotor.DTSetGPIO_PLU(false);

            curMotor.dtPCurrentTPkg.repeatTimes = 0;

            // 清空队列；
            dt->ClearQueue(curMotor.queueTickPackage);
            curMotor.dtIndexTPkg.repeatTimes = 0;
            dt->_hadLoosen = true;
            break;
        }
        case 5: // 解除释放
        {
            // 释放 低电平
            curMotor.DTSetGPIO_ENA(false);

            curMotor.dtWaitCountdown = curMotor.DTGetSubdiv();
            curMotor.dtControlCode = 0;
            dt->_hadLoosen = false;
            break;
        }
        default:
        {
            break;
        }
        }
    }

    dt->isBusy = false;
    return true;
}

int DriveTimer::GetTimerIntervalMicroSec()
{
    return _intervalMicroSec;
}
double DriveTimer::GetMaxFrequency()
{
    return (double)500000 / _intervalMicroSec;
}
StepperMotorControl * DriveTimer::AddMotor(string moterName,
                                        int pinPUL, int pinDIR, int pinENA,
                                        int stepsPerRound )
{
    if (motorLastIndex + 1 >= motorCountMax)
    {
        throw "Max capacity reached.";
    }
    int newMIdx = motorLastIndex + 1;
    motorList[newMIdx] .Init
    (
        newMIdx,moterName,pinPUL,pinDIR, pinENA, stepsPerRound
    );
    motorList[newMIdx].SetTimerInterval(_intervalMicroSec);
    motorLastIndex++;
    return &(motorList[newMIdx]);
}
bool DriveTimer::RemoveMotor(int moterId)
{
    int foundMotorIdx = -1;
    for (int i = motorLastIndex; i >= 0; --i)
    {
        if (motorList[i].GetID() == moterId)
        {
            foundMotorIdx = i;
            break;
        }
    }
    if (foundMotorIdx < 0)
    {
        return false;
    }

    motorList[foundMotorIdx].DeInit();
    --motorLastIndex;
    for (int i = foundMotorIdx; i <= motorLastIndex; ++i)
    {
        motorList[i] = motorList[i + 1];
    }

    return true;
}

bool MadTomDev_Motor_DriveTimer_MultiCoreEntry_keepAlive;
DriveTimer *MadTomDev_Motor_DriveTimer_MultiCoreEntry_data;
int MadTomDev_Motor_DriveTimer_MultiCoreEntry_itvUs;
repeating_timer *MadTomDev_Motor_DriveTimer_MultiCoreEntry_out;
void MadTomDev_Motor_DriveTimer_MultiCoreEntry()
{
    // 间隔时间如果为正数，则tick执行完之后再等待间隔时间；
    // 如果为负数，则下次tick开始时间和本次tick开始时间刚好为间隔时间；
    add_repeating_timer_us(
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_itvUs,
        MadTomDev_Motor_DriveTimer_TimerTick,
        // NULL,
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_data,
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_out);

    while (MadTomDev_Motor_DriveTimer_MultiCoreEntry_keepAlive)
    {
        // keep alive;
        tight_loop_contents();
    }
    cancel_repeating_timer(MadTomDev_Motor_DriveTimer_MultiCoreEntry_out);
}
bool DriveTimer::TimerStart()
{
    if (MadTomDev_Motor_DriveTimer_MultiCoreEntry_keepAlive)
    {
        return false;
    }
    else
    {
        // run on core 1
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_keepAlive = true;
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_itvUs = -_intervalMicroSec;
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_data = this;
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_out = &timer;
        multicore_launch_core1(MadTomDev_Motor_DriveTimer_MultiCoreEntry);
        return true;
    }
}
bool DriveTimer::TimerStop()
{
    if (MadTomDev_Motor_DriveTimer_MultiCoreEntry_keepAlive)
    {
        MadTomDev_Motor_DriveTimer_MultiCoreEntry_keepAlive = false;
        return true;
    }
    else
    {
        return false;
    }
}

void DriveTimer::ClearQueue(queue_t &motorTickQueue)
{
    StepperMotorControl::TickPackage curTPkg;
    while (queue_try_remove(&motorTickQueue, &curTPkg))
    {
        // remove all;
    }
}