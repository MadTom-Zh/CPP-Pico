#include "StepperMotorControl.h"
#include <malloc.h>

using namespace MadTomDev::Motor;

uint32_t getTotalHeap(void) {
   extern char __StackLimit, __bss_end__;
   
   return &__StackLimit  - &__bss_end__;
}
uint32_t getFreeHeap(void) {
   struct mallinfo m = mallinfo();

   return getTotalHeap() - m.uordblks;
}
uint32_t alarmHeapLeft = 130000;
bool checkHeapSufficient()
{
    return alarmHeapLeft <= getFreeHeap(); 
}
void throwHeapInsufficient()
{
    throw "Insufficient Heap.";
}

int StepperMotorControl::GetID()
{
    return _id;
}
void StepperMotorControl::Init(int id, string moterName,
                               int pinPUL, int pinDIR, int pinENA,
                               int stepsPerRound)
{
    _id = id;
    _moterName = moterName;
    _pinPUL = pinPUL;
    _pinDIR = pinDIR;
    _pinENA = pinENA;
    gpio_init(_pinPUL);
    gpio_set_dir(_pinPUL, GPIO_OUT);
    gpio_init(_pinDIR);
    gpio_set_dir(_pinDIR, GPIO_OUT);
    gpio_init(_pinENA);
    gpio_set_dir(_pinENA, GPIO_OUT);

    _stepsPerRound = stepsPerRound;
    // int slBy2 = _subdivsLength * 2;
    //_tickSubdivs = new int[slBy2];
    //_tickSubdivsFlex = new int[slBy2];

    queue_init(&queueTickPackage, sizeof(TickPackage), 32);
}
void StepperMotorControl::DeInit()
{
    gpio_deinit(_pinPUL);
    gpio_deinit(_pinDIR);
    gpio_deinit(_pinENA);
}

void StepperMotorControl::operator=(StepperMotorControl &motorControl)
{
    _id = motorControl.GetID();
    _moterName = motorControl.GetMoterName();
    _pinPUL = motorControl.GetPinPUL();
    _pinDIR = motorControl.GetPinDIR();
    _pinENA = motorControl.GetPinENA();
    _stepsPerRound = motorControl.GetStepsPerRound();
}

string StepperMotorControl::StepperMotorControl::GetMoterName()
{
    return _moterName;
}
int StepperMotorControl::GetPinPUL()
{
    return _pinPUL;
}
int StepperMotorControl::GetPinDIR()
{
    return _pinDIR;
}
int StepperMotorControl::GetPinENA()
{
    return _pinENA;
}

long StepperMotorControl::GetStepCount()
{
    return stepCount;
}
double StepperMotorControl::GetRoundCount()
{
    return (double)stepCount / _stepsPerRound;
}
double StepperMotorControl::GetAngleCount(bool isTotal)
{
    if (isTotal)
    {
        return ConvertQty_Steps_toAngels(stepCount);
    }
    else
    {
        return ConvertQty_Steps_toAngels(stepCount % _stepsPerRound);
    }
}
double StepperMotorControl::GetRadiansCount(bool isTotal)
{
    if (isTotal)
    {
        return ConvertQty_Steps_toRadians(stepCount);
    }
    else
    {
        return ConvertQty_Steps_toRadians(stepCount % _stepsPerRound);
    }
}

const string StepperMotorControl::Help()
{
    return helpString;
}
void StepperMotorControl::DTSetGPIO_PLU(bool onOrOff)
{
    _SetGPIO(_pinPUL, onOrOff);
}
void StepperMotorControl::DTSetGPIO_DIR(bool onOrOff)
{
    _SetGPIO(_pinDIR, onOrOff);
}
void StepperMotorControl::DTSetGPIO_ENA(bool onOrOff)
{
    _SetGPIO(_pinENA, onOrOff);
}
void StepperMotorControl::_SetGPIO(int pin, bool onOrOff)
{
    int el = 0;
    if (onOrOff)
    {
        el = 1;
    }
    gpio_put(pin, el);
}

