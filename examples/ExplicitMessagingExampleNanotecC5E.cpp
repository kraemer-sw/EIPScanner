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

using namespace eipScanner::cip;
using eipScanner::SessionInfo;
using eipScanner::MessageRouter;
using eipScanner::ConnectionManager;
using eipScanner::cip::connectionManager::ConnectionParameters;
using eipScanner::cip::connectionManager::NetworkConnectionParams;
using eipScanner::utils::Buffer;
using eipScanner::utils::Logger;
using eipScanner::utils::LogLevel;

const CipUint GET_OBJECTDICTIONARY_ENTRY = 0x32;
const CipUint SET_OBJECTDICTIONARY_ENTRY = 0x33;
const CipUint CLASS_DESCRIPTOR = 0x64;
const CipUint INSTANCE_DESCRIPTOR = 0x1;



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

  auto si = std::make_shared<SessionInfo>("192.168.2.2", 0xAF12, std::chrono::seconds(10));
  auto messageRouter = std::make_shared<MessageRouter>();

  // Read attribute
  // read value nanotec (60C5h Max Acceleration)
  auto response = messageRouter->sendRequest(si, GET_OBJECTDICTIONARY_ENTRY,
                         EPath(CLASS_DESCRIPTOR, INSTANCE_DESCRIPTOR),
                         {0xc5, 0x60, 0x00});

  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    Buffer buffer(response.getData());
    CipUdint maxAcceleration;
    buffer >> maxAcceleration;

    Logger(LogLevel::INFO) << "Max Acceleration is " << maxAcceleration;
  } else {
    Logger(LogLevel::ERROR) << "We got error=0x" << std::hex << response.getGeneralStatusCode();
  }

  //Write attribute
  // set value nanotec (60C5h Max Acceleration)
  std::vector<uint8_t> address{0xc5, 0x60, 0x00};
  CipUdint newAcceleration = 4321;
  Buffer assembly;

  assembly << address
          << newAcceleration;

  response = messageRouter->sendRequest(si, SET_OBJECTDICTIONARY_ENTRY,
                      EPath(CLASS_DESCRIPTOR, INSTANCE_DESCRIPTOR),
                      assembly.data());

  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    Logger(LogLevel::INFO) << "Writing is successful";
  } else {
    Logger(LogLevel::ERROR) << "We got error=0x" << std::hex << response.getGeneralStatusCode();
  }


  //Read Digital Inputs
  response = messageRouter->sendRequest(si, GET_OBJECTDICTIONARY_ENTRY,
                         EPath(CLASS_DESCRIPTOR, INSTANCE_DESCRIPTOR),
                         {0xfd, 0x60, 0x00});

  if (response.getGeneralStatusCode() == GeneralStatusCodes::SUCCESS) {
    Buffer buffer(response.getData());
    CipUdint digitalInputs;
    buffer >> digitalInputs;

    Logger(LogLevel::INFO) << "Inputs " << std::bitset<8>((digitalInputs >> 16)) << ", Limit switches NLS " << (digitalInputs & 0x01) << " PLS " << (digitalInputs >> 1 & 0x01);
  } else {
    Logger(LogLevel::ERROR) << "We got error=0x" << std::hex << response.getGeneralStatusCode();
  }



#if OS_Windows
  WSACleanup();
#endif

  return EXIT_SUCCESS;
}
