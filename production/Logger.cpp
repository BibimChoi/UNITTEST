#include "Logger.h"

namespace Firmware {

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

void Logger::Log(LogLevel level, const char* msg) {
    if (level == LogLevel::ERROR) {
        errorCount_++;
    }
    // 실제론 UART로 전송, Flash에 기록 등
    (void)msg;
}

void Logger::LogError(const char* msg) {
    Log(LogLevel::ERROR, msg);
}

void Logger::LogInfo(const char* msg) {
    Log(LogLevel::INFO, msg);
}

uint32_t Logger::GetErrorCount() const {
    return errorCount_;
}

} // namespace Firmware
