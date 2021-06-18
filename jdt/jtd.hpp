/************************************************************
 * 在我的Jetson nano,Dell,Tencent间用于通信的协议(Jtd)
 ************************************************************/
#ifndef NTP_HPP
#define NTP_HPP

#include <deque>
#include <iostream>
#include <utility>

// 是否支持以下接口
#ifdef JSONCPP
#include "jsoncpp/json/json.h"
#endif

#ifdef OPENCV
//#include "opencv2/opencv.hpp"
#endif

/***************************************
 *	一、 消息体参数
 **************************************/
// 1.支持的服务类型
#define SERVICE_JSON 1  //传输的是json文件(需要支持jsoncpp库)
#define SERVICE_IMAGE 2 //传输的是image文件(需要支持opencv库)

// 2. 其他参数
#define JTD_MAGIC 9999  // 魔数
#define JTD_HEAD_SIZE 8 // 头部大小8字节

// 3.协议头
struct JdtHead {
        uint16_t magic;    // 魔数
        uint8_t version;   // 版本号
        uint8_t service;   // 请求的服务
        uint32_t data_len; // 数据段的长度
};

// 4.消息体（支持多种数据类型）


/***************************************
 *	二、JdtEncode
 **************************************/
class JdtEncode {
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
 *	三、JdtDecode
 **************************************/
class JdtDecode {
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

#endif
