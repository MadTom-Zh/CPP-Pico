#pragma once


#include <stdio.h>
#include <string.h>
#include <iostream>
//#include <map>
#include <vector>

using namespace std;

namespace MadTomDev
{
    namespace Base
    {
        /// @brief 用于转换注册命令所带方法的各种方法定义，按需扩充
        namespace HandlerTpls
        {
            typedef string  (*Func_S)();
            typedef string  (*FuncL_S)(long);
            typedef string  (*FuncD_S)(double);
            typedef string  (*FuncS_S)(string);
            typedef string  (*FuncLL_S)(long, long);
        }

        class ArgInfo
        {
            private:
            public:
            string argName ;
            string argValueStr ;
            /// @brief 'L'-long, 'D'-double, 'S'-string(char*)
            char argType = 'S';

            long vL = 0;
            double vD = 0;
            string vS; 

            ArgInfo * Clone();

            ArgInfo();
            ~ArgInfo();
        };

        void FreeVector(vector<ArgInfo *> * argList);

        class CMDInfo
        {
        private:
            string cmd;
            void *handler;
        public:
            vector<ArgInfo *> argTplList, inputListCache;
            
            CMDInfo(string cmd, void * handler);
            ~CMDInfo();
            bool CheckInput(string & errMsg);
            void ClearInputCache();
            /// @brief 和 HandlerTpls 对应，具体实现需要完全匹配（详细见说明）
            /// @return （按设计）必须返回string
            string InvokeWithLastArgs();
        };
        
    }
}