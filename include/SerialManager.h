#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <string>
#include <functional>
#include <memory>
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/SerialPortHotPlug.hpp" // 包含热插拔头文件

// 定义接收数据回调的函数类型，方便解耦
using DataCallback = std::function<void(const std::string&)>;

class SerialManager : public itas109::CSerialPortListener, public itas109::CSerialPortHotPlugListener
{
public:
    SerialManager();
    ~SerialManager();

    // 初始化并打开串口
    bool init(const std::string& configPath);
    
    // 发送数据
    bool send(const std::string& data);
    
    // 关闭串口
    void close();

    // 设置上层业务的回调函数（实现解耦）
    void setCallback(DataCallback cb);

protected:
    // 覆盖 CSerialPortListener 的虚函数：处理接收数据
    void onReadEvent(const char *portName, unsigned int readBufferLen) override;

    // 覆盖 CSerialPortHotPlugListener 的虚函数：处理热插拔
    void onHotPlugEvent(const char *portName, int isAdd) override;

private:
    itas109::CSerialPort *m_pSerialPort;
    itas109::CSerialPortHotPlug *m_pHotPlug;
    
    DataCallback m_dataCallback; // 上层逻辑的回调
    std::string m_portName;      // 当前端口名
    int m_baudRate;
};

#endif // SERIAL_MANAGER_H