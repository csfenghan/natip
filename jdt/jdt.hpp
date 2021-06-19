/************************************************************
 * 在我的Jetson nano,Dell,Tencent间用于通信的协议(Jdt)
 ************************************************************/
#ifndef NTP_HPP
#define NTP_HPP

#include <deque>
#include <iostream>
#include <utility>

// 是否支持jsoncpp库
#ifdef JSONCPP
#include "jsoncpp/json/json.h"
#endif

// 是否支持opencv库
#ifdef OPENCV
//#include "opencv2/opencv.hpp"
#endif

namespace jdt {

/***************************************
 *	一、 消息体参数
 **************************************/
// 1.协议头以及其用到的参数
// (1) 魔数、版本号、头部大小
#define HEAD_SIZE 8 // 头部大小8字节

// (2) 服务的类型
#define SERVICE_SEND 1 //发送
#define SERVICE_REQ 2  //申请接收

// (3) 数据的类型
#define TYPE_JSON 1  // json数据
#define TYPE_IMAGE 2 // image数据

// (3)协议头数据格式(实际上用不到，只是规范数据的格式，数据分别以magic、version、service、data_len顺序存放)
struct MsgHead {
        uint8_t service; // 服务的类型(1字节)
        uint8_t type;    // 数据的类型(1字节)
        uint16_t crc;    // 校验码
        uint32_t len;    // 数据段的长度(4字节)
};

// 2.消息体（支持多种数据类型）
class MsgBaseBody {
      public:
        ~MsgBaseBody() {}
};
template <typename T> class MsgBody : public MsgBaseBody {
      public:
        MsgBody() {}
        MsgBody(const T &data, int type) : data_(data), type_(type) {}

      private:
        T data_;
        int type_;
};

/***************************************
 *	二、Encode
 **************************************/
class Encode {
      public:
        // (1)jsoncpp文件传输（需要有jsoncpp库）
#ifdef JSONCPP
        std::pair<uint8_t *, uint32_t> encode(const Json::Value &data);
#endif

        // (2)opencv image文件传输（需要有opencv库）
#ifdef OPENCV
        //       std::pair<uint8_t *, uint32_t> encode(const cv::Mat &img);

#endif
        // (3)销毁new的资源
        void release(const std::pair<uint8_t *, uint32_t> &data);
};

/***************************************
 *	三、Decode
 **************************************/
class Decode {
        enum ParserStatus {
                PARSEING_INIT = 1, // 协议初始化
                PARSEING_HEAD = 2, // 正在解析头部
                PARSEING_BODY = 3, // 正在解析数据
        };

      public:
        void init();
        bool parse(uint8_t *data, uint32_t len);

      private:
        std::deque<uint8_t> data_parsing_;

        bool parseHead(uint8_t *data, uint32_t len);
};

} // namespace jdt
#endif
