#include "HalGpio.h"

// HalGpio의 virtual 함수 기본 구현
// MockHalGpio가 override하므로 실제로 호출되지 않음
namespace Hal {

void HalGpio::SetPin(GpioPin pin, GpioState state) { (void)pin; (void)state; }
GpioState HalGpio::ReadPin(GpioPin pin) { (void)pin; return GpioState::LOW; }
void HalGpio::ConfigureOutput(GpioPin pin) { (void)pin; }
void HalGpio::ConfigureInput(GpioPin pin) { (void)pin; }

} // namespace Hal
