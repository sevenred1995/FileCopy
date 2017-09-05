#include "Log.h"
#include <string.h>
Log* Log::m_pInstance = nullptr;
Log::Log()
{ 
}
Log * Log::getInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new Log();
    return m_pInstance;
}
bool   Log::SetLastErrorInfo(
    const char    *file,
    unsigned long line,
    ERROR_CODE    errorCode, 
    const char    *additionalInfo
    )
{
    memset(m_errorInfo.errorWhereName, 0, 256);
    memset(m_errorInfo.additionalInfo, 0, 256);
    if (strlen(file) > 32)
        return false;
    strcpy(m_errorInfo.errorWhereName, file);
    m_errorInfo.errorWhereLine = line;
    m_errorInfo.errorCode = errorCode;
    if (additionalInfo != NULL)
    {
        if (strlen(additionalInfo) >= 256)
            return false;
        strcpy(m_errorInfo.additionalInfo, additionalInfo);
    }
    return true;
}
ErrorInfo Log::GetLastErrorInfo()
{
    return m_errorInfo;
}

