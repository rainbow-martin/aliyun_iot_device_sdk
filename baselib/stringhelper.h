/*
 * stringhelper.h
 *
 * Author
 *          2014/12/19
 *
 */

#ifndef __STRING_HELPER_H__
#define __STRING_HELPER_H__

#include "types.h"
#include <string>
#include <vector>

class StringHelper
{
public :
    static std::string ValueOf(short val);
    static std::string ValueOf(unsigned short val);
    static std::string ValueOf(int val);
    static std::string ValueOf(unsigned int val);
    static std::string ValueOf(long val);
    static std::string ValueOf(unsigned long val);
    static std::string ValueOf(float val);
    static std::string ValueOf(double val);
    static std::string ValueOf(int64 val);
    static std::string ValueOf(uint64 val);

    static short ShortValue(const char* psz, short nDefault = 0);
    static unsigned short UShortValue(const char* psz, unsigned short nDefault = 0);
    static int IntValue(const char* psz, int nDefault = 0, bool bHex = false);
    static unsigned int UIntValue(const char* psz, unsigned int nDefault = 0);
    static long LongValue(const char* psz, long nDefault = 0);
    static unsigned long ULongValue(const char* psz, unsigned long nDefault = 0);
    static float FloatValue(const char* psz, float fltDefault = 0);
    static double DoubleValue(const char* psz, double dblDefault = 0);
    static int64 Int64Value(const char* psz, int64 nDefault = 0);
    static uint64 UInt64Value(const char* psz, uint64 nDefault = 0);

    // misc helpers
	static std::string TimeString();

    static uint32 GetHashValue(const char* psz);
    static void SafeAssign(std::string& str, const char* psz);
    static std::string SafeWrap(const char* psz);

    static std::string ToHexString(const uint8* pData, uint32 nLength);
};


#endif