int StepperMotorControl::GetStepsPerRound()
{
    return _stepsPerRound;
}
double StepperMotorControl::GetRecommendSPS(double sps)
{
    double oneSubItv = (double)500000 / _timerInterval / sps;
    int oneSubRound = (int)(oneSubItv + 0.5);
    if (oneSubRound < 1)
    {
        oneSubRound = 1;
    }
    return (double)500000 / _timerInterval / oneSubRound;
}
double StepperMotorControl::GetRecommendRPS(double rps)
{
    double SPS = _stepsPerRound * rps;
    double rcmdSPS = GetRecommendSPS(SPS);
    return rcmdSPS / _stepsPerRound;
}
double StepperMotorControl::GetRecommendRPM(double rpm)
{
    return GetRecommendRPS(rpm / 60) * 60;
}

// 设定恒定速度
bool StepperMotorControl::SetSpeedSPS(double sps)
{
    // 计算恒速脉冲间隔；
    bool result = _SetSPSSubdivs(sps, _pkgStandard);
    if (result)
    {
        _topSpeedSPS = sps;
        _tickSubdiv = _pkgStandard.subdivs[0];

        // ok
        /*
                        printf("SetSpeedSPS\n");
                        for(int i =0,iv = _tickSubdivsLength;i<iv;++i)
                        {
                            printf("%d ",_pkgStandard.subdivs[i]);
                        }
                        printf("\n");*/
    }
    return result;
}
bool StepperMotorControl::_SetSPSSubdivs(double sps, TickPackage &pkg)
{
    // 根据速度，计算一次脉冲需要多少个tick（小数）
    double oneSubItv = (double)500000 / _timerInterval / sps;
    if (oneSubItv < 1)
    {
        return false;
    }
    // 按照细分数量，计算细分数组中需要的tick数量（整数）；
    int itvRound = (int)(oneSubItv + 0.5);
    int i;
    for (i = 0; i < _tickSubdivsLength; ++i)
    {
        pkg.subdivs[i] = itvRound;
    }
    int itvLeft, idx;
    bool oneSubItvGreatorThanRound = oneSubItv > itvRound;

    itvLeft = (int)(((double)itvRound - oneSubItv) * _tickSubdivsLength);
    if (oneSubItvGreatorThanRound)
    {
        itvLeft *= -1;
    }
    for (i = itvLeft; i > 0; --i)
    {
        idx = (int)(_tickSubdivsLength * (1 - (i - 1) / (double)itvLeft)) - 1;
        pkg.subdivs[idx] += oneSubItvGreatorThanRound ? 1 : -1;
    }
    return true;
}
bool StepperMotorControl::SetSpeedRPS(double rps)
{
    double SPS = _stepsPerRound * rps;
    return SetSpeedSPS(SPS);
}
bool StepperMotorControl::SetSpeedRPM(double rpm)
{
    return SetSpeedRPS(rpm / 60);
}
double StepperMotorControl::GetSpeedSPS()
{
    return _topSpeedSPS;
}
double StepperMotorControl::GetSpeedRPS()
{
    return _topSpeedSPS / _stepsPerRound;
}
double StepperMotorControl::GetSpeedRPM()
{
    return GetSpeedRPS() * 60;
}
/*
void StepperMotorControl::SetSpeedStepFrequence(double frequency)
{
    speedSPS =
}
double StepperMotorControl::GetSpeedStepFrequence()
{
}
*/

// 设定缓动速度

