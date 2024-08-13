

#include "SerialCom.h"
#include "../Dependence/StringHelper.h"
#include "../Dependence/GarbageCollect.h"

using namespace MadTomDev::Base;

SerialCom::SerialCom(int inputBufferLength, int outputBufferLength)
{
    bufferInputLength = inputBufferLength;
    bufferOutputLength = inputBufferLength;
    bufferInput = new char[inputBufferLength];
    bufferOutput = new char[outputBufferLength];
    // sscanf(bufferInputFormat, "%%ds", inputBufferLength); // "%512s";
}
SerialCom::~SerialCom()
{
    if (bufferInput != NULL)
    {
        delete[] bufferInput;
        bufferInput = NULL;
    }
    if (bufferOutput != NULL)
    {
        delete[] bufferOutput;
        bufferOutput = NULL;
    }
    for (auto it = handlerMap.begin(), itv = handlerMap.end(); it != itv; ++it)
    {
        it->second->~CMDInfo();
        it->second = NULL;
    }
    handlerMap.clear();
}

using namespace MadTomDev::Common;
void _RemoveFrontSlash(string &argFlag)
{
    int i = 0, j = 0, iv = StringHelper::StrLen(argFlag);
    char curC;
    for (; i < iv; ++i)
    {
        curC = argFlag[i];
        if (curC == ' ' || curC == '-' || curC == '/')
        {
            continue;
        }
        else
        {
            break;
        }
    }
    if (i == 0)
    {
        return;
    }
    for (; j < iv; ++j)
    {
        argFlag[j] = argFlag[i++];
        if (i >= iv)
        {
            argFlag[j + 1] = '\0';
            break;
        }
    }
}
void _SetArgList(vector<ArgInfo *> *argList, const string &argStr)
{
    argList->clear();
    // 先用引号将字符串参数分割出来

    // 分离其他参数
    vector<string> strList;
    volatile int i, iv, j, jv;
    string subStr;
    if (StringHelper::HasChar(argStr, '\"'))
    {
        vector<string> tmpList = StringHelper::Split(argStr, '\"');
        vector<string> subList;
        for (i = 0, iv = tmpList.size(); i < iv; ++i)
        {
            if (i % 2 == 0)
            {
                subList = StringHelper::Split(tmpList[i], ' ', true);

                for (j = 0, jv = subList.size(); j < jv; ++j)
                {
                    strList.push_back(subList[j]);
                }
                subList.clear();
            }
            else
            {
                strList.push_back(tmpList[i]);
            }
        }
        tmpList.clear();
    }
    else
    {
        strList = StringHelper::Split(argStr, ' ', true);
    }

    // 去掉前面的 - 或 /
    int strListSize = strList.size();
    for (i = 0; i < strListSize; i += 2)
    {
        subStr = strList[i];
        _RemoveFrontSlash(subStr);
        strList[i] = subStr;
    }

    iv = strListSize;
    if (iv % 2 != 0)
    {
        string errMsg("Arg-value not in pairs.");
        throw errMsg;
    }

    // strList to ArgInfoList
    ArgInfo *newAI;
    for (i = 0, iv -= 1; i < iv; i += 2)
    {
        newAI = new ArgInfo;
        newAI->argName = strList[i];
        newAI->argValueStr = strList[i + 1];
        argList->push_back(newAI);
    }
}
bool _CheckSetArgTplList(vector<ArgInfo *> *argList, string &errMsg)
{
    int argListSize = argList->size();
    int i, j, iv;
    string tmpStr, tmpStr2;
    // 检查是否出现了重复的标记
    for (i = 0, iv = argListSize - 1; i < iv; ++i)
    {
        tmpStr = argList->at(i)->argName;
        for (j = i + 1; j < argListSize; ++j)
        {
            tmpStr2 = argList->at(j)->argName;
            if (tmpStr.compare(tmpStr2) == 0)
            {
                errMsg.reserve(32);
                sprintf(&errMsg[0], "Duplicated argFlag [%s]", tmpStr.c_str());
                return false;
            }
        }
    }
    ArgInfo *curAI;
    for (i = 0; i < argListSize; ++i)
    {
        // 参数标记
        curAI = argList->at(i);
        tmpStr = curAI->argName;
        if (tmpStr[0] == '\0')
        {
            errMsg.reserve(32);
            sprintf(&errMsg[0], "Invalid argFlag at [%d]", i);
            return false;
        }

        // 数据类型 L D S
        tmpStr2 = curAI->argValueStr;
        char c = tmpStr2[0] ;
        if (c != 'L' && c != 'D' && c != 'S')
        {
            errMsg.reserve(48);
            sprintf(&errMsg[0], "Invalid argType at [%d], support only \"L, D, S\"", i);
            return false;
        }
        else
        {
            curAI->argType = c;
        }
    }
    return true;
}

