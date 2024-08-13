
// #include <stdio.h>
#include "pico/stdlib.h"

#include "StepperMotorControl/StepperMotorControl.h"
using MadTomDev::Motor::StepperMotorControl;
#include "StepperMotorControl/DriveTimer.h"
using MadTomDev::Motor::DriveTimer;

#include "SerialCom/CMDInfo.h"
#include "SerialCom/SerialCom.h"


using namespace std;
using namespace MadTomDev::Base;
// using namespace MadTomDev::Common;

StepperMotorControl * ptrMc;
DriveTimer * ptrDt;

string DTGetSubdiv()
{
    string result(16, '\0');
    sprintf(&result[0], "%d", ptrMc->DTGetSubdiv());
    return result;
}
string GetMoterName()
{
    string result(ptrMc->GetMoterName());
    return result;
}
string GetPinPUL()
{
    string result(4, '\0');
    sprintf(&result[0], "%d", ptrMc->GetPinPUL());
    return result;
}
string GetPinDIR()
{
    string result(4, '\0');
    sprintf(&result[0], "%d", ptrMc->GetPinDIR());
    return result;
}
string GetPinENA()
{
    string result(4, '\0');
    sprintf(&result[0], "%d", ptrMc->GetPinENA());
    return result;
}
string GetLastDirection()
{
    if (ptrMc->_direction)
    {
        string result("Clockwise");
        return result;
    }
    else
    {
        string result("Counter-Clockwise");
        return result;
    }
}

string ResetCounter()
{
    ptrMc->ResetCounter();
    string result("Done");
    return result;
}
string GetStepCount()
{
    string result(16, '\0');
    sprintf(&result[0], "%ld", ptrMc->GetStepCount());
    return result;
}
string GetRoundCount()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetRoundCount());
    return result;
}
string GetAngleCount(long isTotal)
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetAngleCount(isTotal != 0));
    return result;
}
string GetRadiansCount(long isTotal)
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetRadiansCount(isTotal != 0));
    return result;
}

string Help()
{
    string result(ptrMc->Help());
    return result;
}
string GetStepsPerRound()
{
    string result(8, '\0');
    sprintf(&result[0], "%d", ptrMc->GetStepsPerRound());
    return result;
}
string GetRecommendSPS(double testSPS)
{
    string result(8, '\0');
    sprintf(&result[0], "%g", ptrMc->GetRecommendSPS(testSPS));
    return result;
}
string GetRecommendRPS(double testRPS)
{
    string result(8, '\0');
    sprintf(&result[0], "%g", ptrMc->GetRecommendRPS(testRPS));
    return result;
}
string GetRecommendRPM(double testRPM)
{
    string result(8, '\0');
    sprintf(&result[0], "%g", ptrMc->GetRecommendRPM(testRPM));
    return result;
}

string SetSpeedSPS(double speedSPS)
{
    if (ptrMc->SetSpeedSPS(speedSPS))
    {
        string result("Done");
        return result;
    }
    else
    {
        string result("Failed");
        return result;
    }
}
string SetSpeedRPS(double speedRPS)
{
    if (ptrMc->SetSpeedRPS(speedRPS))
    {
        string result("Done");
        return result;
    }
    else
    {
        string result("Failed");
        return result;
    }
}
string SetSpeedRPM(double speedRPM)
{
    if (ptrMc->SetSpeedRPM(speedRPM))
    {
        string result("Done");
        return result;
    }
    else
    {
        string result("Failed");
        return result;
    }
}
string GetSpeedSPS()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetSpeedSPS());
    return result;
}
string GetSpeedRPS()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetSpeedRPS());
    return result;
}
string GetSpeedRPM()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetSpeedRPM());
    return result;
}


string SetFlexAccelerationSPSS(double spss)
{
    ptrMc->SetFlexAccelerationSPSS(spss);    
    string result("Called.");
    return result;       
}
string SetFlexAccelerationRPSS(double rpss)
{
    ptrMc->SetFlexAccelerationRPSS(rpss);
    string result("Called.");
    return result;      
}
string SetFlexAccelerationRPMM(double rpmm)
{
    ptrMc->SetFlexAccelerationRPMM(rpmm);
    string result("Called.");
    return result;      
}
string GetFlexAccelerationSPSS()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetFlexAccelerationSPSS());
    return result;
}

string SetFlex2JerkSPSSS(double spsss)
{
    ptrMc->SetFlex2JerkSPSSS(spsss);
    string result("Called.");
    return result; 
}
string GetFlex2JerkSPSSS()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrMc->GetFlex2JerkSPSSS());
    return result;
}

