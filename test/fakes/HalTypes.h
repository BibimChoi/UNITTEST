#pragma once
#include <cstdint>

// ★ FAKE: 실제 HalTypes.h의 무거운 내용 대신 필요한 것만
namespace Hal {

enum class GpioPin : uint8_t {
    PIN_0 = 0,
    PIN_1,
    PIN_2,
    PIN_3,
    MOTOR_EN = 10,
    MOTOR_DIR = 11,
    SENSOR_TRIG = 20,
};

enum class GpioState : uint8_t {
    LOW = 0,
    HIGH = 1,
};

// DmaConfig, UartConfig 등 무거운 타입은 여기서 빈 껍데기로
struct DmaConfig {
    uint32_t baseAddr{0};
    uint16_t channel{0};
};

struct UartConfig {
    uint32_t baudRate{0};
    uint8_t dataBits{0};
};

} // namespace Hal
