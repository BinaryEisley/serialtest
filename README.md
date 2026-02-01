# MotorControl - 串口电机控制程序

这是一个基于 C++ 的串口通信与电机控制示例项目。它封装了 `CSerialPort` 库用于底层通信，使用 `yaml-cpp` 进行参数配置，并实现了串口热插拔检测和异步数据回调机制。

## 📁 项目结构

```text
.
├── CMakeLists.txt          # CMake 构建脚本，包含跨平台编译逻辑
├── config.yaml             # 串口配置文件
├── include/
│   └── SerialManager.h     # 串口管理类头文件
├── src/
│   ├── main.cpp            # 主程序入口，包含业务逻辑
│   └── SerialManager.cpp   # 串口管理类实现
└── thirdparty/
    └── CSerialPort/        # 第三方库 CSerialPort (作为子模块)

```

## ✨ 功能特性

* **串口通信封装**：通过 `SerialManager` 类封装了底层的串口读写操作。
* **配置解耦**：使用 YAML 文件 (`config.yaml`) 管理串口参数（端口号、波特率等），无需重新编译即可修改配置。
* **异步回调**：接收到的串口数据通过回调函数 (`DataCallback`) 传递给上层业务逻辑，实现了通信与业务的解耦。
* **热插拔支持**：集成 `CSerialPortHotPlug`，能够检测串口设备的插入和拔出，并尝试自动重连。
* **跨平台构建**：`CMakeLists.txt` 包含了针对 Windows 和 Linux/Unix 的源文件过滤逻辑，解决了 `CSerialPort` 在不同平台下的编译兼容性问题。

## 🛠️ 依赖环境

在编译之前，请确保您的开发环境安装了以下依赖：

1. **C++ 编译器** (支持 C++11 标准)
2. **CMake** (VERSION 3.10 或更高)
3. **yaml-cpp** 开发库
* Ubuntu/Debian: `sudo apt-get install libyaml-cpp-dev`
* 其他系统需确保 `pkg-config` 能找到 `yaml-cpp`。


4. **CSerialPort**
* 本项目将 `CSerialPort` 作为源码集成在 `thirdparty/CSerialPort` 目录下，构建时会自动编译静态库。



## 🚀 编译与构建

请按照以下步骤进行编译：

```bash
# 1. 创建并进入构建目录
mkdir build
cd build

# 2. 运行 CMake 生成构建文件
cmake ..

# 3. 编译项目
make
# 或者在 Windows 上使用: cmake --build .

```

## ⚙️ 配置说明

在运行程序前，请检查根目录下的 `config.yaml` 文件，并根据实际硬件修改以下参数：

```yaml
serial:
  port_name: /dev/ttyUSB0  # Windows系统可能是 COM3 等
  baud_rate: 115200
  data_bits: 8
  parity: 0               # 0:None, 1:Odd, 2:Even
  stop_bits: 1
  flow_control: 0         # 0:None

```

## 🏃‍♂️ 运行与测试

编译成功后，将在构建目录生成可执行文件 `motor_ctrl`。

由于代码中读取配置文件的路径被硬编码为 `../config.yaml`，请确保在 **`build` 目录**下运行程序：

```bash
# 在 build 目录下执行
./motor_ctrl

```

**运行预期：**

1. 程序启动并尝试打开配置文件中指定的串口。
2. 主循环会每秒发送一条 `STATUS_QUERY_x` 指令给电机/下位机。
3. 如果收到串口返回的数据，控制台会打印 `>>> [Motor Logic] Processing command: ...`。
4. 如果收到字符串 `STOP`，会触发模拟的急停逻辑。
5. 插拔串口设备时，控制台会输出热插拔检测日志。

## 📝 开发说明

* **业务逻辑修改**：主要的业务处理逻辑在 `src/main.cpp` 的 `processMotorCommand` 函数中修改。
* **串口底层修改**：如果需要调整串口的底层行为（如读写超时），请修改 `src/SerialManager.cpp`。
