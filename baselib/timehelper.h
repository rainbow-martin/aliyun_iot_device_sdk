/*
 * timehelper.h
 * From libjingle
 * Author 
 *     Martin.Wang    2013/10/21
 */

#ifndef __TIMER_HELPER_H__
#define __TIMER_HELPER_H__

#include "types.h"
#include "logging.h"
#define EFFICIENT_IMPLEMENTATION 1

const uint32 LAST = 0xFFFFFFFF;
const uint32 HALF = 0x80000000;

/////////////////////////////////////////////////////////////////////////////////////////////
// TimeHelper
// 1 second = 1000 millisecond = 1000,000 microsecond
class TimeHelper{
public:
	static uint32 Time();
	static uint64 TimeInMicros();

static uint32 StartTime() {
  // Close to program execution time
  static const uint32 g_start = Time();
  return g_start;
}

static uint32 TimeAfter(int32 elapsed) {
  ASSERT(elapsed >= 0);
  ASSERT(static_cast<uint32>(elapsed) < HALF);
  return Time() + elapsed;
}

static bool TimeIsBetween(uint32 earlier, uint32 middle, uint32 later) {
  if (earlier <= later) {
    return ((earlier <= middle) && (middle <= later));
  } else {
    return !((later < middle) && (middle < earlier));
  }
}

bool TimeIsLaterOrEqual(uint32 earlier, uint32 later) {
#if EFFICIENT_IMPLEMENTATION
  int32 diff = later - earlier;
  return (diff >= 0 && static_cast<uint32>(diff) < HALF);
#else
  const bool later_or_equal = TimeIsBetween(earlier, later, earlier + HALF);
  return later_or_equal;
#endif
}

bool TimeIsLater(uint32 earlier, uint32 later) {
#if EFFICIENT_IMPLEMENTATION
  int32 diff = later - earlier;
  return (diff > 0 && static_cast<uint32>(diff) < HALF);
#else
  const bool earlier_or_equal = TimeIsBetween(later, earlier, later + HALF);
  return !earlier_or_equal;
#endif
}

static int32 TimeDiff(uint32 later, uint32 earlier) {
#if EFFICIENT_IMPLEMENTATION
  return later - earlier;
#else
  const bool later_or_equal = TimeIsBetween(earlier, later, earlier + HALF);
  if (later_or_equal) {
    if (earlier <= later) {
      return static_cast<long>(later - earlier);
    } else {
      return static_cast<long>(later + (LAST - earlier) + 1);
    }
  } else {
    if (later <= earlier) {
      return -static_cast<long>(earlier - later);
    } else {
      return -static_cast<long>(earlier + (LAST - later) + 1);
    }
  }
#endif
}
};
#endif

