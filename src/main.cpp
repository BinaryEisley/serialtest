#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "SerialManager.h"

// 全局运行标志
std::atomic<bool> g_running(true);

// 模拟：解析串口指令并控制电机
void processMotorCommand(const std::string& cmd) {
    // 这里是你的业务逻辑解耦区
    std::cout << ">>> [Motor Logic] Processing command: " << cmd << std::endl;
    if (cmd == "STOP") {
        std::cout << ">>> [Motor Logic] Emergency Stop!" << std::endl;
    }
}

int main() {
    SerialManager serial;

    // 1. 设置回调函数 (Lambda表达式)
    serial.setCallback([](const std::string& data) {
        // 收到数据后，这里被调用
        // 注意：这通常运行在子线程，如果涉及复杂共享数据需加锁
        processMotorCommand(data);
    });

    // 2. 初始化
    if (!serial.init("../config.yaml")) {
        std::cerr << "Initialization failed!" << std::endl;
        return -1;
    }

    std::cout << "Program started. Press Ctrl+C to exit." << std::endl;

    // 3. 主循环 (模拟发送电机控制指令)
    int count = 0;
    while (g_running) {
        // 模拟每隔1秒发送一个心跳或状态查询指令
        std::string cmd = "STATUS_QUERY_" + std::to_string(count++);
        serial.send(cmd);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}