#include <HTTPClient.h>

#ifndef __JSONDECODE_H__
#define __JSONDECODE_H__

typedef struct temp
{
    float t0;
    float t1;
    float t2;
    float t3;
} Temp;

void decode(Temp *temp);
#endif