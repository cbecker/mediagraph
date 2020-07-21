#include "thread_platform.h"

#ifdef __linux__
#include <pthread.h>
#endif

namespace media_graph
{
void setThreadName(std::thread& thread, const char* threadName)
{
    if (threadName == nullptr) { return; }

#ifdef __linux__
    pthread_setname_np(thread.native_handle(), threadName);
#else
    static_assert(false, "Platform not supported");
#endif
}
}
