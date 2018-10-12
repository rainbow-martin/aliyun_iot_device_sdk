/*
 * platform.h
 * Author
 *     martin.wang 2014/2/12
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef WIN32
#include <stddef.h>
#include <windows.h>
#include <winsock.h>
#elif defined(POSIX)

#else
#error "Unkonw platform"
#endif


#endif // !__PLATFORM_H__

