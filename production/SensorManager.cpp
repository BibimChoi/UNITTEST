#include "SensorManager.h"

namespace Firmware {

SensorManager& SensorManager::GetInstance() {
    static SensorManager instance;
    return instance;
}

int32_t SensorManager::ReadTemperature() {
    // 실제론 I2C로 센서 읽기
    TriggerMeasurement();
    return lastTemp_;
}

int32_t SensorManager::ReadPressure() {
    TriggerMeasurement();
    return lastPressure_;
}

bool SensorManager::IsCalibrated() {
    return calibrated_;
}

void SensorManager::TriggerMeasurement() {
    if (gpio_) {
        gpio_->SetPin(Hal::GpioPin::SENSOR_TRIG, Hal::GpioState::HIGH);
        // delay, read ADC, etc...
        gpio_->SetPin(Hal::GpioPin::SENSOR_TRIG, Hal::GpioState::LOW);
    }
}

} // namespace Firmware
