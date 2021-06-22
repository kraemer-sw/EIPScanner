/*
 * (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
 *
 * SPDX-License-Identifier: CLOSED
 */

#pragma once


#include <stdexcept>

#include "cip/CipString.h"
#include "BaseObject.h"
#include "SessionInfoIf.h"
#include "MessageRouter.h"

namespace eipScanner {
namespace vendor {
namespace wago {
namespace M750_363 {

class AnalogInputPointObject : public BaseObject {
public:
  static const cip::CipUint CLASS_ID = 0x67;

  /**
   * @brief Creates an instance and reads all its data via EIP
   * @param instanceId
   * @param fullAttributes if true, then read all the attributes
   * @param si the EIP session for explicit messaging
   */
  AnalogInputPointObject(cip::CipUint instanceId,
                         const SessionInfoIf::SPtr &si);

  /**
   * @note used for testing
   * @param instanceId
   * @param si
   * @param messageRouter
   */
  AnalogInputPointObject(cip::CipUint instanceId,
                         const SessionInfoIf::SPtr &si, const MessageRouter::SPtr& messageRouter);

  /**
   * @brief Gets the full information [AttrID=1] of the fault
   * @return
   */
  const cip::CipByte &getAipObj_Value() const;

private:
  cip::CipByte _aipObj_Value;;
};

}
}
}
}