void StepperMotorControl::SetFlexAccelerationSPSS(double spss)
{
    if (spss <= 0)
    {
        throw "flex-turn acceleration must be greater than Zero.";
    }
    if(!checkHeapSufficient())
    {
        throwHeapInsufficient();
    }
    _flexAccelerationSPSS = spss;

    // 计算缓动序列
    _flexSqsStepCount = 0;
    _flexSqsStart.clear();
    TickPackage tPkg{
        {},
        0,
        1};
    int startSqsHalfStepsCount = 1;
    int testTickCount = 0;
    int subdivTickCount = 0;
    double spsCount = 0;
    bool iSwitch = false;
    double spsIncre = _flexAccelerationSPSS * _timerInterval / 1000000;

    volatile int pkgCount = 0;

    while (true)
    {
        ++testTickCount;
        spsCount += spsIncre;

        double targetStepsBy2 = (0 + spsCount) / 2 * ((double)_timerInterval * testTickCount / 1000000) * 2;

        if (startSqsHalfStepsCount < targetStepsBy2)
        {
            // switch
            iSwitch = !iSwitch;
            if (tPkg.subdivsLength >= _tickSubdivsLength)
            {
                _flexSqsStart.push_back(tPkg);

                if (++pkgCount >= 256)
                {
                    _flexAccelerationSPSS = 0;
                    _flexSqsStart.clear();
                    _flexSqsEnd.clear();
                    throw "TickPackage overflow.";
                }

                tPkg.subdivsLength = 0;
                tPkg.repeatTimes = 1;
            }
            tPkg.subdivs[tPkg.subdivsLength++] = subdivTickCount;

            ++startSqsHalfStepsCount;
            if (iSwitch)
            {
                ++_flexSqsStepCount;
            }
            else
            {
                if (spsCount + spsIncre > _topSpeedSPS)
                {
                    // 不可继续加速，接近或到达最大速度；
                    _flexSqsStart.push_back(tPkg);
                    break;
                }
            }
            subdivTickCount = 1;
        }
        else
        {
            ++subdivTickCount;
        }
    }

    if(!checkHeapSufficient())
    {
        _flexSqsStart.clear();
        _flexSqsEnd.clear();
        throwHeapInsufficient();
    }

    // 将启动序列逆序复制一份
    _flexSqsEnd.clear();
    int i, j, k;
    for (i = _flexSqsStart.size() - 1; i >= 0; --i)
    {
        tPkg = _flexSqsStart[i];
        TickPackage newPkg{
            {},
            tPkg.subdivsLength,
            tPkg.repeatTimes};
        for (j = tPkg.subdivsLength - 1, k = 0; j >= 0; --j, ++k)
        {
            newPkg.subdivs[k] = tPkg.subdivs[j];
        }
        _flexSqsEnd.push_back(newPkg);
    }
}
void StepperMotorControl::SetFlexAccelerationRPSS(double rpss)
{
    double spss = rpss * _stepsPerRound;
    SetFlexAccelerationSPSS(spss);
}
void StepperMotorControl::SetFlexAccelerationRPMM(double rpmm)
{
    double spss = rpmm * _stepsPerRound * 3600;
    SetFlexAccelerationSPSS(spss);
}
double StepperMotorControl::GetFlexAccelerationSPSS()
{
    return _flexAccelerationSPSS;
}

void StepperMotorControl::SetFlex2JerkSPSSS(double spsss)
{
    if (spsss <= 0)
    {
        throw "Jerk must be greater than Zero.";
    }
    _flexJerkSPSSS = spsss;
}
double StepperMotorControl::GetFlex2JerkSPSSS()
{
    return _flexJerkSPSSS;
}

void StepperMotorControl::_SetDirection(bool isClockwise)
{
    if (_direction == isClockwise)
    {
        return;
    }
    _direction = isClockwise;
    dtControlCode = _direction ? 1 : 2;
}

// 恒速转动
bool StepperMotorControl::TurnSteps(long steps, double rps, bool setAsDefaultSpeed)
{
    if (_topSpeedSPS <= 0)
    {
        throw "speed not set.";
    }
    if (steps == 0)
    {
        return false;
    }
    _SetDirection(steps > 0);
    if (steps < 0)
    {
        steps = -steps;
    }
    double speedSPS = rps * _stepsPerRound;
    if (setAsDefaultSpeed)
    {
        if (!SetSpeedSPS(speedSPS))
        {
            return false;
        }
        vector<TickPackage> sq;
        _GetSubdivs(_pkgStandard, steps, sq);
        _EnqueueSubdivs(sq);
    }
    else
    {
        TickPackage tmpPkg;
        if (!_SetSPSSubdivs(speedSPS, tmpPkg))
        {
            return false;
        }
        vector<TickPackage> sq;
        _GetSubdivs(tmpPkg, steps, sq);
        _EnqueueSubdivs(sq);
    }
    return true;
}

