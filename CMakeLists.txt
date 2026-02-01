cmake_minimum_required(VERSION 3.10)
project(MotorControl)

set(CMAKE_CXX_STANDARD 11)

# 1. 查找依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

# 2. 设置头文件路径
include_directories(
    include
    thirdparty/CSerialPort/include
    ${YAML_CPP_INCLUDE_DIRS}
)

# -----------------------------------------------------------
# 3. 手动构建 CSerialPort 库 (修复 Windows 文件报错的问题)
# -----------------------------------------------------------

# 搜集所有源文件
file(GLOB CSERIALPORT_SOURCES "thirdparty/CSerialPort/src/*.cpp")

# === 关键修改：根据操作系统过滤源文件 ===
if(UNIX)
    # Linux/Unix 下，剔除所有以 WinBase.cpp 结尾的文件
    list(FILTER CSERIALPORT_SOURCES EXCLUDE REGEX ".*WinBase\\.cpp$")
elseif(WIN32)
    # Windows 下，剔除所有以 UnixBase.cpp 结尾的文件
    list(FILTER CSERIALPORT_SOURCES EXCLUDE REGEX ".*UnixBase\\.cpp$")
endif()

# 打印最终参与编译的文件列表（用于检查）
message(STATUS "Platform Sources: ${CSERIALPORT_SOURCES}")

if(NOT CSERIALPORT_SOURCES)
    message(FATAL_ERROR "No sources found! Check thirdparty/CSerialPort/src path.")
endif()

# 生成静态库
add_library(CSerialPort STATIC ${CSERIALPORT_SOURCES})
target_link_libraries(CSerialPort pthread rt)

# -----------------------------------------------------------
# 4. 构建主程序
# -----------------------------------------------------------

add_executable(motor_ctrl
    src/main.cpp
    src/SerialManager.cpp
)

target_link_libraries(motor_ctrl
    CSerialPort
    ${YAML_CPP_LIBRARIES}
    pthread
)