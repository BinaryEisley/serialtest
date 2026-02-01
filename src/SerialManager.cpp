#include "SerialManager.h"
#include <iostream>
#include <yaml-cpp/yaml.h>

using namespace itas109;

SerialManager::SerialManager() : m_pSerialPort(nullptr), m_pHotPlug(nullptr) {
    m_pSerialPort = new CSerialPort();
    m_pHotPlug = new CSerialPortHotPlug();
    
    // 注册热插拔监听
    m_pHotPlug->connectHotPlugEvent(this);
}

SerialManager::~SerialManager() {
    close();
    if (m_pSerialPort) {
        delete m_pSerialPort;
        m_pSerialPort = nullptr;
    }
    if (m_pHotPlug) {
        delete m_pHotPlug;
        m_pHotPlug = nullptr;
    }
}

bool SerialManager::init(const std::string& configPath) {
    try {
        YAML::Node config = YAML::LoadFile(configPath);
        
        m_portName = config["serial"]["port_name"].as<std::string>();
        m_baudRate = config["serial"]["baud_rate"].as<int>();
        int dataBits = config["serial"]["data_bits"].as<int>();
        int parity = config["serial"]["parity"].as<int>();
        int stopBits = config["serial"]["stop_bits"].as<int>();
        int flowControl = config["serial"]["flow_control"].as<int>();

        m_pSerialPort->init(m_portName.c_str(), 
                            m_baudRate, 
                            static_cast<Parity>(parity), 
                            static_cast<DataBits>(dataBits), 
                            static_cast<StopBits>(stopBits), 
                            static_cast<FlowControl>(flowControl));

        m_pSerialPort->setReadIntervalTimeout(0); // 读间隔超时设置，根据需要调整

        // 注册读取事件监听
        m_pSerialPort->connectReadEvent(this);

        if (m_pSerialPort->open()) {
            std::cout << "[Serial] Port " << m_portName << " opened successfully." << std::endl;
            return true;
        } else {
            std::cerr << "[Serial] Failed to open port " << m_portName << std::endl;
            return false;
        }

    } catch (const YAML::Exception& e) {
        std::cerr << "[Config] YAML Error: " << e.what() << std::endl;
        return false;
    }
}

bool SerialManager::send(const std::string& data) {
    if (!m_pSerialPort->isOpen()) return false;
    // CSerialPort 的 writeData 通常不保证全部发完，实际工程可能需要循环写
    return m_pSerialPort->writeData(data.c_str(), data.length()) > 0;
}

void SerialManager::close() {
    if (m_pSerialPort && m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
        std::cout << "[Serial] Port closed." << std::endl;
    }
}

void SerialManager::setCallback(DataCallback cb) {
    m_dataCallback = cb;
}

// ---------------- 事件回调实现 ----------------

// 当串口有数据到达时触发
void SerialManager::onReadEvent(const char *portName, unsigned int readBufferLen) {
    if (readBufferLen > 0) {
        // 读取所有数据
        // 注意：CSerialPort的readAllData返回缓冲区指针，通常需要拷贝出来
        char *data = new char[readBufferLen + 1];
        int recLen = m_pSerialPort->readData(data, readBufferLen);
        
        if (recLen > 0) {
            data[recLen] = '\0'; // 确保字符串结束符
            std::string strData(data, recLen);
            
            // 调用上层回调，将数据传出去，不在这里处理业务逻辑
            if (m_dataCallback) {
                m_dataCallback(strData);
            }
        }
        delete[] data;
    }
}

// 当串口插入或拔出时触发
void SerialManager::onHotPlugEvent(const char *portName, int isAdd) {
    std::cout << "[HotPlug] Port: " << portName << " | Action: " << (isAdd ? "Plugged In" : "Removed") << std::endl;
    
    // 简单的自动重连逻辑示例
    if (isAdd && m_portName == portName) {
        std::cout << "[HotPlug] Target device inserted, attempting to reopen..." << std::endl;
        if(m_pSerialPort->isOpen()) m_pSerialPort->close();
        if(m_pSerialPort->open()) {
            std::cout << "[HotPlug] Reconnected successfully." << std::endl;
        }
    } else if (!isAdd && m_portName == portName) {
        std::cout << "[HotPlug] Target device removed." << std::endl;
        m_pSerialPort->close();
    }
}