/*
 * (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
 *
 * SPDX-License-Identifier: CLOSED
 */


#include "DiscreteInputPointObject.h"
#include "utils/Buffer.h"


namespace eipScanner {
namespace vendor {
namespace wago {
namespace M750_363 {

using namespace cip;
using utils::Buffer;

enum DiscreteInputPointObjectAttributeIds : CipUsint {
  FULL_INFORMATION = 0
};

DiscreteInputPointObject::DiscreteInputPointObject(CipUint instanceId, const SessionInfoIf::SPtr &si)
  : DiscreteInputPointObject(instanceId, si, std::make_shared<MessageRouter>()) {

}

const cip::CipByte &DiscreteInputPointObject::getDipObj_Value() const {
  return _dipObj_Value;
}

DiscreteInputPointObject::DiscreteInputPointObject(CipUint instanceId, const SessionInfoIf::SPtr &si,
                                                   const MessageRouter::SPtr& messageRouter)
  : BaseObject(CLASS_ID, instanceId)
  , _dipObj_Value{} {

  auto response = messageRouter->sendRequest(si, ServiceCodes::GET_ATTRIBUTE_SINGLE,
                                             EPath(CLASS_ID, instanceId, DiscreteInputPointObjectAttributeIds::FULL_INFORMATION));

  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    Buffer buffer(response.getData());
    buffer >> _dipObj_Value;
  } else {
    logGeneralAndAdditionalStatus(response);
    throw std::runtime_error("Failed to read FULL_INFORMATION attribute");
  }
}

bool DiscreteInputPointObject::isSet() const {
  return _dipObj_Value & 0x01;
}

}
}
}
}
