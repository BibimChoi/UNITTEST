#pragma once
#include "HalTypes.h"

// ★ FAKE: 실제 HalGpio.h와 동일한 인터페이스, HW 레지스터 의존성 없음
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