void StepperMotorControl::TurnSteps(long steps)
{
    if (_topSpeedSPS <= 0)
    {
        throw "speed not set.";
    }
    if (steps == 0)
    {
        return;
    }
    _SetDirection(steps > 0);
    if (steps < 0)
    {
        steps = -steps;
    }
    vector<TickPackage> sq;
    _GetSubdivs(_pkgStandard, steps, sq);
    _EnqueueSubdivs(sq);
}
bool StepperMotorControl::TurnRounds(float rounds, double rps, bool setAsDefaultSpeed)
{
    double steps = rounds * _stepsPerRound;
    return TurnSteps(steps, rps, setAsDefaultSpeed);
}
void StepperMotorControl::TurnRounds(float rounds)
{
    double steps = rounds * _stepsPerRound;
    TurnSteps(steps);
}
bool StepperMotorControl::TurnAngles(float angels, double rps, bool setAsDefaultSpeed)
{
    return TurnSteps(ConvertQty_Angels_toSteps(angels),
                     rps, setAsDefaultSpeed);
}
void StepperMotorControl::TurnAngles(float angels)
{
    TurnSteps(ConvertQty_Angels_toSteps(angels));
}
bool StepperMotorControl::TurnRadians(float radians, double rps, bool setAsDefaultSpeed)
{
    return TurnSteps(ConvertQty_Radians_toSteps(radians),
                     rps, setAsDefaultSpeed);
}
void StepperMotorControl::TurnRadians(float radians)
{
    TurnSteps(ConvertQty_Radians_toSteps(radians));
}

// 2024-07-09
// 通过速度-时间曲线，确定某一时刻需达到的转动角度，如果少于，则执行转动；
// 两种情况，加速到最大速度，保持一段时间，减速，或，加速到一定速度（无保持），减速；

// 缓动转动
void StepperMotorControl::FlexTurnSteps(long steps)
{
    if (_topSpeedSPS <= 0)
    {
        throw "flex-turn max speed not set.";
    }
    if (_flexAccelerationSPSS <= 0)
    {
        throw "flex-turn acceleration not set.";
    }
    _SetDirection(steps > 0);
    if (steps < 0)
    {
        steps = -steps;
    }

    long stepsD2 = steps / 2;
    if (_flexSqsStepCount <= stepsD2)
    {
        // 步数超过启停序列总长，将用到完整的启停序列
        int stepsLeft = steps - _flexSqsStepCount - _flexSqsStepCount;
        if (stepsLeft > 0)
        {
            vector<TickPackage> sqsMid;
            _GetSubdivs(_pkgStandard, stepsLeft, sqsMid);
            _EnqueueSubdivs(_flexSqsStart);
            _EnqueueSubdivs(sqsMid);
            _EnqueueSubdivs(_flexSqsEnd);
        }
        else
        {
            _EnqueueSubdivs(_flexSqsStart);
            _EnqueueSubdivs(_flexSqsEnd);
        }
    }
    else
    {
        bool isStepsOdd = (steps % 2 == 1);
        vector<TickPackage> sqStart;
        _GetSubdivs(_flexSqsStart, stepsD2, false, sqStart);
        vector<TickPackage> sqEnd;
        _GetSubdivs(_flexSqsStart, stepsD2, true, sqEnd);

        if (isStepsOdd)
        {
            int div1, div2;
            _GetSubdivStep(_flexSqsStart, stepsD2, div1, div2);
            TickPackage tmpPkg{
                {div1, div2},
                2,
                1};
            _EnqueueSubdivs(sqStart);
            queue_add_blocking(&queueTickPackage, &tmpPkg);
            _EnqueueSubdivs(sqEnd);
        }
        else
        {
            _EnqueueSubdivs(sqStart);
            _EnqueueSubdivs(sqEnd);
        }
    }
}
void StepperMotorControl::_EnqueueSubdivs(vector<TickPackage> &sequence)
{
    for (int i = 0, iv = sequence.size(); i < iv; ++i)
    {
        queue_add_blocking(&queueTickPackage, &sequence[i]);
    }
}

