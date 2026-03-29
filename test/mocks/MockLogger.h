#pragma once
#include "Logger.h"   // ← fake 헤더가 잡힘
#include <gmock/gmock.h>

namespace Firmware {

class MockLogger : public Logger {
public:
    MockLogger() = default;

    MOCK_METHOD(void, Log, (LogLevel level, const char* msg), (override));
    MOCK_METHOD(void, LogError, (const char* msg), (override));
    MOCK_METHOD(void, LogInfo, (const char* msg), (override));
    MOCK_METHOD(uint32_t, GetErrorCount, (), (const, override));
};

void SetMockLogger(MockLogger* mock);

} // namespace Firmware
