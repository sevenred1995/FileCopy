#pragma once
#include <stdint.h>

typedef void* MappingHandle;

MappingHandle CreateTheFileMapping(
    MappingHandle hFile,
    uint64_t mappingSize,
    bool bTrueIsRead
);

char   *MapViewOfTheFile(
    MappingHandle hMapping,
    uint64_t qwFileOffset,
    uint64_t qwBlockBytes,
    bool bTrueIsRead
);

void   CloseFileMappingHandle(MappingHandle *ppvMapping);

bool   UnmapViewOfTheFile(const char* c_pszData);