#pragma once
#include <cstdint>

namespace Firmware {

class SensorManager {
public:
    static SensorManager* GetInstance();   // ★ 포인터 반환

    virtual ~SensorManager() = default;
    virtual int32_t ReadTemperature();
    virtual int32_t ReadPressure();
    virtual bool IsCalibrated();
    virtual void TriggerMeasurement();

protected:
    SensorManager() = default;
};

} // namespace Firmware
