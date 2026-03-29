#pragma once
#include <cstdint>

// 실제 펌웨어에선 여기서 레지스터 맵, 인터럽트 벡터 등
// 온갖 HW 의존성이 딸려옴
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

// 실제론 여기에 DMA, Timer, UART 등 수십개 타입이 있다고 상상
struct DmaConfig {
    uint32_t baseAddr;
    uint16_t channel;
    // ... 생략
};

struct UartConfig {
    uint32_t baudRate;
    uint8_t dataBits;
    // ... 생략
};

} // namespace Hal
