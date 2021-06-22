/*
 * (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
 *
 * SPDX-License-Identifier: CLOSED
 */

#include "Mocks.h"
#include "vendor/wago/M750-363/DiscreteInputPointObject.h"
#include <gtest/gtest.h>

using namespace eipScanner;
using namespace eipScanner::vendor::wago::M750_363;

class TestDiscreteInputPointObject : public ::testing::Test {
public:
  const static cip::CipUint OBJECT_ID = 1;
  const std::vector<uint8_t> FULL_INFORMATION_DATA = {0x01};

  void SetUp() override {
    _messageRouter = std::make_shared<TMockMessageRouter>();
    _nullSession = nullptr;
  }

  TMockMessageRouter::SPtr _messageRouter;
  SessionInfoIf::SPtr _nullSession;
};

TEST_F(TestDiscreteInputPointObject, ShouldReadAllDataInConstructor) {
  cip::MessageRouterResponse response;
  response.setData(FULL_INFORMATION_DATA);

  EXPECT_CALL(*_messageRouter,
              sendRequest(_nullSession, cip::ServiceCodes::GET_ATTRIBUTE_SINGLE,
                          cip::EPath(0x65, OBJECT_ID, 0)))
      .WillOnce(::testing::Return(response));

  DiscreteInputPointObject discreteInputPointObject(OBJECT_ID, _nullSession,
                                                    _messageRouter);

  const auto &dipObj = discreteInputPointObject.getDipObj_Value();
  EXPECT_EQ(1, dipObj);
}

TEST_F(TestDiscreteInputPointObject, ShouldThrowExecptionIfFailedToGetData) {
  cip::MessageRouterResponse response;
  response.setData(FULL_INFORMATION_DATA);
  response.setGeneralStatusCode(
      cip::GeneralStatusCodes::INVALID_ATTRIBUTE_VALUE);

  EXPECT_CALL(*_messageRouter,
              sendRequest(_nullSession, cip::ServiceCodes::GET_ATTRIBUTE_SINGLE,
                          cip::EPath(0x65, OBJECT_ID, 0)))
      .WillOnce(::testing::Return(response));

  EXPECT_THROW(
      DiscreteInputPointObject(OBJECT_ID, _nullSession, _messageRouter),
      std::runtime_error);
}
