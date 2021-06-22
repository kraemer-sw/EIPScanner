/*
* (C) Copyright 2021 Kraemer AG, Bassersdorf, Switzerland.
*
* SPDX-License-Identifier: CLOSED
*/


#include "ModuleConfigurationObject.h"
#include "utils/Buffer.h"


namespace eipScanner {
namespace vendor {
namespace wago {
namespace M750_363 {

using namespace cip;
using utils::Buffer;


ModuleConfigurationObject::ModuleConfigurationObject(CipUint instanceId, const SessionInfoIf::SPtr &si)
  : ModuleConfigurationObject(instanceId, si, std::make_shared<MessageRouter>()) {

}

const ModuleConfigurationObject::ModuleConfiguration &ModuleConfigurationObject::getModuleConfiguration() const {
  return m_moduleConfiguration;
}

ModuleConfigurationObject::ModuleConfigurationObject(CipUint instanceId, const SessionInfoIf::SPtr &si,
                                                   const MessageRouter::SPtr& messageRouter)
  : BaseObject(CLASS_ID, instanceId)
  , m_moduleConfiguration{} {

  auto response = messageRouter->sendRequest(si, ServiceCodes::GET_ATTRIBUTE_SINGLE,
                                             EPath(CLASS_ID, instanceId, 1));

  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    Buffer buffer(response.getData());
    buffer >> m_moduleConfiguration.at.raw;
  } else {
    logGeneralAndAdditionalStatus(response);
    throw std::runtime_error("Failed to read FULL_INFORMATION attribute");
  }
}

ModuleConfigurationObject::ModuleType ModuleConfigurationObject::getModuleType() const {
  switch (m_moduleConfiguration.at.moduleConfiguration.ioModuleType) {
    case 0:
      return ModuleType::analogue;
    case 1:
      return ModuleType::digital;
  }
  throw std::runtime_error("Unknown I/O module type");
}

bool ModuleConfigurationObject::getDigitalModuleHasInputs() const {
  return m_moduleConfiguration.at.moduleConfiguration.at.digitalIOModule.hasInputs;
}

bool ModuleConfigurationObject::getDigitalModuleHasOutputs() const {
  return m_moduleConfiguration.at.moduleConfiguration.at.digitalIOModule.hasOutputs;
}

uint8_t ModuleConfigurationObject::getDigitalModuleDataSizeBits() const {
  return m_moduleConfiguration.at.moduleConfiguration.at.digitalIOModule.dataSizeBits;
}

unsigned int ModuleConfigurationObject::getAnalogueModuleTypeDescription() const {
  return m_moduleConfiguration.at.moduleConfiguration.at.analogueIOModule.typeDescription;
}

}
}
}
}
