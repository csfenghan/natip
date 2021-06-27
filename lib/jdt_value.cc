#include "jdt_value.hpp"

namespace jdt {
// 判断service的类型，是file还是cmd还是error
bool Value::isData() { return data_->head_.service_type == SEND_DATA; }
bool Value::isCmd() { return data_->head_.service_type == SEND_CMD; }
bool Value::isError() { return data_->head_.service_type == SEND_ERROR; }
ServiceType Value::getServiceType() { return data_->head_.service_type; }

// 1. file类型专属的操作
bool Value::isJsonData() {
        return data_->head_.detail_type.data_type == JSON_DATA;
}
bool Value::isImageData() {
        return data_->head_.detail_type.data_type == IMAGE_DATA;
}
DataType Value::getDataType() { return data_->head_.detail_type.data_type; }

#ifdef JSONCPP // 获取json标准数据
Json::Value Value::asJson() {
        ExtendBody<Json::Value> *ptr;

        ptr = (ExtendBody<Json::Value> *)data_.get();
        return ptr->getData();
}
#endif

#ifdef OPENCV
cv::Mat asImage(); // 获取opencv标准数据
#endif

// 2. cmd类型专属的操作
bool Value::isMessageCmd() {
        return data_->head_.detail_type.cmd_type == STRING_MASSAGE;
}
CmdType Value::getCmdType() { return data_->head_.detail_type.cmd_type; }
std::string Value::asMessageCmd() {
        ExtendBody<std::string> *ptr;

        ptr = (ExtendBody<std::string> *)data_.get();
        return ptr->getData();
}

// 3. error类型专属的操作
bool Value::isFormatError() {
        return data_->head_.detail_type.error_type == FORMAT_ERROR;
}
ErrorType Value::getErrorType() { return data_->head_.detail_type.error_type; }
std::string Value::asFormatError() {
        ExtendBody<std::string> *ptr;

        ptr = (ExtendBody<std::string> *)data_.get();
        return ptr->getData();
}

} // namespace jdt
