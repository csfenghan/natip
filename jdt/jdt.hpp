/************************************************************
 * 在我的Jetson nano,Dell,Tencent间用于通信的协议(Jdt)
 ************************************************************/
#ifndef NTP_HPP
#define NTP_HPP

#include <deque>
#include <iostream>
#include <memory>
#include <queue>
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
#define TYPE_UNDEFINE 0 // 未定义类型
#define TYPE_JSON 1     // json数据
#define TYPE_IMAGE 2    // image数据
#define TYPE_STRING 3   // string数据

// (3)协议头数据格式(实际上用不到，只是规范数据的格式，数据分别以magic、version、service、data_len顺序存放)
struct MsgHead {
        uint8_t service; // 服务的类型(1字节)
        uint8_t type;    // 数据的类型(1字节)
        uint16_t crc;    // 校验码
        uint32_t len;    // 数据段的长度(4字节)
};

// 2.消息体（支持多种数据类型）
// 消息的基本类型，提供获取数据类型、提供数据等只读接口
class MsgBody {
      public:
        MsgBody() : type_(TYPE_UNDEFINE), valid_(false) {}
        ~MsgBody() {}

        // 获取数据类型
        int getType() { return type_; }

        // 数据是否有效
        bool isValid() { return valid_; }

      protected:
        int type_;   // 数据的类型
        bool valid_; // 数据是否有效（可能发生损坏、丢失等导致）
};
using Value = MsgBody;

// 存放不同类型数据的类，负责写入数据的具体内容及其有效性
template <typename T> class ExtendMsgBody : public MsgBody {
      public:
        ExtendMsgBody() : MsgBody() {} // 初始化时必须指定类型
        ExtendMsgBody(const T &data, int type, bool is_valid) : data_(data) {}

        // 设置数据
        void setData(const T &data) { data_ = data; }

        // 读取数据
        T getData() { return data_; }

        // 设置type
        void setType(int type) { type_ = type; }

        // 设置有效性
        void setValid(bool is_valid) { valid_ = is_valid; }

      private:
        T data_;
};

/***************************************
 *	二、Encode
 **************************************/
class Encode {
      public:
        // 功能：编码string数据
        // 返回：返回一个pair结构，first参数是编码后数据的首地址，second是数据的长度
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
        Decode() { init(); }

        // 功能：解析收到的数据流
        // 描述：解析以data开头，长度为len的字节流。解析后的数据保存在data_parsed_中
        // 返回：如果长度len的字节流被全部解析，则返回true，否则返回false
        bool parse(uint8_t *data, uint32_t len);

        // 判断要取出的消息的类型
        bool nextIsString();
        bool nextIsJson();
        bool nextIsImage();

        // 按照对应的类型获取消息
        std::string getString();
        std::string popString();
#ifdef JSONCPP
        Json::Value getJson();
        Json::Value popJson();
#endif
#ifdef OPENCV

#endif

        // 查看是否为空
        bool empty();

        // 释放一个消息
        void pop();

        // 查看当前的消息长度
        size_t size();

      private:
        MsgHead curr_head_;                // 正在解析的消息的消息头
        std::deque<uint8_t> data_parsing_; // 当前正在解析的字节流
        std::queue<std::shared_ptr<Value>> data_parsed_; // 已结解析完的数据
        ParserStatus status_;                            // 当前的解析状态

        // 初始化解析器状态
        void init();

        // 功能：解析协议头
        // 描述：解析保存在data_parsing_中的数据，如果解析成功，则设置curr_head_中的参数，否则什么也不做
        // 描述：如果解析失败，则返回false；否则返回true
        bool parseHead();

        // 功能：解析协议体
        // 描述：解析保存在data_parsing_中的数据，如果解析成功，则将解析出的消息加入data_parsed_中，
        // 	并清空curr_head_和data_parsing_中已经解析的部分,如果解析失败，则什么也不做
        // 返回：如果解析失败，则返回false；否则返回true
        bool parseBody();

        // 解析jsoncpp数据(需要jsoncpp支持)
        std::shared_ptr<Value> parseJson();

        // 解析image(OpenCV)数据(需要opencv支持)
        std::shared_ptr<Value> parseImage();

        // 功能：解析string数据
        // 描述：以string的格式解析存放在data_parsing_中的数据，并设置数据的类型和其有效性
        // 返回：返回指向解析的数据的智能指针
        std::shared_ptr<Value> parseString();
};

} // namespace jdt
#endif
