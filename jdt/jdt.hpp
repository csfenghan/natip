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
// (1) 头部大小
#define HEAD_SIZE 8 // 头部大小8字节

// (2) 服务的类型
#define SERVICE_SEND 1 //发送
#define SERVICE_REQ 2  //申请接收

// (3) 数据的类型
#define TYPE_JSON 1   // json数据
#define TYPE_IMAGE 2  // image数据
#define TYPE_STRING 3 // string数据

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
        MsgBaseBody(int type) : type_(type) {}
        virtual ~MsgBaseBody() {}

        inline int getType() { return type_; }

      private:
        int type_;
};
template <typename T> class MsgBody : public MsgBaseBody {
      public:
        MsgBody(const int &type) : MsgBaseBody(type) {}
        MsgBody(const T &data, const int &type) : MsgBaseBody(type), data_(data) {}

        void setData(const T &data) { data_ = data; }

      private:
        T data_;
};

/***************************************
 *	二、Encode
 **************************************/
class Encode {
      public:
        // 传输string字符串
        std::pair<uint8_t *, uint32_t> encode(const std::string &data);

        // jsoncpp文件传输（需要有jsoncpp库）
#ifdef JSONCPP
        std::pair<uint8_t *, uint32_t> encode(const Json::Value &data);
#endif

        // opencv image文件传输（需要有opencv库）
#ifdef OPENCV
        //       std::pair<uint8_t *, uint32_t> encode(const cv::Mat &img);

#endif
        // 销毁new的资源
        void release(const std::pair<uint8_t *, uint32_t> &data);

      private:
        void encodeHead(uint8_t *ptr, int service, int type, int len);
};

/***************************************
 *	三、Decode
 **************************************/
class Decode {
        enum ParserStatus {
                PARSING_INIT = 1, // 协议初始化
                PARSING_HEAD = 2, // 正在解析头部
                PARSING_BODY = 3, // 正在解析数据
        };

      public:
        // 初始化解析器状态
        void init();

        // 功能：解析收到的数据流
        // 描述：解析以data开头，长度为len的字节流。解析后的数据保存在data_parsed_中，可以通过front()获取
        // 返回：如果长度len的字节流被全部解析，则返回true，否则返回false
        bool parse(uint8_t *data, uint32_t len);

        // 获取消息
        MsgBaseBody front();
        MsgBaseBody back();

        // 查看是否为空
        bool empty();

        // 释放一个消息
        void pop_front();
        void pop_back();

        // 查看当前的消息长度
        size_t size();

      private:
        MsgHead curr_head_;                   // 正在解析的消息的消息头
        std::deque<uint8_t> data_parsing_;    // 当前正在解析的字节流
        std::deque<MsgBaseBody> data_parsed_; // 已经解析完的数据
        ParserStatus status_;                 // 当前的解析状态

        // 功能：解析协议头
        // 描述：解析保存在data_parsing_中的数据，如果解析成功，则设置curr_head_中的参数，否则什么也不做
        // 描述：如果解析失败，则返回false；否则返回true
        bool parseHead();

        // 功能：解析协议体
        // 描述：解析保存在data_parsing_中的数据，如果解析成功，则将解析出的消息加入data_parsed_中，
        // 	并清空curr_head_和data_parsing_中已经解析的部分,如果解析失败，则什么也不做
        // 返回：如果解析失败，则返回false；否则返回true
        bool parseBody();
};

} // namespace jdt
#endif
