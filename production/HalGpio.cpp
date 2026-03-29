#include "HalGpio.h"

namespace Hal {

void HalGpio::SetPin(GpioPin pin, GpioState state) {
    // 실제론 레지스터 쓰기
    // volatile uint32_t* reg = (volatile uint32_t*)(GPIO_BASE + offset);
    // *reg = (state == GpioState::HIGH) ? (*reg | mask) : (*reg & ~mask);
    (void)pin;
    (void)state;
}

GpioState HalGpio::ReadPin(GpioPin pin) {
    // 실제론 레지스터 읽기
    (void)pin;
    return GpioState::LOW;
}

void HalGpio::ConfigureOutput(GpioPin pin) {
    (void)pin;
}

void HalGpio::ConfigureInput(GpioPin pin) {
    (void)pin;
}

} // namespace Hal
