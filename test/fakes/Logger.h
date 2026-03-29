#pragma once
#include <cstdint>

namespace Firmware {

enum class LogLevel : uint8_t {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
};

class Logger {
public:
    static Logger* GetInstance();   // ★ 포인터 반환

    virtual ~Logger() = default;
    virtual void Log(LogLevel level, const char* msg);
    virtual void LogError(const char* msg);
    virtual void LogInfo(const char* msg);
    virtual uint32_t GetErrorCount() const;

protected:
    Logger() = default;
};

} // namespace Firmware
