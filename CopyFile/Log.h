#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
class Log
{
public:
    static Log  *getInstance()
    {
        if (m_pInstance == nullptr)
            m_pInstance = (Log*)malloc(sizeof(Log));
        return m_pInstance;
    }
public:
    bool         SetLastErrorInfo(const char* c_szErrorInfo);
    const char*  GetLastErrorInfo();
private:
    Log();
    class Garbo
    {
        ~Garbo()
        {
            if (Log::m_pInstance)
                delete Log::m_pInstance;
        }
    };
    static Garbo m_garbo;
    static Log *m_pInstance;
private:
    char m_szErrorInfo[256];
};
