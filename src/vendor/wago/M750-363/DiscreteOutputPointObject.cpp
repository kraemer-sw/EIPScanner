/*
* (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
*
* SPDX-License-Identifier: CLOSED
*/


#include "DiscreteOutputPointObject.h"
#include "utils/Buffer.h"


namespace eipScanner {
namespace vendor {
namespace wago {
namespace M750_363 {

using namespace cip;
using utils::Buffer;


DiscreteOutputPointObject::DiscreteOutputPointObject(CipUint instanceId, const SessionInfoIf::SPtr &si)
  : DiscreteOutputPointObject(instanceId, si, std::make_shared<MessageRouter>()) {

}

void DiscreteOutputPointObject::setDopObj_Value(const cip::CipByte dopObj_value)  {
  _dipObj_Value = dopObj_value;

  Buffer digitalOutput;
  digitalOutput << CipByte(_dipObj_Value);

  auto response = m_messageRouter->sendRequest(m_sessionInfoIf, ServiceCodes::SET_ATTRIBUTE_SINGLE,
                                             EPath(CLASS_ID, getInstanceId(), 1),
                                             digitalOutput.data());

}

DiscreteOutputPointObject::DiscreteOutputPointObject(CipUint instanceId, const SessionInfoIf::SPtr &si,
                                                   const MessageRouter::SPtr& messageRouter)
  : BaseObject(CLASS_ID, instanceId)
  , m_sessionInfoIf{si}
  , m_messageRouter{messageRouter}
  , _dipObj_Value{} {



//  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
//    Buffer buffer(response.getData());
//    buffer >> _dipObj_Value;
//  } else {
//    logGeneralAndAdditionalStatus(response);
//    throw std::runtime_error("Failed to read FULL_INFORMATION attribute");
//  }
}

//bool DiscreteOutputPointObject::isSet() const {
//  return _dipObj_Value & 0x01;
//}

}
}
}
}
