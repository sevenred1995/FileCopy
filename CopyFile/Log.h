#pragma once
#define _CRT_SECURE_NO_WARNINGS

typedef enum E_ERROR_CODE
{
    C_FILE_NOEXIST,
    C_FILE_EXIST,
    C_CREATE_FAILED,
    C_OPEN_FAILED,
    C_CREAT_MAPPING_FAILED,
    C_MAPPING_FILE_FAILED,
    C_MEMCPY_ERROR,
    C_FINDFIREST_ERROR,
    C_PATHSTR_OUT_RANGE
}ERROR_CODE;

typedef struct ERROR_INFO_STRUCT 
{
    char          errorWhereName[32];
    unsigned long errorWhereLine;
    ERROR_CODE    errorCode;
    char          additionalInfo[256];
}ErrorInfo;
class Log
{
public:
    static Log  *getInstance();
public:
    bool         SetLastErrorInfo(
        const char    *file,
        unsigned long line,
        ERROR_CODE    errorCode,
        const char    *additionalInfo
    );
    ErrorInfo  GetLastErrorInfo();
private:
    Log();
    static Log  *m_pInstance;
    class  Garbo
    {
    public:
        ~Garbo()
        {
            if (Log::m_pInstance)
            {
                delete Log::m_pInstance;
            }    
        }
    };
    static Garbo m_garbo;
private:
    ErrorInfo m_errorInfo;
};
