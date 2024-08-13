#include <stdio.h>
#include "pico/stdlib.h"

#include <string.h>

#include "SerialCom.h"

const uint LED_PIN = 25;

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

char bufferOfInput[512];
char bufferOfOutput[512];
void WaitForInput()
{
    // printf("Waiting for input.\n");
    scanf("%512s", bufferOfInput);
    LedOnOff();
}
void ProcessInput()
{
    /*
    命令（不分大小写）  命令标记        参数     备注
                    （收< 或 发>）
    hi                  a (<>)                   发送或收到问候
    whoAreYou           b (<)                    询问服务端身份
    Im                  b (>)                    服务端回答身份
    calPlus             c (<)           a1,a2    让服务端计算加法
    calPlus             c (>)           a1       服务端返回加法结果
    UnnoCmd             d (>)                    服务端返回未知命令提示
    */

    int inputStrLength = strlen(bufferOfOutput);
    switch (bufferOfInput[0])
    {
    case 'a':
    {
        /*
        int inputLen = strlen(bufferOfInput);
        // int strLen = strlen(clientName);
        // printf("strLen %d\n", strLen);

        if (inputLen > 1)
        {
            char clientName[64];
            sscanf(bufferOfInput, "a%s", &clientName);
            if (isLedOn)
            {
                sprintf(bufferOfOutput, "aHi, %s!", clientName);
            }
            else
            {
                sprintf(bufferOfOutput, "aHello, %s!", clientName);
            }
        }
        else
        {
            sprintf(bufferOfOutput, (isLedOn ? "aHi!" : "aHello!"));
        }
        */
        if(inputStrLength == 1)
        {
            sprintf(bufferOfOutput,"a");
        }
        else
        {
            sprintf(bufferOfOutput,"z");
        }
    }
    break;
    case 'b':
    {
        if(inputStrLength == 1)
        {
            sprintf(bufferOfOutput, "bTester Pico");
        }
        else
        {
            sprintf(bufferOfOutput,"z");
        }
    }
    break;
    case 'c':
    {
        bool resultOK = true;
        char op;
        double a, b, c;

        //  **  遇到空格就结束，所以不能用空格，否则会被认为是两次输入；

        sscanf(bufferOfInput, "c%c%lf,%lf", &op, &a, &b);
        //printf("op %c, a %f, b %f\n", op, a, b);
        switch (op)
        {
        case '+':
            c = a + b;
            break;
        case '-':
            c = a - b;
            break;
        case '*':
            c = a * b;
            break;
        case '/':
            c = a / b;
            break;
        case '%':
            c = (long)a % (long)b;            
            break;
        default:
            resultOK = false;
            break;
        }
        if (resultOK)
        {
            sprintf(bufferOfOutput, "c%lf", c);
        }
        else
        {
            //sprintf(bufferOfOutput, "cCalculation error, or operator not supported.");

            // one 'c' for error return
            sprintf(bufferOfOutput, "c");
        }
    }
    break;
    case 'h':
    {
        if(inputStrLength == 1)
        {
            sprintf(bufferOfOutput,"h");
        }
        else
        {
            sprintf(bufferOfOutput,"z");
        }
    }
    break;
    default:
    {
        if (inputStrLength > 0)
        {
            //printf("unknow input %s\n", bufferOfInput);
            //sprintf(bufferOfOutput, "dUnknow command.");
            sprintf(bufferOfOutput, "z");
        }
    }
    break;
    }
}
void OutputResult()
{
    printf("%s\n", bufferOfOutput);
}


char * Func1(int a,int b)
{
    char * result ="ab";
    result result;
}

RegistItem:: FuncII_S Func2(int a,int b)
{
    char * result ="ab2";
    result result;
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // puts("Hello, world!");
    // char buffer[2034];

    while (true)
    {

        WaitForInput();
        ProcessInput();
        OutputResult();
    }

    // testing
    char * cmd = "do";
    //RegistItem test(cmd, &Func1);
    RegistItem test(cmd, &Func2);
    test.Test();





    return 0;
}
