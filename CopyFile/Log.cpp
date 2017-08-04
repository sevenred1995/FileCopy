#include "Log.h"
#include <string.h>
Log* Log::m_pInstance = nullptr;
Log::Log()
{ 
    memset(m_szErrorInfo, 0, 256);
}
bool Log::SetLastErrorInfo(const char * c_szErrorInfo)
{
    if (c_szErrorInfo == nullptr|| strlen(c_szErrorInfo)>=256)
        return false;
    strcpy(m_szErrorInfo, c_szErrorInfo);
    //c_szErrorInfo = nullptr;
    return true;
}

const char * Log::GetLastErrorInfo()
{
    return m_szErrorInfo;
}
