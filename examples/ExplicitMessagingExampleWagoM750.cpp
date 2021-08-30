//
// Created by Aleksey Timin on 11/16/19.
//

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#include <winsock2.h>
#define OS_Windows (1)
#endif

#include <cstdlib>
#include <sstream>
#include <bitset>
#include <cip/connectionManager/NetworkConnectionParams.h>
#include "SessionInfo.h"
#include "MessageRouter.h"
#include "ConnectionManager.h"
#include "utils/Logger.h"
#include "utils/Buffer.h"
#include "vendor/wago/M750-363/DiscreteInputPointObject.h"
#include "vendor/wago/M750-363/DiscreteOutputPointObject.h"
#include "vendor/wago/M750-363/ModuleConfigurationObject.h"

using namespace eipScanner::cip;
using eipScanner::SessionInfo;
using eipScanner::MessageRouter;
using eipScanner::ConnectionManager;
using eipScanner::cip::connectionManager::ConnectionParameters;
using eipScanner::cip::connectionManager::NetworkConnectionParams;
using eipScanner::utils::Buffer;
using eipScanner::utils::Logger;
using eipScanner::utils::LogLevel;
using eipScanner::vendor::wago::M750_363::DiscreteInputPointObject;
using eipScanner::vendor::wago::M750_363::DiscreteOutputPointObject;
using eipScanner::vendor::wago::M750_363::ModuleConfigurationObject;

int main() {
  Logger::setLogLevel(LogLevel::DEBUG);

#if OS_Windows
  WSADATA wsaData;
  int winsockStart = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (winsockStart != 0) {
    Logger(LogLevel::ERROR) << "Failed to start WinSock - error code: " << winsockStart;
    return EXIT_FAILURE;
  }
#endif

  auto si = std::make_shared<SessionInfo>("192.168.1.1", 0xAF12, std::chrono::seconds(10));
  auto messageRouter = std::make_shared<MessageRouter>();

  // di lesen
  DiscreteInputPointObject discreteInputPointObject(1, si, messageRouter);
  Logger(LogLevel::INFO) << "DI " << std::bitset<8>(discreteInputPointObject.getDipObj_Value());

  // do schreiben
  for (auto i: {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}) {
    DiscreteOutputPointObject discreteOutputPointObject(i, si, messageRouter);
    discreteOutputPointObject.setDopObj_Value(1);
  }

  // do zurücklesen
  auto response = messageRouter->sendRequest(si, ServiceCodes::GET_ATTRIBUTE_SINGLE,
                         EPath(0x66, 2, 1));
  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    std::ostringstream ss;
    ss << " data=";
    for (auto &byte : response.getData()) {
      ss << "[" << std::hex << (int) byte << "]";
    }
    Logger(LogLevel::INFO) << "Received: " << ss.str();
  } else {
    Logger(LogLevel::ERROR) << "We got error=0x" << std::hex << response.getGeneralStatusCode();
  }

  for (auto i: {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}) {
    DiscreteOutputPointObject discreteOutputPointObject(i, si, messageRouter);
    discreteOutputPointObject.setDopObj_Value(0);
  }

  ModuleConfigurationObject moduleConfigurationObject0(1, si, messageRouter);
  //printf("%s: %d, %d \n", __FILE__, __LINE__, moduleConfigurationObject.getModuleType());
  ModuleConfigurationObject moduleConfigurationObject2(2, si, messageRouter);
//  ModuleConfigurationObject moduleConfigurationObject3(3, si, messageRouter);
//  ModuleConfigurationObject moduleConfigurationObject4(4, si, messageRouter);
//  ModuleConfigurationObject moduleConfigurationObject5(5, si, messageRouter);

#if OS_Windows
  WSACleanup();
#endif

  return EXIT_SUCCESS;
}
