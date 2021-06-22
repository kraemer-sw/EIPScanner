/*
* (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
*
* SPDX-License-Identifier: CLOSED
*/


#include "AnalogInputPointObject.h"
#include "utils/Buffer.h"


namespace eipScanner {
namespace vendor {
namespace wago {
namespace M750_363 {

using namespace cip;
using utils::Buffer;


AnalogInputPointObject::AnalogInputPointObject(CipUint instanceId, const SessionInfoIf::SPtr &si)
  : AnalogInputPointObject(instanceId, si, std::make_shared<MessageRouter>()) {

}

const cip::CipByte &AnalogInputPointObject::getAipObj_Value() const {
  return _aipObj_Value;
}

AnalogInputPointObject::AnalogInputPointObject(CipUint instanceId, const SessionInfoIf::SPtr &si,
                                                   const MessageRouter::SPtr& messageRouter)
  : BaseObject(CLASS_ID, instanceId)
  , _aipObj_Value{} {

  auto response = messageRouter->sendRequest(si, ServiceCodes::GET_ATTRIBUTE_SINGLE,
                                             EPath(CLASS_ID, instanceId, 1));

  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    Buffer buffer(response.getData());
    buffer >> _aipObj_Value;
  } else {
    logGeneralAndAdditionalStatus(response);
    throw std::runtime_error("Failed to read FULL_INFORMATION attribute");
  }
}


}
}
}
}
