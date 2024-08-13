

#include "StringHelper.h"
using namespace MadTomDev::Common;

int StringHelper::StrLen(const string str)
{
    if(str.length() == 0)
    {
        return 0;
    }
    char curC;
    int i =0;
    while(true)
    {
        curC = str[i++];
        if(curC == '\0')
        {
            break;
        }
    }
    return --i;
}
bool StringHelper::StrEqual(const string str1, const string str2)
{
    int str1Len = StrLen(str1);
    int str2Len = StrLen(str2);
    if(str1Len != str2Len)
    {
        return false;
    }
    for(int i=0;i<str1Len;++i)
    {
        if(str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}
int StringHelper::IndexOf(const char *str, const char findChar, int startAt, int endAt)
{
    int strLength = strlen(str);
    if (strLength == 0)
    {
        return -1;
    }
    for (int i = startAt; i < strLength && i < endAt; ++i)
    {
        if (str[i] == findChar)
        {
            return i;
        }
    }
    return -1;
}

int StringHelper::NeedTrim(const string &value)
{
    // 0-无空格，1-有开头空格，2-有结尾空格，3-开头结尾都有空格；
    int vLength = StrLen( value);
    if (vLength == 0)
    {
        return 0;
    }

    bool hasStartSpace = value[0] == ' ',
         hasEndSpace = value[vLength - 1] == ' ';
    if (hasStartSpace && hasEndSpace)
    {
        return 3;
    }
    else if (hasStartSpace)
    {
        return 1;
    }
    else if (hasEndSpace)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

int StringHelper::NeedTrim(const char *str)
{
    // 0-无空格，1-有开头空格，2-有结尾空格，3-开头结尾都有空格；
    int vLength = strlen(str);
    if (vLength == 0)
    {
        return 0;
    }

    bool hasStartSpace = str[0] == ' ',
         hasEndSpace = str[vLength - 1] == ' ';
    if (hasStartSpace && hasEndSpace)
    {
        return 3;
    }
    else if (hasStartSpace)
    {
        return 1;
    }
    else if (hasEndSpace)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

string StringHelper::Trim(const string &value, bool trimStart, bool trimEnd)
{
    // idxStart 和 idxEnd分别定位第一个 非空格 字符和最后一个 非空格 字符的位置
    int idxEnd = StrLen(value);
    if (idxEnd == 0)
    {
        string blankStr;
        return blankStr;
    }

    int idxStart = 0;
    int i = 0;
    char curC;
    if (trimStart)
    {
        curC = value[i++];
        while (curC != '\0')
        {
            if (curC == ' ')
            {
                curC = value[i++];
            }
            else
            {
                idxStart = i - 1;
                break;
            }
        }
    }
    if (trimEnd)
    {
        i = idxEnd - 1;
        curC = value[i--];
        while (i > idxStart)
        {
            if (curC == ' ')
            {
                curC = value[i--];
            }
            else
            {
                idxEnd = i + 2;
                break;
            }
        }
    }
    if (idxEnd == idxStart)
    {
        string blankStr;
        return blankStr;
    }
    string result(value, idxStart, idxEnd - idxStart);
    return result;
}
string StringHelper::Trim(const char *str, bool trimStart, bool trimEnd)
{
    string vStr(str);
    return Trim(vStr, trimStart, trimEnd);
}

string StringHelper::ReplaceAll(const string &value,
                                const string &find,
                                const string &replaceWith,
                                int startAt,
                                int endAt)
{
    int vLength = StrLen(value);
    if (vLength == 0)
    {
        string blankStr;
        return blankStr;
    }
    int fLength = StrLen(find);
    if (fLength == 0)
    {
        string result(value);
        return result;
    }

    vector<int> idxList;
    int i = startAt;
    int iv;
    if (endAt < vLength)
    {
        iv = endAt - fLength;
    }
    else
    {
        iv = vLength - fLength;
    }
    char findFirstChar = find[0];
    int j, cpyIdx = 0;
    bool fullMatch;
    while (i <= iv)
    {
        if (value[i] == findFirstChar)
        {
            fullMatch = true;
            for (j = 1; j < fLength; ++j)
            {
                if (value[i + j] != find[j])
                {
                    fullMatch = false;
                    break;
                }
            }
            if (fullMatch == true)
            {
                // 找到匹配詞

                // 將不含查找詞的語句入列；
                idxList.push_back(cpyIdx);
                idxList.push_back(i);

                // 位置索引推進；
                i += fLength;
                cpyIdx = i;
            }
            else
            {
                // 不能完全匹配

                // 位置索引推進；
                i += j;
            }
        }
        else
        {
            ++i;
        }
    }

    idxList.push_back(cpyIdx);
    idxList.push_back(vLength);

    // 按照標記位置，重新組裝字符串；
    // 計算新字符串的長度
    iv = idxList.size() / 2;
    int rLength = StrLen(replaceWith);
    int resultLength = (iv - 1) * rLength;
    int k;
    for (i = 0; i < iv; ++i)
    {
        resultLength += (idxList[i + i + 1] - idxList[i + i]);
    }
    resultLength += 1; // '\0'

    // 複製内容到新串
    string result(resultLength, '\0');
    int ir = 0, m;
    for (i = 0; i < iv; ++i)
    {
        // 將不含關鍵字的串，複製到結果；
        j = idxList[i + i];
        k = idxList[i + i + 1];

        for (; j < k; ++j)
        {
            result[ir++] = value[j];
        }
        if (i < iv - 1)
        {
            // 寫入替換串
            for (j = 0; j < rLength; ++j)
            {
                result[ir++] = replaceWith[j];
            }
        }
    }
    idxList.clear();
    return result;
}

vector<string> StringHelper::Split(const string &value, const string &spliter,
                                   bool ignorBlanks,
                                   char escapeChar)
{
    vector<string> result;
    int vLength = StrLen(value);
    if (vLength == 0)
    {
        string blank(1, '\0');
        result.push_back(blank);
        return result;
    }
    int sLength = StrLen(spliter);
    int sizeOfChar = sizeof(char);
    if (sLength == 0)
    {
        string item(value);
        result.push_back(item);
        return result;
    }

    int i = 0, j, k, idxCpy = 0;
    char curC, firstS = spliter[0];
    bool fullMatch, isBlank, addBlank;
    while (i < vLength)
    {
        curC = value[i];
        if (curC == escapeChar)
        {
            i += 2;
            continue;
        }

        if (curC == firstS)
        {
            fullMatch = true;
            if (sLength > 1)
            {
                for (j = 1; j < sLength; ++j)
                {
                    if (value[i + j] != spliter[j])
                    {
                        fullMatch = false;
                        break;
                    }
                }
            }
            if (fullMatch)
            {
                isBlank = (i == idxCpy);
                // 将分隔符之前的串保存到数组
                if (isBlank)
                {
                    if (ignorBlanks == false)
                    {
                        // 加入空串 (xx, 加入NULL)；
                        string blank(1, '\0');
                        result.push_back(blank);
                    }
                }
                else
                {
                    // 加入指针之前的串；
                    string item(i - idxCpy + 1, '\0');
                    for (j = idxCpy, k = 0; j < i; ++j, ++k)
                    {
                        item[k] = value[j];
                    }
                    result.push_back(item);
                }
                i += sLength;
                idxCpy = i;
            }
            else
            {
                i += j;
            }
        }
        else
        {
            ++i;
        }
    }
    if (idxCpy < vLength)
    {
        string item(vLength - idxCpy + 1, '\0');
        for (j = idxCpy, k = 0; j < vLength; ++j, ++k)
        {
            item[k] = value[j];
        }
        result.push_back(item);
    }
    return result;
}
vector<string> StringHelper::Split(const string &value, const char spliter,
                                   bool ignorBlanks,
                                   char escapeChar)
{
    string str(2, '\0');
    str[0] = spliter;
    vector<string> result = MadTomDev::Common::StringHelper::Split(value, str, ignorBlanks, escapeChar);

    return result;
}

bool StringHelper::TryPastToLong(const string &value, long &outValue, int base)
{
    try
    {
        outValue = stol(value, 0, base);
        return true;
    }
    catch (...)
    {
        return false;
    }
}
bool StringHelper::TryPastToDouble(const string &value, double &outValue)
{
    try
    {
        outValue = stod(value, 0);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

string StringHelper::ToUpperCase(const string &value)
{
    int vLength = value.length();
    string result(vLength + 1, '\0');
    for (int i = 0; i < vLength; ++i)
    {
        result[i] = toupper(value[i]);
    }
    return result;
}
string StringHelper::ToLowerCase(const string &value)
{
    int vLength = value.length();
    string result(vLength + 1, '\0');
    for (int i = 0; i < vLength; ++i)
    {
        result[i] = tolower(value[i]);
    }
    return result;
}

bool StringHelper::HasChar(const string &value, const char sample)
{
    return (value.find(sample) != std::string::npos);
}
bool StringHelper::HasSpace(const string &value)
{
    return HasChar(value, ' ');
}
bool StringHelper::HasNumber(const string &value)
{
    int vLength = value.length();
    if (vLength == 0)
    {
        return false;
    }
    char curChar;
    for (int i = 0; i < vLength; ++i)
    {
        curChar = value[i];
        if ('0' <= curChar && curChar <= '9')
        {
            return true;
        }
    }
    return false;
}
bool StringHelper::HasChar(const string &value)
{
    int vLength = value.length();
    if (vLength == 0)
    {
        return false;
    }
    char curChar;
    for (int i = 0; i < vLength; ++i)
    {
        curChar = value[i];
        if (('A' <= curChar && curChar <= 'Z') || ('a' <= curChar && curChar <= 'z'))
        {
            return true;
        }
    }
    return false;
}
bool StringHelper::HasSymbol(const string &value)
{
    int vLength = value.length();
    if (vLength == 0)
    {
        return false;
    }
    char curChar;
    for (int i = 0; i < vLength; ++i)
    {
        curChar = value[i];
        if ((33 <= curChar && curChar <= 47) || (58 <= curChar && curChar <= 64) || (91 <= curChar && curChar <= 96) || (123 <= curChar && curChar <= 126))
        {
            return true;
        }
    }
    return false;
}

StringHelper::CharContainTypes StringHelper::CheckCharContainTypes(const string &value)
{
    if (value.length() == 0)
    {
        return CharContainTypes::None;
    }

    bool hasLL = false, hasLU = false,
         hasNum = false, hasSym = false,
         hasOther = false;

    char curChar;
    int i = 0;
    while (true)
    {
        curChar = value[i++];
        if (curChar == '\0')
        {
            break;
        }
        if ('a' <= curChar && curChar <= 'z')
        {
            if (hasLL == false)
                hasLL = true;
        }
        else if ('A' <= curChar && curChar <= 'Z')
        {
            if (hasLU == false)
                hasLU = true;
        }
        else if ('0' <= curChar && curChar <= '9')
        {
            if (hasNum == false)
                hasNum = true;
        }
        else if ((33 <= curChar && curChar <= 47) || (58 <= curChar && curChar <= 64) || (91 <= curChar && curChar <= 96) || (123 <= curChar && curChar <= 126))
        {
            if (hasSym == false)
                hasSym = true;
        }
        else if (hasOther == false)
        {
            hasOther = true;
        }
    }

    int resultCode = 0;
    if (hasLL)
        resultCode += 1;
    if (hasLU)
        resultCode += 2;
    if (hasNum)
        resultCode += 4;
    if (hasSym)
        resultCode += 8;
    if (hasOther)
        resultCode += 16;

    return (CharContainTypes)resultCode;
}
