#ifndef _FILEUTIL_H
#define _FILEUTIL_H

#include <stdint.h>

typedef void* Handle;

#define INVALID_MAPPING_HANDLE_VALUE ((Handle)(__int64)-1)

/*create file mapping with read only,we can get the mapping handle 
 *and the file size.if return value is NULL,the function fails.
*/
Handle CreateFileMappingWithReadMode(
    const char *filePath,
    uint64_t   *outFilesize
);

Handle CreateFileMappingWithWriteMode(
    const char *flePath,
    uint64_t    inFilemappsize
);

bool   CloseMappingHandle(Handle handle);


void *MapViewOfFileingWithReadMode(
    Handle mappHandle, 
    uint64_t offset, 
    uint64_t size
);
void *MapViewOfFileingWithWriteMode(
    Handle mappHandle,
    uint64_t  offset,
    uint64_t  size
);

bool UnmapViewOfTheFile(
    void *dataBuffer
);

#endif 