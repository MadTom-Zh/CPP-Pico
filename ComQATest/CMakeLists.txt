


cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(ComQATest)

# 如果需要throw 和 try catch，则加上下面这一句；
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


# rest of your project
add_executable(ComQATest
    Dependence/GarbageCollect
    Dependence/StringHelper
    SerialCom/CMDInfo
    SerialCom/SerialCom
    main.cpp


)


# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(ComQATest pico_stdlib)


# create map/bin/hex/uf2 file in addition to ELF.
#pico_add_extra_outputs(ComQATest)

#add_subdirectory(Dependence)
#add_subdirectory(SerialCom)

#include_directories(Dependence)
#include_directories(SerialCom)

#target_include_directories(Dependence)
#target_include_directories(SerialCom)