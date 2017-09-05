#ifndef _COPYLOGIC_H
#define _COPYLOGIC_H
#endif // !_COPYLOGIC_H


bool CopyTheFolder(
    const char *sourcePath,
    const char *destPath,
    bool  failIfExists 
);
bool CopyTheFile(
    const char *sourcePath,
    const char *destPath,
    bool  failIfExists
);