void StepperMotorControl::_GetSubdivs(TickPackage &pkg, int steps, vector<StepperMotorControl::TickPackage> &result)
{
    int stepsInPkg = pkg.subdivsLength / 2;
    if (stepsInPkg < steps)
    {
        TickPackage tmpPkg = pkg;    
        tmpPkg.repeatTimes = 0;
        while (stepsInPkg <= steps)
        {
            steps -= stepsInPkg;
            tmpPkg.repeatTimes++;
        }
        result.push_back(tmpPkg);
        if(steps > 0)
        {
            TickPackage tmpPkg = pkg;
            tmpPkg.subdivsLength = steps * 2;
            result.push_back(tmpPkg);
        }
    }
    else if (stepsInPkg == steps)
    {
        result.push_back(pkg);
    }
    else
    {
        TickPackage tmpPkg = pkg;
        tmpPkg.subdivsLength = steps * 2;
        result.push_back(tmpPkg);
    }
}
void StepperMotorControl::_GetSubdivs(vector<TickPackage> &sequence, int steps, bool backwards, vector<StepperMotorControl::TickPackage> &result)
{
    int stepsInPkg;
    if (backwards)
    {
        // 顺序找到最后一个需要的包，然后逆序写入队列
        int pkgIdx=0,pkgStepsCount = 0;
        int i, iv, j;
        for (pkgIdx = 0, iv = sequence.size(); pkgIdx < iv; ++pkgIdx)
        {
            TickPackage &curTPkg = sequence[pkgIdx];
            pkgStepsCount += curTPkg.subdivsLength / 2;
            if(steps <= pkgStepsCount)
            {
                break;
            }
        }
        if (pkgStepsCount < steps)
        {
            throw "insufficient steps in flex-turn sequence.";
        }
        if(pkgStepsCount != steps)
        {
            // 需要从pkg中间截取
            TickPackage tmpTPkg = sequence[pkgIdx--];
            tmpTPkg.subdivsLength -= (pkgStepsCount - steps) * 2;
            for(i = 0,iv = tmpTPkg.subdivsLength/2;i< iv;++i)
            {
                j = tmpTPkg.subdivs[i];
                tmpTPkg.subdivs[i] = tmpTPkg.subdivs[tmpTPkg.subdivsLength - i - 1];
                tmpTPkg.subdivs[tmpTPkg.subdivsLength - i - 1] = j;
            }
            result.push_back(tmpTPkg);
        }
        for(;pkgIdx >=0;)
        {
            TickPackage tmpTPkg = sequence[pkgIdx--];
            for(i = 0,iv = tmpTPkg.subdivsLength/2;i< iv;++i)
            {
                j = tmpTPkg.subdivs[i];
                tmpTPkg.subdivs[i] = tmpTPkg.subdivs[tmpTPkg.subdivsLength - i - 1];
                tmpTPkg.subdivs[tmpTPkg.subdivsLength - i - 1] = j;
            }
            result.push_back(tmpTPkg);
        }
    }
    else
    {
        for (int i = 0, iv = sequence.size(); i < iv; ++i)
        {
            if (steps <= 0)
            {
                break;
            }

            TickPackage &curTPkg = sequence[i];
            stepsInPkg = curTPkg.subdivsLength / 2;
            if (stepsInPkg <= steps)
            {
                result.push_back(curTPkg);
                steps -= stepsInPkg;
            }
            else
            {
                TickPackage tmpTPkg = sequence[i];
                tmpTPkg.subdivsLength = steps * 2;
                result.push_back(tmpTPkg);
                steps = 0;
            }
        }
    }
}

