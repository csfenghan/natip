/****************************************************************************
 *	这是文件定义了jdt消息的结构和参数类型
 ****************************************************************************/
#ifndef JDT_BODY_HPP
#define JDT_BODY_HPP

#include <iostream>

namespace jdt {
/****************************************
 *	jdt头部用到的类型参数
 ***************************************/
// service的类型
enum ServiceType {
        SEND_DATA = 1, // 发送的是数据
        SEND_CMD = 2,  // 发送的是指令
        SEND_ERROR = 3 // 标识错误
};

// file的类型
enum DataType {
        STRING_DATA = 1, // 发送的是文本格式
        JSON_DATA = 2,   // 发送的是json格式
        IMAGE_DATA = 3,  // 发送的是image格式
};

// cmd的类型
enum CmdType {
        STRING_MASSAGE = 1, // 发送的是普通的string消息
};

// ERROR的类型
enum ErrorType {
        FORMAT_ERROR = 1,        // 发送的文件格式错误
        SERVER_SYSTEM_ERROR = 2, // 服务器系统错误
};

// 一个存放具体消息格式的union，四个参数只用一个字节
union DetailType {
        uint8_t value; // 用于给union赋值和获取union值
        DataType data_type;
        CmdType cmd_type;
        ErrorType error_type;
};
/****************************************
 *	jdt头部结构
 ***************************************/
// ps:最好不要直接将JdtHead结构写入，因为enum所占的字节数是不确定的。虽然一般默认是int大小，但不同平台不同编译器可能有所不同
struct Head {
        ServiceType service_type; // 1个字节，服务的类型
        DetailType detail_type; // 1个字节，根据service_type不同，具有不同的语义
        uint16_t crc16; // 2个字节，消息的校验码
        uint16_t seq;   // 2个字节，消息的序号
        uint16_t ack;   // 2个字节，消息的确认号
        uint32_t len;   // 4个字节，消息的总长度
};
const int HeadSize = 12;

/****************************************
 *	jdt消息结构
 ***************************************/
// 消息的基类，用于被继承后以支持任意数据类型
class Body {
      public:
        Head head_;
};

// 真正存放消息的结构，
template <typename T> class ExtendBody : public Body {
      public:
        T getData() { return data_; }
        void setData(T data) { data_ = data; }
        void setHead(Head head) { head_ = head; }

      private:
        T data_;
};
} // namespace jdt

#endif
