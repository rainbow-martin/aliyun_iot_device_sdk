/*
 * types.h
 * 
 * Author 
 *     Martin.Wang    2012/11/09
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#include "platform.h"

typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef unsigned long long uint64;
typedef long long int64;

#ifdef POSIX
typedef int SOCKET;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#ifndef INFINITE
#define INFINITE	uint32(-1)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET	(SOCKET)(~0)
#endif


#endif //!__TYPES_H__
