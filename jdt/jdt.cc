#include "jdt.hpp"
#include "jsoncpp/json/json.h"

#include <arpa/inet.h>
#include <string.h>
#include <string>

namespace jdt {

/***************************************
 *	一、JdtEncode实现
 **************************************/
// 编码string数据
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
 *	二、JdtDecode实现
 **************************************/

// 初始化解析状态
void Decode::init() { status_ = PARSING_INIT; }

// 解析收到的数据流
bool Decode::parse(uint8_t *data, uint32_t len) {
        uint8_t *ptr;
        uint32_t len_reserved;

        // 保存数据
        for (uint32_t i = 0; i < len; i++)
                data_parsing_.push_back(data[i]);

        ptr = &data_parsing_[0];
        len_reserved = data_parsed_.size();

        // 解析len字节的数据
        while (len_reserved > 0) {
                if (status_ == PARSING_INIT || status_ == PARSING_HEAD) {
                        if (!parseHead(ptr, len_reserved)) {
                                return false;
                        }
                        ptr += HEAD_SIZE;
                        len_reserved -= HEAD_SIZE;
                        status_ = PARSING_BODY;
                }
                if (status_ == PARSING_BODY) {
                }
        }

        return true;
}

// 解析协议头
// 如果解析失败，则返回false；否则返回true
bool Decode::parseHead(uint8_t *data, uint32_t len) {
        if (len < HEAD_SIZE)
                return false;
        curr_head_.service = *(uint8_t *)data;
        data += 1;
        curr_head_.type = *(uint8_t *)data;
        data += 1;
        curr_head_.crc = *(uint16_t *)data;
        data += 2;
        curr_head_.len = ntohl(*(uint32_t *)data);

        return true;
}
} // namespace jdt