string TurnSteps(long steps, double speedRPS, long setAsDefaultSpeed)
{
    if (ptrMc->TurnSteps(steps, speedRPS, setAsDefaultSpeed != 0))
    {
        string result("Called");
        return result;
    }
    else
    {
        string result("Call failed");
        return result;
    }
}
string TurnStepsS(long steps)
{
    ptrMc->TurnSteps(steps);
    string result("Called");
    return result;
}
string TurnRounds(double rounds, double speedRPS, long setAsDefaultSpeed)
{
    if (ptrMc->TurnRounds(rounds, speedRPS, setAsDefaultSpeed != 0))
    {
        string result("Called");
        return result;
    }
    else
    {
        string result("Call failed");
        return result;
    }
}
string TurnRoundsS(double rounds)
{
    ptrMc->TurnRounds(rounds);
    string result("Called");
    return result;
}
string TurnAngles(double angels, double speedRPS, long setAsDefaultSpeed)
{
    if (ptrMc->TurnAngles(angels, speedRPS, setAsDefaultSpeed != 0))
    {
        string result("Called");
        return result;
    }
    else
    {
        string result("Call failed");
        return result;
    }
}
string TurnAnglesS(double angels)
{
    ptrMc->TurnAngles(angels);
    string result("Called");
    return result;
}
string TurnRadians(double radians, double speedRPS, long setAsDefaultSpeed)
{
    if (ptrMc->TurnRadians(radians, speedRPS, setAsDefaultSpeed != 0))
    {
        string result("Called");
        return result;
    }
    else
    {
        string result("Call failed");
        return result;
    }
}
string TurnRadiansS(double radians)
{
    ptrMc->TurnRadians(radians);
    string result("Called");
    return result;
}

string FlexTurnSteps(long steps)
{
    ptrMc->FlexTurnSteps(steps);
    string result("Called");
    return result;
}
string FlexTurnRounds(double rounds)
{
    ptrMc->FlexTurnRounds((float)rounds);
    string result("Called");
    return result;
}
string FlexTurnAngles(double angels)
{
    ptrMc->FlexTurnAngles((float)angels);
    string result("Called");
    return result;
}
string FlexTurnRadians(double radians)
{
    ptrMc->FlexTurnRadians((float)radians);
    string result("Called");
    return result;
}

string Flex2TurnSteps(long steps)
{
    ptrMc->Flex2TurnSteps(steps);
    string result("Called");
    return result;
}
string Flex2TurnRounds(float rounds)
{
    ptrMc->Flex2TurnRounds(rounds);
    string result("Called");
    return result;
}
string Flex2TurnAngles(float angels)
{
    ptrMc->Flex2TurnAngles(angels);
    string result("Called");
    return result;
}
string Flex2TurnRadians(float radians)
{
    ptrMc->Flex2TurnRadians(radians);
    string result("Called");
    return result;
}

string ResetPosition(long isFullReset, long flexTurn)
{
    ptrMc->ResetPosition(isFullReset != 0, flexTurn != 0);
    string result("Called");
    return result;
}
string Brake()
{
    ptrMc->Brake();
    string result("Called");
    return result;
}
string Loose()
{
    ptrMc->Loose();
    string result("Called");
    return result;
}
string Regain()
{
    
    ptrMc->Regain();
    string result("Called");
    return result;
}
string GetMaxFrequency()
{
    string result(16, '\0');
    sprintf(&result[0], "%g", ptrDt->GetMaxFrequency());
    return result;
}
string GetSyncMissingCount()
{
    string result(16, '\0');
    sprintf(&result[0], "%d", ptrDt->syncMissingCount);
    return result;
}





