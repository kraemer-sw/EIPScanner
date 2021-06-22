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

class ModuleConfigurationObject : public BaseObject {
public:
  static const cip::CipUint CLASS_ID = 0x80;

  enum class ModuleType {
    analogue,
    digital
  };

  struct ModuleConfiguration {
    union {
      struct ModuleConfiguration {
        union {
          struct DigitalIOModule {
            unsigned char hasInputs : 1;
            unsigned char hasOutputs : 1;
            unsigned char : 0; // start a new byte
            unsigned char dataSizeBits : 7;
          } digitalIOModule;
          struct AnalogueIOModule {
            unsigned int typeDescription : 15;
          } analogueIOModule;
        } at;
        unsigned char ioModuleType : 1;
      } moduleConfiguration;
      uint16_t raw;
    } at;
  };

  /**
   * @brief Creates an instance and reads all its data via EIP
   * @param instanceId
   * @param fullAttributes if true, then read all the attributes
   * @param si the EIP session for explicit messaging
   */
  ModuleConfigurationObject(cip::CipUint instanceId,
                           const SessionInfoIf::SPtr &si);

  /**
   * @note used for testing
   * @param instanceId
   * @param si
   * @param messageRouter
   */
  ModuleConfigurationObject(cip::CipUint instanceId,
                           const SessionInfoIf::SPtr &si, const MessageRouter::SPtr& messageRouter);

  /**
   * @brief Gets the full information [AttrID=1] of the fault
   * @return
   */
  const ModuleConfiguration &getModuleConfiguration() const;

  ModuleType getModuleType() const;

  bool getDigitalModuleHasInputs() const;
  bool getDigitalModuleHasOutputs() const;
  uint8_t getDigitalModuleDataSizeBits() const;
  unsigned int getAnalogueModuleTypeDescription() const;

private:
  ModuleConfiguration m_moduleConfiguration;
};
}
}
}
}