void StepperMotorControl::_GetSubdivStep(vector<TickPackage> &sequence,
                                         int stepIndex, int &outDiv1, int &outDiv2)
{
    outDiv1 = 0;
    outDiv2 = 0;
    int stepsInPkg;
    for (int i = 0, iv = sequence.size(); i < iv; ++i)
    {
        TickPackage &curTPkg = sequence[i];
        stepsInPkg = curTPkg.subdivsLength / 2;
        if (stepIndex < stepsInPkg)
        {
            // in range
            int divIdx = stepIndex * 2;
            outDiv1 = curTPkg.subdivs[divIdx];
            outDiv2 = curTPkg.subdivs[divIdx + 1];
            break;
        }
        else
        {
            // out of range, continue
            stepIndex -= stepsInPkg;
        }
    }
}
void StepperMotorControl::FlexTurnRounds(float rounds)
{
    double steps = rounds * _stepsPerRound;
    FlexTurnSteps(steps);
}
void StepperMotorControl::FlexTurnAngles(float angels)
{
    FlexTurnSteps(ConvertQty_Angels_toSteps(angels));
}
void StepperMotorControl::FlexTurnRadians(float radians)
{
    FlexTurnSteps(ConvertQty_Radians_toSteps(radians));
}

void StepperMotorControl::Flex2TurnSteps(long steps)
{
    if (_topSpeedSPS <= 0)
    {
        throw "flex-turn max speed not set.";
    }
    if (_flexJerkSPSSS <= 0)
    {
        throw "flex-turn jerk not set.";
    }
    if(!checkHeapSufficient())
    {
        throwHeapInsufficient();
    }

    // 模拟计算前半段的脉冲序列；
    double spssCur;
    double spsCur = 0, spsCurPre = 0;

    int stepsD2 = steps / 2;
    bool isStepsOdd = (steps % 2 == 1);

    // 加速到最大速度，需要的时间
    double topSpeedTime = 2 * sqrt(_topSpeedSPS / _flexJerkSPSSS);

    bool jerkUpOrDown = true;
    int halfStepCount = 0;

    // 检查加速到最大速度时，能经过的距离；
    double fullHafeWay = _topSpeedSPS * topSpeedTime / 2;
    double targetTopSps, targetTopSpsD2;
    int i, iv;
    if (fullHafeWay <= stepsD2)
    {
        // 需要完全加速
        targetTopSps = _topSpeedSPS;

        // 最后不一定剩余这么多；
        // stepsLeft = (long)(steps - targetMaxSps * fullSpeedTime);
    }
    else
    {
        // 无需完全加速，加速中途需要减速
        // 最大速度要小于设定的最大速度；
        // 计算需要的最大速度；

        topSpeedTime = pow(4 * steps / _flexJerkSPSSS, 1.0 / 3);
        targetTopSps = topSpeedTime * topSpeedTime * _flexJerkSPSSS / 4;
        fullHafeWay = targetTopSps * topSpeedTime / 2;
    }
    targetTopSpsD2 = targetTopSps / 2;

    vector<TickPackage> sqStart;
    TickPackage curTP;
    int curTPDivIdx = 0;

    // double spssIncre = jerkSPSSS * timerInterval / 1000000;
    double timerInterval_sec = (double)_timerInterval / 1000000;
    long tickCount = 0, tickCount_speedDown = 0;
    double distance = 0;
    int subDivCount = 0;

    int pkgCount = 0;

    while (true)
    {
        ++tickCount;
        if (jerkUpOrDown)
        {
            // spssCur = spssIncre * tickCount * timerInterval / 1000000;
            //  a = j * t;
            spssCur = _flexJerkSPSSS * tickCount * timerInterval_sec;
        }
        else
        {
            tickCount_speedDown += 2;
            // spssCur = spssIncre * (tickCount - tickCount_speedDown) * timerInterval / 1000000;
            spssCur = _flexJerkSPSSS * (tickCount - tickCount_speedDown) * timerInterval_sec;
        }
        // v = a * t
        spsCur += spssCur * timerInterval_sec;

        distance += (spsCurPre + spsCur) * timerInterval_sec;

        if (2 <= (distance - halfStepCount))
        {
            throw "Cant catch up!";
        }

        if (1 < (distance - halfStepCount))
        {
            // logList.Add(DateTime.Now.ToString("HH:mm:ss.fff") + " Half step.");
            if (halfStepCount % 2 == 0)
            {
                // votage high
                if (_tickSubdivsLength <= curTPDivIdx + 1)
                {
                    curTP.repeatTimes = 1;
                    curTP.subdivsLength = _tickSubdivsLength;
                    sqStart.push_back(curTP);

                    if (++pkgCount >= 256)
                    {
                        throw "Tick-Package overflow.";
                    }

                    /*curTP = new TickPackage()
                    {
                        repeatTimes = 1,
                        subdivsLength = 64,
                    };*/
                    curTPDivIdx = 0;
                }
                curTP.subdivs[curTPDivIdx++] = subDivCount;
            }
            else
            {
                // votage low
                curTP.subdivs[curTPDivIdx++] = subDivCount;

                // if (spsCur >= targetTopSps || spssCur <= 0)
                if (!jerkUpOrDown && spssCur - (_flexJerkSPSSS * timerInterval_sec) <= 0)
                {
                    curTP.repeatTimes = 1;
                    curTP.subdivsLength = curTPDivIdx;
                    sqStart.push_back(curTP);
                    break;
                }

                if (jerkUpOrDown && spsCur >= targetTopSpsD2)
                {
                    jerkUpOrDown = false;
                }
            }
            ++halfStepCount;
            subDivCount = 1;
        }
        else
        {
            ++subDivCount;
        }
        spsCurPre = spsCur;
    }

    if(!checkHeapSufficient())
    {
        sqStart.clear();
        throwHeapInsufficient();
    }

    int stepsInSq = 0;
    for (i = 0, iv = sqStart.size(); i < iv; ++i)
    {
        stepsInSq += sqStart[i].subdivsLength / 2;
    }

    if (stepsD2 < stepsInSq)
    {
        // 如果序列中的步数，多于一半，则删减
        int stepsToRemove;
        int stepsInPkg;
        int i;
        if (isStepsOdd)
        {
            stepsToRemove = stepsInSq - stepsD2 - 1;
        }
        else
        {
            stepsToRemove = stepsInSq - stepsD2;
        }
        while (true)
        {
            i = sqStart.size() - 1;
            stepsInPkg = sqStart[i].subdivsLength / 2;
            if (stepsInPkg <= stepsToRemove)
            {
                sqStart.pop_back();
                stepsToRemove -= stepsInPkg;
                stepsInSq -= stepsInPkg;
            }
            else
            {
                sqStart[i].subdivsLength -= stepsToRemove * 2;
                stepsInSq -= stepsToRemove;
                break;
            }
        }

        if (isStepsOdd)
        {
            // 跳过 sqStart 最后一位，取逆序
            // sqEnd
            vector<TickPackage> sqEnd;
            _GetSubdivs(sqStart, stepsInSq - 1, true, sqEnd);
            // enqueue, sqStart, sqEnd
            _EnqueueSubdivs(sqStart);
            _EnqueueSubdivs(sqEnd);
        }
        else
        {
            // 取 spStart 完整逆序；
            // sqEnd
            vector<TickPackage> sqEnd;
            _GetSubdivs(sqStart, stepsInSq, true, sqEnd);
            _EnqueueSubdivs(sqStart);
            _EnqueueSubdivs(sqEnd);
        }
    }
    else
    {
        // 需要以最后速度补充中间匀速的部分
        int stepsLeft = steps - stepsInSq - stepsInSq;
        TickPackage tmpPkg;
        if (!_SetSPSSubdivs(targetTopSps, tmpPkg))
        {
            throw "Impossible, topSPS invalid??";
        }
        vector<TickPackage> sqMid;
        _GetSubdivs(tmpPkg, stepsLeft, sqMid);

        // 取 spStart 完整逆序；
        vector<TickPackage> sqEnd;
        _GetSubdivs(sqStart, stepsInSq, true, sqEnd);
        // enqueue, sqStart, sqMid, sqEnd
        _EnqueueSubdivs(sqStart);
        _EnqueueSubdivs(sqMid);
        _EnqueueSubdivs(sqEnd);
    }
}
void StepperMotorControl::Flex2TurnRounds(float rounds)
{
    double steps = rounds * _stepsPerRound;
    Flex2TurnSteps(steps);
}
void StepperMotorControl::Flex2TurnAngles(float angels)
{
    Flex2TurnSteps(ConvertQty_Angels_toSteps(angels));
}
void StepperMotorControl::Flex2TurnRadians(float radians)
{
    Flex2TurnSteps(ConvertQty_Radians_toSteps(radians));
}

