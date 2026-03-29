#pragma once
#include "HalGpio.h"
#include <gmock/gmock.h>

namespace Hal {

class MockHalGpio : public HalGpio {
public:
    MOCK_METHOD(void, SetPin, (GpioPin pin, GpioState state), (override));
    MOCK_METHOD(GpioState, ReadPin, (GpioPin pin), (override));
    MOCK_METHOD(void, ConfigureOutput, (GpioPin pin), (override));
    MOCK_METHOD(void, ConfigureInput, (GpioPin pin), (override));
};

} // namespace Hal
