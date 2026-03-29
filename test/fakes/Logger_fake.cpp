#include "Logger.h"
#include "MockLogger.h"

namespace Firmware {

static MockLogger* g_mockLogger = nullptr;

void SetMockLogger(MockLogger* mock) {
    g_mockLogger = mock;
}

// ★ 포인터 반환
Logger* Logger::GetInstance() {
    return g_mockLogger;
}

void Logger::Log(LogLevel level, const char* msg) { (void)level; (void)msg; }
void Logger::LogError(const char* msg) { (void)msg; }
void Logger::LogInfo(const char* msg) { (void)msg; }
uint32_t Logger::GetErrorCount() const { return 0; }

} // namespace Firmware