// 2024-07-09
// 急停和释放均会抛弃后续列队的动作；
// 其他
void StepperMotorControl::Brake()
{
    dtControlCode = 3;
}
void StepperMotorControl::Loose()
{
    dtControlCode = 4;
}
void StepperMotorControl::Regain()
{
    dtControlCode = 5;
}

void StepperMotorControl::ResetPosition(bool isFullReset, bool flexTurn)
{
    if (stepCount == 0)
    {
        return;
    }
    long steps = -stepCount;
    if (!isFullReset)
    {
        steps %= _stepsPerRound;
        if (steps == 0)
        {
            return;
        }
        int sprD2 = _stepsPerRound / 2;
        if (steps > 0)
        {
            if (sprD2 < steps)
            {
                steps -= _stepsPerRound;
            }
        }
        else
        {
            if (steps < -sprD2)
            {
                steps += _stepsPerRound;
            }
        }
    }
    if (flexTurn)
    {
        FlexTurnSteps(steps);
    }
    else
    {
        TurnSteps(steps);
    }
}
void StepperMotorControl::ResetCounter()
{
    stepCount = 0;
}

// 速度转换
double StepperMotorControl::ConvertSpeed_RPS_toSPS(double rps)
{
    return rps * _stepsPerRound;
}
double StepperMotorControl::ConvertSpeed_SPS_toRPS(double sps)
{
    return sps / _stepsPerRound;
}
double StepperMotorControl::ConvertSpeed_RPM_toSPS(double rpm)
{
    return rpm / 60 * _stepsPerRound;
}
double StepperMotorControl::ConvertSpeed_SPS_toRPM(double sps)
{
    return sps / _stepsPerRound * 60;
}

double StepperMotorControl::ConvertQty_Angels_toRadians(double angels)
{
    return angels * MATH_PI / 180;
}
double StepperMotorControl::ConvertQty_Radians_toAngels(double radians)
{
    return radians * 180 / MATH_PI;
}
int StepperMotorControl::ConvertQty_Angels_toSteps(double angels)
{
    return _stepsPerRound * (angels / 360);
}
double StepperMotorControl::ConvertQty_Steps_toAngels(int steps)
{
    return 360 * (steps / _stepsPerRound);
}
int StepperMotorControl::ConvertQty_Radians_toSteps(double radians)
{
    return _stepsPerRound * (radians / 2 / MATH_PI);
}
double StepperMotorControl::ConvertQty_Steps_toRadians(int steps)
{
    return 2 * MATH_PI * (steps / _stepsPerRound);
}

// 测试
void StepperMotorControl::SuddenReverse(int totalSteps, double rps, bool setAsDefaultSpeed)
{
}

void StepperMotorControl::SetTimerInterval(int microSec)
{
    _timerInterval = microSec;
}
int StepperMotorControl::DTGetSubdiv()
{
    return _tickSubdiv;
}