int main()
{
    stdio_init_all();

    printf("Steeper motor control test start!\n");
    
    DriveTimer dt = DriveTimer{100};
    ptrDt = & dt;
    ptrMc = dt.AddMotor("motor0", 2, 3, 4, 1600);
    
    // set max speed, etc
    double maxFqc = dt.GetMaxFrequency();
    printf("Max Fqc %g\n", maxFqc);
    SetSpeedSPS(maxFqc);    

    

    //printf("before");
    //sc.SetCMD("GetMemUsage");
    //sc.ProcessNOutput();

    SetFlexAccelerationSPSS(9884);


    SetFlex2JerkSPSSS(10000);
    

    dt.TimerStart();

    SerialCom sc {128, 128};

    sc.RegisterCMD("DTGetSubdiv", (void *)DTGetSubdiv);
    sc.RegisterCMD("GetMoterName", (void *)GetMoterName);
    // sc->RegisterCMD("GetMoterName -msg S", (void *)GetMoterName);
    // sc->RegisterCMD("CalAdd -a L -b L", (void *)CalAdd);
    sc.RegisterCMD("GetPinPUL", (void *)GetPinPUL);
    sc.RegisterCMD("GetPinDIR", (void *)GetPinDIR);
    sc.RegisterCMD("GetPinENA", (void *)GetPinENA);
    sc.RegisterCMD("GetLastDirection", (void *)GetLastDirection);

    sc.RegisterCMD("GetStepCount", (void *)GetStepCount);
    sc.RegisterCMD("GetRoundCount", (void *)GetRoundCount);
    sc.RegisterCMD("GetAngleCount -isTotal L", (void *)GetAngleCount);
    sc.RegisterCMD("GetRadiansCount -isTotal L", (void *)GetRadiansCount);

    sc.RegisterCMD("Help", (void *)Help);
    sc.RegisterCMD("GetStepsPerRound", (void *)GetStepsPerRound);
    sc.RegisterCMD("GetRecommendSPS -sps D", (void *)GetRecommendSPS);
    sc.RegisterCMD("GetRecommendRPS -rps D", (void *)GetRecommendRPS);
    sc.RegisterCMD("GetRecommendRPM -rpm D", (void *)GetRecommendRPM);

    sc.RegisterCMD("SetSpeedSPS -sps D", (void *)SetSpeedSPS);
    sc.RegisterCMD("SetSpeedRPS -rps D", (void *)SetSpeedRPS);
    sc.RegisterCMD("SetSpeedRPM -rpm D", (void *)SetSpeedRPM);
    sc.RegisterCMD("GetSpeedSPS", (void *)GetSpeedSPS);
    sc.RegisterCMD("GetSpeedRPS", (void *)GetSpeedRPS);
    sc.RegisterCMD("GetSpeedRPM", (void *)GetSpeedRPM);


    sc.RegisterCMD("SetFlexAccelerationSPSS -spss D", (void *)SetFlexAccelerationSPSS);
    sc.RegisterCMD("SetFlexAccelerationRPSS -rpss D", (void *)SetFlexAccelerationRPSS);
    sc.RegisterCMD("SetFlexAccelerationRPMM -rpmm D", (void *)SetFlexAccelerationRPMM);
    sc.RegisterCMD("GetFlexAccelerationSPSS", (void *)GetFlexAccelerationSPSS);
    
    sc.RegisterCMD("SetFlex2JerkSPSSS -spsss D", (void *)SetFlex2JerkSPSSS);
    sc.RegisterCMD("GetFlex2JerkSPSSS", (void *)GetFlex2JerkSPSSS);

    sc.RegisterCMD("TurnSteps -s L -rps D -SetAsDefaultSpeed L", (void *)TurnSteps);
    sc.RegisterCMD("TurnStepsS -s L", (void *)TurnStepsS);
    sc.RegisterCMD("TurnRounds -r D -rps D -SetAsDefaultSpeed L", (void *)TurnRounds);
    sc.RegisterCMD("TurnRoundsS -r D", (void *)TurnRoundsS);
    sc.RegisterCMD("TurnAngles -a D -rps D -SetAsDefaultSpeed L", (void *)TurnAngles);
    sc.RegisterCMD("TurnAnglesS -a D", (void *)TurnAnglesS);
    sc.RegisterCMD("TurnRadians -r D -rps D -SetAsDefaultSpeed L", (void *)TurnRadians);
    sc.RegisterCMD("TurnRadiansS -r D", (void *)TurnRadiansS);

    sc.RegisterCMD("FlexTurnSteps -s L", (void *)FlexTurnSteps);
    sc.RegisterCMD("FlexTurnRounds -r D", (void *)FlexTurnRounds);
    sc.RegisterCMD("FlexTurnAngles -a D", (void *)FlexTurnAngles);
    sc.RegisterCMD("FlexTurnRadians -r D", (void *)FlexTurnRadians);
    
    sc.RegisterCMD("Flex2TurnSteps -s L", (void *)Flex2TurnSteps);
    sc.RegisterCMD("Flex2TurnRounds -r D", (void *)Flex2TurnRounds);
    sc.RegisterCMD("Flex2TurnAngles -a D", (void *)Flex2TurnAngles);
    sc.RegisterCMD("Flex2TurnRadians -r D", (void *)Flex2TurnRadians);

    sc.RegisterCMD("ResetPosition -isFullReset L -flexTurn L", (void *)ResetPosition);
    sc.RegisterCMD("Brake", (void *)Brake);
    sc.RegisterCMD("Loose", (void *)Loose);
    sc.RegisterCMD("Regain", (void *)Regain);
    
    sc.RegisterCMD("GetMaxFrequency", (void *)GetMaxFrequency);
    sc.RegisterCMD("GetSyncMissingCount", (void *)GetSyncMissingCount);
    

    while (true)
    {
        printf("Waiting for input!\n");

        try
        {
            sc.WaitForInput();
            sc.ProcessNOutput();
        }
        catch (const char *errMsg)
        {
            printf("ErrC: %s\n", errMsg);
            // break;
        }
        catch (char *errMsg1)
        {
            printf("ErrC: %s\n", errMsg1);
            free(errMsg1);
            // break;
        }
        catch (string &errMsg2)
        {
            printf("ErrS: %s\n", errMsg2.c_str());
            // break;
        }
        catch (const std::exception &e)
        {
            printf("ErrE: %s\n", e.what());
            // break;
        }
        catch (...)
        {
            printf("Other Err.\n");
            // break;
        }
    }
    return 0;
}
