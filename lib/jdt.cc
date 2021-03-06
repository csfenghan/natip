#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include "unix_api.h"

#include <arpa/inet.h>
#include <string.h>
#include <string>

namespace jdt {

/***************************************
 *	MsgBody实现
 **************************************/
// 以string类型获取数据
const std::string Msg::asString() const {
        if (!isString())
                err_ret("this data is not string format!");
        ExtendMsgBody<std::string> *ptr;

        ptr = (ExtendMsgBody<std::string> *)data_.get();
        return ptr->getData();
}
#ifdef JSONCPP
const Json::Value Msg::asJson() const {
        if (!isJson())
                err_ret("this data is not json format!");
        ExtendMsgBody<Json::Value> *ptr;

        ptr = (ExtendMsgBody<Json::Value> *)data_.get();
        return ptr->getData();
}
#endif
const std::string Msg::asError() const {
        if (!isError())
                err_ret("this msg is not error data!");

        ExtendMsgBody<std::string> *ptr;

        ptr = (ExtendMsgBody<std::string> *)data_.get();
        return ptr->getData();
}

/***************************************
 *	JdtEncode实现
 **************************************/
// 功能：编码string数据
// 返回：返回一个pair结构，first参数是编码后数据的首地址，second是数据的长度
std::pair<uint8_t *, uint32_t> Encode::encode(const std::string &data) {
        std::pair<uint8_t *, uint32_t> result;
        uint32_t len;
        uint8_t *ptr;

        // 设置协议内容
        len = HEAD_SIZE + data.size();
        result.first = new uint8_t[len];
        result.second = len;
        ptr = result.first;

        encodeHead(ptr, SERVICE_SEND, TYPE_STRING, len);
        ptr += HEAD_SIZE;
        memcpy(ptr, data.data(), data.size());

        return result;
}

// 编码jsoncpp数据
#ifdef JSONCPP
std::pair<uint8_t *, uint32_t> Encode::encode(const Json::Value &data) {
        std::pair<uint8_t *, uint32_t> result;
        Json::FastWriter fwriter;
        std::string body;
        uint32_t len;
        uint8_t *ptr;

        // 1.将data转换成字符格式
        body = fwriter.write(data);
        len = HEAD_SIZE + body.size();

        result.first = new uint8_t[len]; //使用完毕后一定要release
        result.second = len;

        // 2.设置头部字段（为了防止大小端不同，需要进行网络字节序转换）
        ptr = result.first;
        encodeHead(ptr, SERVICE_SEND, TYPE_JSON, len);

        // 3.放置data数据
        ptr += HEAD_SIZE;
        memcpy(ptr, body.data(), body.size());

        return result;
}
#endif

// 释放分配的资源
void Encode::release(const std::pair<uint8_t *, uint32_t> &data) { delete data.first; }

// 按照struct MsgHead顺序设置协议头
void Encode::encodeHead(uint8_t *ptr, int service, int type, int len) {
        *(uint8_t *)ptr = service;
        ptr += 1;
        *(uint8_t *)ptr = type;
        ptr += 1;
        *(uint16_t *)ptr = 0;
        ptr += 2;
        *(uint32_t *)ptr = htonl(len);
}

/***************************************
 *	JdtDecode实现
 **************************************/
// 构造函数
Decode::Decode() { init(); }

// 初始化解析状态
void Decode::init() { status_ = PARSING_INIT; }

// 查看是否为空
bool Decode::empty() { return data_parsed_.empty(); }

// 查看当前的消息长度
size_t Decode::size() { return data_parsed_.size(); }

// 获取一个消息
const Msg Decode::getOneMsg(bool is_release) {
        Msg result(data_parsed_.front());

        if (is_release)
                releaseOneMsg();
        return result;
}

// 释放一个消息
void Decode::releaseOneMsg() { data_parsed_.pop(); }

// 功能：解析收到的数据流
// 描述：解析以data开头，长度为len的字节流。解析后的数据保存在data_parsed_中
// 返回：如果长度len的字节流被全部解析，则返回true，否则返回false
bool Decode::parse(uint8_t *data, uint32_t len) {
        // 保存数据
        for (uint32_t i = 0; i < len; i++)
                data_parsing_.push_back(data[i]);

        // 解析len字节的数据
        while (!data_parsing_.empty()) {
                if (status_ == PARSING_INIT || status_ == PARSING_HEAD) {
                        if (!parseHead()) {
                                return false;
                        }
                        status_ = PARSING_BODY;
                }
                if (status_ == PARSING_BODY) {
                        if (!parseBody()) {
                                return false;
                        }
                        status_ = PARSING_HEAD;
                }
        }

        return true;
}

// 功能：解析协议头
// 描述：解析保存在data_parsing_中的数据，如果解析成功，则设置curr_head_中的参数，否则什么也不做
// 描述：如果解析失败，则返回false；否则返回true
bool Decode::parseHead() {
        if (data_parsing_.size() < HEAD_SIZE)
                return false;

        uint8_t *ptr;

        ptr = &data_parsing_[0];
        curr_head_.service = *(uint8_t *)ptr;
        ptr += 1;
        curr_head_.type = *(uint8_t *)ptr;
        ptr += 1;
        curr_head_.crc = *(uint16_t *)ptr;
        ptr += 2;
        curr_head_.len = ntohl(*(uint32_t *)ptr);

        return true;
}

// 功能：解析协议体
// 描述：解析保存在data_parsing_中的数据，如果解析成功，则将解析出的消息加入data_parsed_中，
// 	并清空curr_head_和data_parsing_中已经解析的部分,如果解析失败，则什么也不做
// 返回：如果解析失败，则返回false；否则返回true
bool Decode::parseBody() {
        if (data_parsing_.size() < curr_head_.len)
                return false;

        bool is_error;
        std::shared_ptr<MsgBody> msg;

        is_error = false;
        // 根据消息的type类型来选择对应的数据结构，并判断是否支持这种数据类型
        switch (curr_head_.type) {

        // json类型数据，需要jsoncpp库支持
        case TYPE_JSON:
#ifdef JSONCPP
                msg = parseJson();
                break;
#endif
                fprintf(stderr, "can't recognize the type JSONCPP,you need "
                                "jsoncpp library\n");
                is_error = true;
                break;

        // image类型数据，需要opencv库支持
        case TYPE_IMAGE:
#ifdef OPENCV
                msg = parseImage();
                break;
#endif
                fprintf(stderr,
                        "can't recognize the type IMAGE,you need OpenCV library\n");
                is_error = true;
                break;

                // string类型数据
        case TYPE_STRING:
                msg = parseString();
                break;
        default:
                break;
        }

        // 如果正确，加入data_parsed_中
        if (msg->isValid()) {
                data_parsed_.push(msg);
        } else {
                is_error = true;
        }

        // 清除data_parsing_中已经解析或无效的部分的部分，清空curr_head_;
        data_parsing_.erase(data_parsing_.begin(),
                            data_parsing_.begin() + curr_head_.len);
        memset(&curr_head_, 0, sizeof(curr_head_));

        if (is_error)
                return false;
        else
                return true;
}
// 解析jsoncpp数据
#ifdef JSONCPP
std::shared_ptr<MsgBody> Decode::parseJson() {
        auto msg = std::make_shared<ExtendMsgBody<Json::Value>>();
        Json::Reader reader;
        Json::Value value;
        char *ptr;

        ptr = (char *)&data_parsing_[0];
        msg->setType(TYPE_JSON);

        if (!reader.parse(ptr + HEAD_SIZE, ptr + curr_head_.len, value, false)) {
                msg->setValid(false);
                return msg;
        }
        msg->setData(value);
        msg->setValid(true);

        return msg;
}
#endif

// 解析image(OpenCV)数据
#ifdef OPENCV
// std::shared_ptr<MsgBody> Decode::parseImage() {}
#endif

// 功能：解析string数据
// 描述：以string的格式解析存放在data_parsing_中的数据，并设置数据的类型和其有效性
// 返回：返回指向解析的数据的智能指针
std::shared_ptr<MsgBody> Decode::parseString() {
        auto msg = std::make_shared<ExtendMsgBody<std::string>>();

        msg->setData(std::string(data_parsing_.begin() + HEAD_SIZE,
                                 data_parsing_.begin() + curr_head_.len));
        msg->setValid(true);
        msg->setType(TYPE_STRING);

        return msg;
}

/***************************************
 *	Jdt实现
 **************************************/
void Jdt::setFd(int fd) {
        // 先关闭之前的描述符
        if (!isInit())
                close(socket_fd_);
        socket_fd_ = fd;
}

void Jdt::sendMsg(const std::string &data) {
        if (!isInit())
                err_sys("can't use uninitialized fd");
        Encode encode;

        auto data_encoded = encode.encode(data);
        Rio_writen(socket_fd_, data_encoded.first, data_encoded.second);
}

#ifdef JSONCPP
void Jdt::sendMsg(const Json::Value &data) {
        if (!isInit())
                err_sys("can't use uninitialized fd");
        Encode encode;

        auto data_encoded = encode.encode(data);
        Rio_writen(socket_fd_, data_encoded.first, data_encoded.second);
}
#endif

const Msg Jdt::recvMsg() {
        if (!isInit())
                err_sys("can't use uninitialized fd");
        char buf[MAXLINE];
        int n;

        while (decode_.empty()) {
                n = Rio_readn(socket_fd_, buf, MAXLINE);
                decode_.parse((uint8_t *)buf, n);
        }
        if (decode_.empty())
                return Msg();
        return Msg(decode_.getOneMsg(true));
}

void Jdt::release() {
        if (isInit())
                close(socket_fd_);
}

bool Jdt::isInit() const {
        if (socket_fd_ == -1)
                return false;
        return true;
}

} // namespace jdt