void SerialCom::RegisterCMD(const string &cmdWithArgs, void *handler)
{
    // 檢查命令模板，滿足下面格式
    // 命令 参数类型 参数标记1 参数类型 参数标记2 ....
    // 其中，至少要有命令，參數類型和標記必須成對出現；
    // 命令不可重複，參數標記不可重複
    // 樣例 DoWork   -t L   -m S
    // L-long  D-double  S-char*
    // 區分大小寫， -t 和 -T將會是兩個不同的參數；

    if (handler == NULL)
    {
        throw "Handler is NULL.";
    }

    string cmdWithArgs_trimed = StringHelper::Trim(cmdWithArgs);
    int cmdWithArgs_trimed_length = cmdWithArgs_trimed.length();
    if (cmdWithArgs_trimed_length == 0)
    {
        throw "Arg [cmdWithArgs] is NULL or empty string.";
    }

    int idxC1 = cmdWithArgs.find(' ');
    string cmd;
    if (idxC1 > 0)
    {
        cmd = string(cmdWithArgs_trimed, 0, idxC1);
    }
    else
    {
        cmd = string(cmdWithArgs_trimed);
    }

    auto test = handlerMap.find(cmd);
    if (handlerMap.size() > 0 && test != handlerMap.end())
    {
        string msg;
        msg.reserve(64);
        sprintf(&msg[0], "A command [%s] has been registered.", cmd.c_str());

        throw msg;
    }
    if (idxC1 > 1)
    {
        // with args
        CMDInfo *newCmdInfo = new CMDInfo(cmd, handler);
        _SetArgList(&(newCmdInfo->argTplList), string(cmdWithArgs_trimed, idxC1 + 1, cmdWithArgs_trimed_length - idxC1 - 1));

        // check args
        string errMsg;
        if (!_CheckSetArgTplList(&(newCmdInfo->argTplList), errMsg))
        {
            delete newCmdInfo;
            throw errMsg;
        }

        //if(newCmdInfo->argTplList.size() > 0)
        //{
        //    printf("tpl 0 argName [%s].\n",newCmdInfo->argTplList[0]->argName.c_str());
        //}

        //printf("arg tpl list size: %d\n",newCmdInfo->argTplList.size());

        // all okay, register
        // CMDInfo newCMD(cmd, &argTplList, handler);
        handlerMap[cmd] = newCmdInfo;
    }
    else
    {
        // no args, register
        // CMDInfo newCMD(cmd, NULL, handler);
        // handlerMap[cmd] = &newCMD;
        // vector<ArgInfo *> emptyList;
        // handlerMap[cmd] = new CMDInfo(cmd, emptyList, handler);
        handlerMap[cmd] = new CMDInfo(cmd, handler);
    }

    // free(cmd);
}
bool SerialCom::UnRegisterCMD(const string &cmd)
{
    auto test = handlerMap.find(cmd);
    if (test != handlerMap.end())
    {
        // found, remove
        CMDInfo *toDel = handlerMap[cmd];
        handlerMap.erase(cmd);
        toDel->~CMDInfo();
        // toDel = NULL;
        return true;
    }
    return false;
}
bool SerialCom::HasCMD(const string &cmd)
{
    return (handlerMap.find(cmd) != handlerMap.end());
}

void SerialCom::SetCMD(const string &userCMD)
{
}
void SerialCom::WaitForInput()
{
    // scanf(bufferInputFormat, bufferInput);
    //  获取整行
    while (true)
    {
        fflush(stdin);
        scanf("%[^\n]", bufferInput);

        int inStrLen = strlen(bufferInput);
        if (inStrLen == 0)
        {
            continue;
        }
        if (StringHelper::NeedTrim(bufferInput) != 0)
        {
            string inStrTrimed = StringHelper::Trim(bufferInput);
            int inStrTrimedLen = inStrTrimed.length();
            if (inStrTrimedLen > 0)
            {
                if (inStrLen != inStrTrimedLen)
                {
                    for (int i = 0; i < inStrTrimedLen; ++i)
                    {
                        bufferInput[i] = inStrTrimed[i];
                    }
                    bufferInput[inStrTrimedLen] = '\0';
                }
                break;
            }
        }
        else
        {
            break;
        }
    }
}
void SerialCom::ProcessNOutput()
{
    // bufferInput

    int idxC1 = StringHelper::IndexOf(bufferInput, ' ');
    string cmd;
    if (idxC1 > 0)
    {
        cmd = string(bufferInput, 0, idxC1);
    }
    else
    {
        cmd = string(bufferInput);
    }
    auto test = handlerMap.find(cmd);

    if (test == handlerMap.end())
    {
        string msg;
        msg.reserve(64);
        sprintf(&msg[0], "Command [%s] not found in register list.", cmd.c_str());
        throw msg;
    }
    CMDInfo *CMD = test->second;

    if (idxC1 > 1)
    {
        // with args
        _SetArgList(&(CMD->inputListCache), bufferInput + idxC1 + 1);

        //printf("Cmd[], input list cache size [%d].\n", CMD->inputListCache.size());
    }
    else
    {
        // no args, invoke, with no arg
    }

    // check args
    string errMsg;
    if (!CMD->CheckInput(errMsg))
    {
        CMD->ClearInputCache();
        throw errMsg;
    }
    //printf("user input list size [%d].\n",CMD->inputListCache.size());

    // all okay, invoke
    string result = CMD->InvokeWithLastArgs();
    printf("%s\n", result.c_str());
    // printf("%s\n", CMD->InvokeWithLastArgs());
}
