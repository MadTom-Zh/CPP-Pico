
#include "CMDInfo.h"
#include "../Dependence/GarbageCollect.h"
#include "../Dependence/StringHelper.h"

using namespace MadTomDev::Base;
using namespace MadTomDev::Common::StringHelper;

ArgInfo *ArgInfo::Clone()
{
    ArgInfo *clone = new ArgInfo;

    clone->argName = string(this->argName);
    clone->argValueStr = string(this->argValueStr);
    clone->argType = this->argType;

    clone->vL = this->vL;
    clone->vD = this->vD;
    clone->vS = string(this->vS);

    return clone;
}

ArgInfo::ArgInfo()
{
}

ArgInfo::~ArgInfo()
{
}

void MadTomDev::Base::FreeVector(vector<ArgInfo *> *argList)
{
    if (!argList->empty())
    {
        int i = argList->size();
        for (--i; i >= 0; --i)
        {
            argList->at(i)->~ArgInfo();
        }
        argList->clear();
    }
}

CMDInfo::CMDInfo(string cmd, void *handler)
{
    this->cmd = cmd;
    this->handler = handler;
}

CMDInfo::~CMDInfo()
{
    cmd.clear();
    FreeVector(&argTplList);
    FreeVector(&inputListCache);
    // free(handler);
    handler = NULL;
}

using namespace MadTomDev::Common::StringHelper;
bool CMDInfo::CheckInput(string &errMsg)
{
    int argTplListSize = argTplList.size();
    int i, iv;
    ArgInfo *newAI;
    if (inputListCache.empty())
    {
        for (i = 0; i < argTplListSize; ++i)
        {
            // newAI = argTplList->at(i)->Clone();
            inputListCache.push_back(argTplList[i]->Clone());
        }
        return true;
    }
    vector<ArgInfo *> tmpArgs;
    tmpArgs.swap(inputListCache);
    //inputListCache.clear();

    // check in pairs
    int j, jv = tmpArgs.size();

    ArgInfo *a1, *a2;
    // 检查是否重复输入了参数标记，如两个相同的 -t
    for (i = 0, iv = jv - 1; i < iv; ++i)
    {
        a1 = tmpArgs[i];
        for (j = i + 1; j < jv; ++j)
        {
            a2 = tmpArgs[j];
            if (StrEqual(a1->argName, a2->argName))
            {
                FreeVector(&tmpArgs);
                errMsg.reserve(64);
                sprintf(&errMsg[0], "Duplicate flag [%s].", a1->argName.c_str());
                return false;
            }
        }
    }

    bool flagNotFound;
    ArgInfo *rawAI, *tplAI;
    for (i = 0, iv = jv; i < iv; ++i)
    {
        // 检查输入的参数标记是否全部和登记时的匹配；
        flagNotFound = true;
        rawAI = tmpArgs[i];
        for (j = 0; j < argTplListSize; ++j)
        {
            //printf("test.\n");
            tplAI = argTplList[j];
            if (StrEqual(rawAI->argName, tplAI->argName))
            {
                flagNotFound = false;
                break;
            }
        }
        if (flagNotFound)
        {
            FreeVector(&tmpArgs);
            errMsg.reserve(64);
            sprintf(&errMsg[0], "Un-recognized flag [%s].", rawAI->argName.c_str());
            return false;
        }
    }

    // 按顺序整理参数，写入到 inputListCache
    jv = tmpArgs.size();
    volatile int testI = jv;
    for (i = 0; i < argTplListSize; ++i)
    {
        newAI = argTplList[i]->Clone();
        for (j = 0; j < jv; ++j)
        {
            rawAI = tmpArgs[j];
            if (StrEqual(rawAI->argName, newAI->argName))
            {
                switch (newAI->argType)
                {
                case 'L':
                    if (!TryPastToLong(rawAI->argValueStr, newAI->vL))
                    {
                        FreeVector(&tmpArgs);
                        errMsg.reserve(32);
                        sprintf(&errMsg[0], "Err convert to long at [%d].", j);
                        FreeVector(&inputListCache);
                        return false;
                    }
                    break;
                case 'D':
                    if (!TryPastToDouble(rawAI->argValueStr, newAI->vD))
                    {
                        FreeVector(&tmpArgs);
                        errMsg.reserve(32);
                        sprintf(&errMsg[0], "Err convert to double at [%d].", j);
                        FreeVector(&inputListCache);
                        return false;
                    }
                default:
                    newAI->vS = string(rawAI->argValueStr);
                    break;
                }
                break;
            }
        }
        inputListCache.push_back(newAI);
    }
    FreeVector(&tmpArgs);
    return true;
}

void CMDInfo::ClearInputCache()
{
    FreeVector(&inputListCache);
}

string CMDInfo::InvokeWithLastArgs()
{
    int parCounts = inputListCache.size();

    string errMsg;
    switch (parCounts)
    {
    case 0:
    {
        HandlerTpls ::Func_S func = (HandlerTpls ::Func_S)handler;
        return func();
    }

    case 1:
    {
        ArgInfo *ai1 = inputListCache[0];
        switch (ai1->argType)
        {
        case 'L':
            return ((HandlerTpls::FuncL_S)handler)(ai1->vL);
        case 'D':
            return ((HandlerTpls::FuncD_S)handler)(ai1->vD);
        case 'S':
            return ((HandlerTpls::FuncS_S)handler)(ai1->vS);

        default:
        {
            errMsg.reserve(32);
            sprintf(&errMsg[0], "Unknow para-type/seq [%c].", ai1->argType);
            throw errMsg;
        }
        }
        // break;
    }

    case 2:
    {
        ArgInfo *ai1 = inputListCache[0];
        ArgInfo *ai2 = inputListCache[1];
        switch (ai1->argType, ai2->argType)
        {
        case ('L', 'L'):
            return ((HandlerTpls::FuncLL_S)handler)(ai1->vL, ai2->vL);

        default:
        {
            errMsg.reserve(32);
            sprintf(&errMsg[0], "Unknow para-type/seq [%c][%c].", ai1->argType, ai2->argType);
            throw errMsg;
        }
        }

        break;
    }
    case 3:
    {
        ArgInfo *ai1 = inputListCache[0];
        ArgInfo *ai2 = inputListCache[1];
        ArgInfo *ai3 = inputListCache[2];
        switch (ai1->argType, ai2->argType, ai3->argType)
        {
        case ('L','D', 'L'):
            return ((HandlerTpls::FuncLDL_S)handler)(ai1->vL,ai2->vD, ai3->vL);

        default:
        {
            errMsg.reserve(32);
            sprintf(&errMsg[0], "Unknow para-type/seq [%c][%c][%c].", ai1->argType, ai2->argType, ai3->argType);
            throw errMsg;
        }
        }

        break;
    }

    default:
    {
        errMsg.reserve(32);
        sprintf(&errMsg[0], "Unknow parameter count [%d].", parCounts);
        throw errMsg;
    }
    }
    /*
        //void * ptr =  &Func1 ;
        //void * ptr =  (void *)(&Func1) ;
        //void * ptr =  static_cast<void*>(&Func1) ;
        void * ptr =  reinterpret_cast<void*>(&Func1) ;
        HandlerTpls :: Func_S  h2 = ( HandlerTpls :: Func_S)ptr;
        result = h2();
        printf("result 2 %s \n",result);
        free(result);
    */

    return NULL;
}