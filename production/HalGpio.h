#pragma once
#include "HalTypes.h"

// HW GPIO 추상화 - 실제론 레지스터 직접 접근
namespace Hal {

class HalGpio {
public:
    virtual ~HalGpio() = default;
    virtual void SetPin(GpioPin pin, GpioState state);
    virtual GpioState ReadPin(GpioPin pin);
    virtual void ConfigureOutput(GpioPin pin);
    virtual void ConfigureInput(GpioPin pin);
};

} // namespace Hal
