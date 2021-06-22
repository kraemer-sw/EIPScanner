include_directories(ra/powerFlex525)
include_directories(yaskawa/mp3300iec)
include_directories(wago/M750-363)

set( VENDOR_FILES
        # ra
        vendor/ra/powerFlex525/DPIFaultManager.cpp
        vendor/ra/powerFlex525/DPIFaultObject.cpp
        vendor/ra/powerFlex525/DPIFaultCode.cpp
        vendor/ra/powerFlex525/DPIFaultParameter.cpp

        # yaskawa
        vendor/yaskawa/mp3300iec/Yaskawa_EPath.cpp
        vendor/yaskawa/mp3300iec/Yaskawa_MessageRouter.cpp
        vendor/yaskawa/mp3300iec/Yaskawa_MessageRouterRequest.cpp

        # wago
        vendor/wago/M750-363/AnalogInputPointObject.cpp
        vendor/wago/M750-363/DiscreteInputPointObject.cpp
        vendor/wago/M750-363/DiscreteOutputPointObject.cpp
        vendor/wago/M750-363/ModuleConfigurationObject.cpp
        vendor/wago/M750-363/AssemblyObject.cpp

        PARENT_SCOPE
)
