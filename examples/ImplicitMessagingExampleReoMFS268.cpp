//
// Created by Aleksey Timin on 11/16/19.
//

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#include <winsock2.h>
#define OS_Windows (1)
#endif

#include <sstream>
#include <cip/connectionManager/NetworkConnectionParams.h>
#include "SessionInfo.h"
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

enum ReoWriteState {
  Idle,
  AwaitWriteEnable,
  AwaitParameterWrite,
  AwaitWriteDisable,
  Done
};

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

  auto si = std::make_shared<SessionInfo>("192.168.0.2", 0xAF12);

  // Implicit messaging
  ConnectionManager connectionManager;

  ConnectionParameters parameters;
  parameters.connectionPath = {0x20, 0x04, 0x24, 0x03, 0x2C, 0x64, 0x2C, 0x65};  // REO
  parameters.o2tRealTimeFormat = true;
  parameters.originatorVendorId = 0xffff;
  parameters.originatorSerialNumber = 0x12345;
  parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::P2P;
  parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::SCHEDULED_PRIORITY;
  parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::FIXED;

  parameters.t2oNetworkConnectionParams |= 8; // REO
  parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::P2P;
  parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::SCHEDULED_PRIORITY;
  parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::FIXED;
  parameters.o2tNetworkConnectionParams |= 8; // REO

  parameters.originatorSerialNumber = 0x12345;
  parameters.o2tRPI = 1000000;
  parameters.t2oRPI = 1000000;
  parameters.transportTypeTrigger |= NetworkConnectionParams::CLASS1;

  ReoWriteState writeState = Idle;
  CipUint parameterAddress = 0x9005;
  CipUint parameterValue = 3200; // Hz*100
  Buffer parameterPair;
  parameterPair << parameterAddress << parameterValue;

  auto io = connectionManager.forwardOpen(si, parameters);
  if (auto ptr = io.lock()) {
      ptr->setDataToSend(std::vector<uint8_t>{0x00, 0x00, 0xc0, 0xde, 0xb5, 0xe7, 0x80, 0x00});  // senden Schreibfreigabe (5.2.2)
//      ptr->setDataToSend(std::vector<uint8_t>{0x00, 0x00, 0x90, 0x05, 0x13, 0x88, 0x80, 0x00});  // senden Parameter (5.2.4)
//      ptr->setDataToSend(std::vector<uint8_t>{0x00, 0x00, 0xc0, 0xde, 0x00, 0x00, 0x80, 0x00});  // senden Schreibfreigabe aufheben (5.2.6)
//      ptr->setDataToSend(std::vector<uint8_t>{0x00, 0x00, 0x10, 0x05, 0x0, 0x0, 0x80, 0x0});

    ptr->setCloseListener([]() {
      Logger(LogLevel::INFO) << "Closed";
    });

    ptr->setReceiveDataListener([ptr, &writeState, parameterPair](auto realTimeHeader, auto sequence, auto data) {
      std::ostringstream ss;
      ss << "secNum=" << sequence << " data=";
      for (auto &byte : data) {
        ss << "[" << std::hex << (int) byte << "]";
      }
      Logger(LogLevel::INFO) << "Received: " << ss.str();

      printf("****  state: %d\n", writeState);

      switch (writeState) {
        case Idle: {
          ptr->setDataToSend(std::vector<uint8_t>{0x00, 0x00, 0xc0, 0xde, 0xb5, 0xe7, 0x80, 0x00});  // senden Schreibfreigabe (5.2.2)
          writeState = AwaitWriteEnable;
          break;
        }
        case AwaitWriteEnable: {
          if (data != std::vector<uint8_t>{0x00, 0x00, 0xc0, 0xde, 0xb5, 0xe7, 0xc0, 0xde}) {
            printf("%s: %d,  AwaitWriteEnable\n", __FILE__, __LINE__);
            return;
          }
          auto sendData = std::vector<uint8_t>{0x00, 0x00, 0x90, 0x05, 0x13, 0x88, 0x80, 0x00};  // 0x00 0x00, address, value, 0x80 0x00
          const auto bufVec = parameterPair.data();
          sendData[2] = bufVec[1];
          sendData[3] = bufVec[0];
          sendData[4] = bufVec[3];
          sendData[5] = bufVec[2];
          ptr->setDataToSend(sendData);  // senden Parameter (5.2.4)
          writeState = AwaitParameterWrite;
          break;
        }
        case AwaitParameterWrite: {
          auto expectedData = std::vector<uint8_t>{0x00, 0x00, 0x90, 0x05, 0x13, 0x88, 0xc0, 0xde};    // 0x00 0x00, address, value, 0xc0 0xde
          const auto bufVec = parameterPair.data();
          expectedData[2] = bufVec[1];
          expectedData[3] = bufVec[0];
          expectedData[4] = bufVec[3];
          expectedData[5] = bufVec[2];
          if (data != expectedData) {
            printf("%s: %d,  AwaitParameterWrite\n", __FILE__, __LINE__);
            return;
          }
          ptr->setDataToSend(std::vector<uint8_t>{0x00, 0x00, 0xc0, 0xde, 0x00, 0x00, 0x80, 0x00});  // senden Schreibfreigabe aufheben (5.2.6)
          writeState = AwaitWriteDisable;
          break;
        }
        case AwaitWriteDisable: {
          if (data != std::vector<uint8_t>{0x00, 0x00, 0xc0, 0xde, 0x00, 0x00, 0xc0, 0xde}) {
            printf("%s: %d,  AwaitWriteDisable\n", __FILE__, __LINE__);
            return;
          }
          writeState = Done;
          break;
        }
        case Done: {
          printf("%s: %d,  Done\n", __FILE__, __LINE__);
          break;
        }
      }
    });
  }

  int count = 200;
  while (connectionManager.hasOpenConnections() && count-- > 0) {
    connectionManager.handleConnections(std::chrono::milliseconds(100));
  }

  connectionManager.forwardClose(si, io);

#if OS_Windows
  WSACleanup();
#endif

  return EXIT_SUCCESS;
}

