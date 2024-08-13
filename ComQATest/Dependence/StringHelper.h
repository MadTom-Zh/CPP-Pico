#pragma once

//#ifndef MADTOMDEV_BASE_STRINGHELPER_H
//#define MADTOMDEV_BASE_STRINGHELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
using namespace std;

namespace MadTomDev
{
    namespace Common
    {
        // using namespace std;
        namespace StringHelper
        {
            int StrLen(const string str);
            bool StrEqual(const string str1, const string str2);
            int IndexOf(const char * str,const char findChar,int startAt = 0, int endAt = __INT_MAX__);

            /// @brief 检查字符串，是否包含开头和结尾空格；
            /// @param value 源串
            /// @return 0-无空格，1-有开头空格，2-有结尾空格，3-开头结尾都有空格；
            int NeedTrim(const string & value);
            int NeedTrim(const char * str);
            string  Trim(const string & value, bool trimStart = true,bool trimEnd = true);
            string Trim(const char * value, bool trimStart = true,bool trimEnd = true);

            /// @brief 將字符串value中，指定範圍内，指定的關鍵詞，全部替換爲另一個關鍵詞
            /// @param value 源串，要替換的串
            /// @param find 關鍵詞
            /// @param replaceWith 新關鍵詞，用於替換原關鍵詞
            /// @param startAt 在value的開始位置，開頭則0；
            /// @param endAt 在value的結束位置，結尾則串長
            /// @return 返回替換關鍵詞后的新串，如aaabcabc將abc替換爲123，開始結束分別爲1 7，則返回aa123abc
            string  ReplaceAll(const string & value,
                             const string & find,
                             const string & replaceWith,
                             int startAt = 0,
                             int endAt = __INT_MAX__);

            /// @brief 用分隔符，将字符串分割为若干片段
            /// @param value 源串，要分割的串
            /// @param spliter 分隔符
            /// @param ignorBlanks 忽略空白片段，默认false不忽略
            /// @param escapeChar 忽略符，如分隔符为a时，遇到\a则跳过
            /// @return 分割后的片段列表
            vector<string>  Split(const string & value, const string & spliter,
                               bool ignorBlanks = false,
                               char escapeChar = '\0');
            vector<string>  Split(const string & value, const char spliter,
                               bool ignorBlanks = false,
                               char escapeChar = '\0');


            /// @brief 将字符串转换为长整型
            /// @param value 源串
            /// @param outValue 输出结果；如果不成功(return false)，则必定为0
            /// @param base 转换基数，0-自动，2-二进制，10-十进制，最高32
            /// @return 转换是否成功
            bool TryPastToLong(const string & value, long &outValue, int base = 10);

            /// @brief 将字符串转换为双精度
            /// @param value 源串
            /// @param outValue 输出结果，因精度关系，可能出现额外小数；如果不成功(return false)，则必定为0
            /// @return 转换是否成功
            bool TryPastToDouble(const string & value, double &outValue);

            /// @brief 将字符串中所有小写字符转换为大写
            /// @param value 源串
            /// @return 转换后的新串
            string  ToUpperCase(const string & value);
            /// @brief 将字符串中所有大写字符转换为小写
            /// @param value 源串
            /// @return 转换后的新串
            string  ToLowerCase(const string & value);

            bool HasChar(const string & value, const char sample);
            bool HasSpace(const string & value);
            bool HasNumber(const string & value);
            bool HasChar(const string & value);
            bool HasSymbol(const string & value);

            enum CharContainTypes
            {
                None = 0,
                Base_LetterLower = 1, // +
                Base_LetterUpper = 2, // +
                LUpper_LLower = 3,
                Base_Number = 4, // +
                LLower_Number = 5,
                LUpper_Number = 6,
                LUpper_LLower_Number = 7,
                Base_Symbol = 8, // +
                LLower_Symbol = 9,
                LUpper_Symbol = 10,
                LUpper_LLower_Symbol = 11,
                Number_Symbol = 12,
                LLower_Number_Symbol = 13,
                LUpper_Number_Symbol = 14,
                LUpper_LLower_Number_Symbol = 15,
                Base_Other = 16, // +
                LLower_Other = 17,
                LUpper_Other = 18,
                LUpper_LLower_Other = 19,
                Number_Other = 20,
                LLower_Number_Other = 21,
                LUpper_Number_Other = 22,
                LUpper_LLower_Number_Other = 23,
                Symbol_Other = 24,
                LLower_Symbol_Other = 25,
                LUpper_Symbol_Other = 26,
                LUpper_LLower_Symbol_Other = 27,
                Number_Symbol_Other = 28,
                LLower_Number_Symbol_Other = 29,
                LUpper_Number_Symbol_Other = 30,
                LUpper_LLower_Number_Symbol_Other = 31,
            };

            /// @brief 检查字符串中都含有哪些字符类型，大写、小写、数字、符号、其他
            /// @param value 源串
            /// @return 检查结果，如要判断是否包含特定类型，可以result & [base_指定类型]
            CharContainTypes CheckCharContainTypes(const string & value);

        };
    }
}
//#endif
