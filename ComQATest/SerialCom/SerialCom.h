

// last update 2024 0807 1218
// by MadTom
//



#pragma once


#include <stdio.h>
#include <string.h>
#include <iostream>


#include "CMDInfo.h"
#include <map>

using namespace std;


namespace MadTomDev
{
    namespace Base
    {
        class SerialCom
        {
        private:
            int bufferInputLength;
            int bufferOutputLength;
            char *bufferInput;
            char *bufferOutput;

        protected:
            map<string, CMDInfo *> handlerMap;            

        public:
            /// @brief 初始化串口通信对象，注意使用输入等待时会暂停线程；
            /// @param inputBufferLength 接收指令的缓冲区大小
            /// @param outputBufferLength 输出结果的缓冲区大小
            SerialCom(int inputBufferLength = 256, int outputBufferLength = 256);
            ~SerialCom();
            /// @brief 注册命令（含参数，命令不可重复），命令和对应方法名称建议一致（可以不一致）
            /// @param cmdWithArgs 带参数的命令，如 CalAdd -a D -b D （详细见说明）
            /// @param handler 对应方法（输入参数和注册参数一一对应）
            void RegisterCMD(const string &cmdWithArgs, void * handler);
            /// @brief 注销命令（只需要命令，无需参数）
            /// @param cmd 要注销的命令
            /// @return 是否成功注销
            bool UnRegisterCMD(const string &cmd);
            /// @brief 是否存在注册的命令
            /// @param cmd 要检查的命令
            /// @return 是否存在
            bool HasCMD(const string &cmd);
            /// @brief 手动输入一跳指令；随后再执行ProcessNOutput()，模拟用户输入；
            /// @param userCMD 
            void SetCMD(const string &userCMD);
            /// @brief 等待用户输入，结果将存入 bufferInput
            void WaitForInput();
            /// @brief 检查输入并执行对应方法，输出执行结果
            void ProcessNOutput();
        };
    }
}
