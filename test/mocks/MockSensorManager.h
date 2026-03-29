#pragma once
#include "SensorManager.h"   // ← fake 헤더가 잡힘 (include 경로 우선순위)
#include <gmock/gmock.h>

namespace Firmware {

class MockSensorManager : public SensorManager {
public:
    // 테스트에서 생성할 수 있도록 생성자를 public으로
    MockSensorManager() = default;

    MOCK_METHOD(int32_t, ReadTemperature, (), (override));
    MOCK_METHOD(int32_t, ReadPressure, (), (override));
    MOCK_METHOD(bool, IsCalibrated, (), (override));
    MOCK_METHOD(void, TriggerMeasurement, (), (override));
};

// GetInstance가 Mock을 반환하도록 설정하는 함수
void SetMockSensorManager(MockSensorManager* mock);

} // namespace Firmware
