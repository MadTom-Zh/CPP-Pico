

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "pico/stdlib.h"

#include "Dependence/StringHelper.h"

#include "SerialCom/CMDInfo.h"
#include "SerialCom/SerialCom.h"

using namespace std;
using namespace MadTomDev::Base;
using namespace MadTomDev::Common;

// const uint LED_PIN = 25;
const unsigned int LED_PIN = 25;

bool isLedOn = false;
void LedOnOff()
{
    if (isLedOn)
    {
        gpio_put(LED_PIN, 0);
        isLedOn = false;
    }
    else
    {
        gpio_put(LED_PIN, 1);
        isLedOn = true;
    }
}

string Test()
{
    string result("Hello!");
    return result;
}

string Echo(const string &msg)
{
    string result(msg);
    return result;
}

string CalAdd(int a, int b)
{
    string result(32, '\0');
    // result.reserve(32);
    sprintf(&result[0], "%d", a + b);
    return result;
}

/*
void printList(vector<string> & list)
{
    string s;
    for(int i=0,iv = list.size();i<iv;++i)
    {
        s = list[i];
        printf("i [%s]", s);
    }
}*/

int main()
{
    stdio_init_all();
    printf("Hello world!\n");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);



    // for testing;
    // volatile int a;


    /*
     测试场景
     Test();                // 返回字符 Hello
     Echo(char *);          // 复述 输入的 文字
     CalAdd(int a, int b);  // 返回 a+b 的计算结果(字符串)
    */

    SerialCom *sc = new SerialCom(128, 128);
    sc->RegisterCMD("Test", (void *)Test);
    sc->RegisterCMD("Echo -msg S", (void *)Echo);
    sc->RegisterCMD("CalAdd -a L -b L", (void *)CalAdd);

   

    while (true)
    {
        LedOnOff();

        printf("Waiting for input!\n");

        try
        {
            sc->WaitForInput();
            sc->ProcessNOutput();

            if (sc->HasCMD("Test"))
            {
                printf("Test remove test.\n");
                string cmd1("Test");
                if (sc->UnRegisterCMD(cmd1) == true)
                {
                    printf("Test() has been unregistered.\n");
                }
            }
        }
        catch (char *errMsg)
        {
            printf("ErrC: %s\n", errMsg);
            free(errMsg);
            // break;
        }
        catch (string &errMsg1)
        {
            printf("ErrS: %s\n", errMsg1.c_str());
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
