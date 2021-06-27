/*****************************************************************
 * 	本文件定义了jdt接收数据所使用的基本数据类型，接收到的消息
 * 	都被转换成了jdt::JdtValue的对象
 *****************************************************************/

#ifndef JDT_VALUE_HPP
#define JDT_VALUE_HPP

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "jdt_body.hpp"

#ifdef JSONCPP
#include "jsoncpp/json/json.h"
#endif

#ifdef OPENCV
#include "opencv2/opencv.hpp"
#endif

namespace jdt {

// 消息被取出后的类型
class Value : public Body {
      public:
        Value(std::shared_ptr<Body> data) : data_(data) {}
        // 判断service的类型，是file还是cmd还是error
        bool isData();
        bool isCmd();
        bool isError();
        ServiceType getServiceType();

        // 1. data类型专属的操作
        bool isJsonData();
        bool isImageData();
        DataType getDataType();

#ifdef JSONCPP
        Json::Value asJson(); // 获取json标准数据
#endif
#ifdef OPENCV
        cv::Mat asImage(); // 获取opencv标准数据
#endif

        // 2. cmd类型专属的操作
        bool isMessageCmd();
        CmdType getCmdType();
        std::string asMessageCmd();

        // 3. error类型专属的操作
        bool isFormatError();
        ErrorType getErrorType();
        std::string asFormatError();

      private:
        // 存放指向基类型的智能指针，使用时进行类型强制转换以获取真正的数据
        std::shared_ptr<Body> data_;
};

} // namespace jdt

#endif
