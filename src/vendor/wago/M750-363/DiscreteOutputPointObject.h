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

class DiscreteOutputPointObject : public BaseObject {
public:
  static const cip::CipUint CLASS_ID = 0x66;

  /**
   * @brief Creates an instance and reads all its data via EIP
   * @param instanceId
   * @param fullAttributes if true, then read all the attributes
   * @param si the EIP session for explicit messaging
   */
  DiscreteOutputPointObject(cip::CipUint instanceId,
                           const SessionInfoIf::SPtr &si);

  /**
   * @note used for testing
   * @param instanceId
   * @param si
   * @param messageRouter
   */
  DiscreteOutputPointObject(cip::CipUint instanceId,
                           const SessionInfoIf::SPtr &si, const MessageRouter::SPtr& messageRouter);

 void setDopObj_Value(const cip::CipByte);

  //bool isSet() const;

private:
 const SessionInfoIf::SPtr& m_sessionInfoIf;
 const MessageRouter::SPtr& m_messageRouter;
  cip::CipByte _dipObj_Value;
};
}
}
}
}

