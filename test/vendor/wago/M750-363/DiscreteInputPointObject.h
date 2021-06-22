/*
 * (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
 *
 * SPDX-License-Identifier: CLOSED
 */

#pragma once

#include <stdexcept>

#include "BaseObject.h"
#include "MessageRouter.h"
#include "SessionInfoIf.h"
#include "cip/CipString.h"

namespace eipScanner {
namespace vendor {
namespace wago {
namespace M750_363 {

class DiscreteInputPointObject : public BaseObject {
public:
  static const cip::CipUint CLASS_ID = 0x65;

  /**
   * @brief Creates an instance and reads all its data via EIP
   * @param instanceId
   * @param fullAttributes if true, then read all the attributes
   * @param si the EIP session for explicit messaging
   */
  DiscreteInputPointObject(cip::CipUint instanceId,
                           const SessionInfoIf::SPtr &si);

  /**
   * @note used for testing
   * @param instanceId
   * @param si
   * @param messageRouter
   */
  DiscreteInputPointObject(cip::CipUint instanceId,
                           const SessionInfoIf::SPtr &si,
                           const MessageRouter::SPtr &messageRouter);

  /**
   * @brief Gets the full information [AttrID=1] of the fault
   * @return
   */
  const cip::CipByte &getDipObj_Value() const;

  bool isSet() const;

private:
  cip::CipByte _dipObj_Value;
  ;
};

} // namespace M750_363
} // namespace wago
} // namespace vendor
} // namespace eipScanner
