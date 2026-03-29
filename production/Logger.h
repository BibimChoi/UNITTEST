#pragma once
#include "HalTypes.h"
#include <cstdint>

// Logger - 또 다른 Singleton
// 실제론 UART, Flash 드라이버 등 의존성이 딸려옴
namespace Firmware {

enum class LogLevel : uint8_t {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
};

class Logger {
public:
    static Logger* GetInstance();

    virtual ~Logger() = default;
    virtual void Log(LogLevel level, const char* msg);
    virtual void LogError(const char* msg);
    virtual void LogInfo(const char* msg);
    virtual uint32_t GetErrorCount() const;

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    uint32_t errorCount_{0};
    // 실제론 UART handle, ring buffer, flash 주소 등등
    Hal::UartConfig uartCfg_{};
};

} // namespace Firmware
