cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(StepperMotor57Test)

# 如果需要throw 和 try catch，则加上下面这一句；
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
# 如果添加了自己的頭文件，則需要在main.cpp下面增加相對路徑文件名，不帶後綴，
# 如Dependence/StringHelper （注意後面不加.h）
add_executable(StepperMotor57Test
    main.cpp
    StepperMotorControl/DriveTimer
    StepperMotorControl/StepperMotorControl
    Dependence/GarbageCollect
    Dependence/StringHelper
    SerialCom/CMDInfo
    SerialCom/SerialCom
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(
    StepperMotor57Test
    pico_stdlib
    
    pico_multicore
    )

    # pico_util_queue

# Add any user requested libraries
# 不指定库，则全部连接；
#target_link_libraries(ComQATest)
# ??? pico_add_extra_outputs(ComQATest)

# Add the standard include files to the build
# 添加常见库和标准库（如果需要的话）
target_include_directories(StepperMotor57Test PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# create map/bin/hex/uf2 file in addition to ELF.
# 额外创建ELF文件
#pico_add_extra_outputs(StepperMotor57Test)

add_compile_options(-Wall)

