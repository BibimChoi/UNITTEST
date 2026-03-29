#pragma once
#include "HalTypes.h"
#include "HalGpio.h"
#include <cstdint>

// SensorManager - Singleton
// 실제 펌웨어에선 I2C, SPI 드라이버 헤더도 include 되어 있어서
// include 체인이 매우 깊어짐
namespace Firmware {

class SensorManager {
public:
    static SensorManager* GetInstance();

    virtual ~SensorManager() = default;
    virtual int32_t ReadTemperature();
    virtual int32_t ReadPressure();
    virtual bool IsCalibrated();
    virtual void TriggerMeasurement();

private:
    SensorManager() = default;
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;

    Hal::HalGpio* gpio_{nullptr};
    bool calibrated_{false};
    int32_t lastTemp_{0};
    int32_t lastPressure_{0};
};

} // namespace Firmware
