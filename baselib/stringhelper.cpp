/*
 * stringhelper.cpp
 *
 * Author
 *          2014/12/19
 *
 */

#include "types.h"
#include "stringhelper.h"
#include <string.h>

#include <sstream>
#include <strstream>
#include <iomanip>
#include <time.h>

//static const char* s_pszHexChars = "0123456789ABCDEF";
static const char* s_pszHexChars = "0123456789abcdef";
static uint8 Char2Hex(char c)
{
	uint8 nChar = 0;

	if( c >= 'A' && c <= 'F' )
		nChar = c - 'A' + 10;
	else if( c >= 'a' && c <= 'f' )
		nChar = c - 'a' + 10;
	else if( c >= '0' && c <= '9' )
		nChar = c - '0';
	else
		nChar = 0;

	return nChar;
}

/////////////////////////////////////////////////////////////////////////////
// StringHelper
//
std::string StringHelper::ValueOf(short val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(unsigned short val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(int val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(unsigned int val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(long val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(unsigned long val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(float val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(double val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(int64 val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

std::string StringHelper::ValueOf(uint64 val)
{
    std::ostringstream os;
    os << val;// << std::ends;
    return std::string(os.str());
}

short StringHelper::ShortValue(const char* psz, short nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    short val;
    is >> val;
    return val;
}

unsigned short StringHelper::UShortValue(const char* psz, unsigned short nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    unsigned short val;
    is >> val;
    return val;
}

int StringHelper::IntValue(const char* psz, int nDefault, bool bHex)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    int val;
	if( bHex )
	    is >> std::hex >> val;
	else
		is >> val;

    return val;
}

unsigned int StringHelper::UIntValue(const char* psz, unsigned int nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    unsigned int val;
    is >> val;
    return val;
}

long StringHelper::LongValue(const char* psz, long nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    long val;
    is >> val;
    return val;
}

unsigned long StringHelper::ULongValue(const char* psz, unsigned long nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    unsigned long val;
    is >> val;
    return val;
}

float StringHelper::FloatValue(const char* psz, float fltDefault)
{
    if(psz == NULL)
        return fltDefault;

    std::istrstream is(psz);
    float val;
    is >> val;
    return val;
}

double StringHelper::DoubleValue(const char* psz, double dblDefault)
{
    if(psz == NULL)
        return dblDefault;

    std::istrstream is(psz);
    double val;
    is >> val;
    return val;
}

int64 StringHelper::Int64Value(const char* psz, int64 nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    int64 val;
    is >> val;
    return val;
}

uint64 StringHelper::UInt64Value(const char* psz, uint64 nDefault)
{
    if(psz == NULL)
        return nDefault;

    std::istrstream is(psz);
    uint64 val;
    is >> val;
    return val;
}

uint32 StringHelper::GetHashValue(const char* psz)
{
    if(psz == NULL || strlen(psz) == 0)
        return 0;

    uint8 abChecksum[sizeof(uint32)];
    memset(abChecksum, 0, sizeof(abChecksum));

    for(int i = 0; i < (int)strlen(psz); i++)
        abChecksum[i % sizeof(uint32)] ^= psz[i];

    return ((uint32)abChecksum[3] << 24)  | 
            ((uint32)abChecksum[2] << 16) | 
            ((uint32)abChecksum[1] << 8)  |
            ((uint32)abChecksum[0]);
}

void StringHelper::SafeAssign(std::string& str, const char* psz)
{
    if(psz)
        str = psz;
    else
        str.clear();
}

std::string StringHelper::SafeWrap(const char* psz)
{
    if(psz)
        return std::string(psz);

    return std::string("");
}

std::string StringHelper::ToHexString(const uint8* pData, uint32 nLength)
{
    if(pData != NULL && nLength > 0)
    {
        std::ostringstream os;
        for(uint32 i = 0; i < nLength; i++)
        {
            os << s_pszHexChars[pData[i] >> 4];
            os << s_pszHexChars[pData[i] & 0xf];
        }
        // os << std::ends;
        return std::string(os.str());
    }
    return std::string("");
}

std::string StringHelper::TimeString()
{
	std::ostringstream oss;
	time_t t = ::time(NULL);
	struct tm* tm1 = localtime(&t);
	oss << tm1->tm_year+1900 <<"-"<< std::setfill('0') << std::setw(2) << tm1->tm_mon+1
		<<"-"<< std::setw(2) << tm1->tm_mday << " " << std::setw(2) << tm1->tm_hour
		<<":" << std::setw(2) << tm1->tm_min << ":" << std::setw(2) << tm1->tm_sec;
	return oss.str();
}


