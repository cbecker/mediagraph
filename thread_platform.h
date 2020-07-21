#pragma once

#include <thread>

namespace media_graph
{
void setThreadName(std::thread& thread, const char* threadName);
}
