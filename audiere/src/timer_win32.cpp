#include <windows.h>
#include <mmsystem.h>
#include "timer.h"


namespace audiere {

  u64 GetNow() {
    // get frequency of the performance counter
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency) && frequency.QuadPart != 0) {
      LARGE_INTEGER now;
      if (QueryPerformanceCounter(&now)) {
        return 1000000 * u64(now.QuadPart) / frequency.QuadPart;
      }
    }

    // no performance counter, so use the Win32 multimedia timer
    return timeGetTime() * 1000;
  }

}
