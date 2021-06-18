#include "jdt.hpp"
#include "jsoncpp/json/json.h"

#include <arpa/inet.h>
#include <string.h>
#include <string>

namespace jdt {

/***************************************
 *	一、JdtEncode实现
 **************************************/
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

        *(uint16_t *)ptr = htons(MAGIC);
        ptr += 2;
        *(uint8_t *)ptr = 1;
        ptr += 1;
        *(uint8_t *)ptr = SERVICE_JSON;
        ptr += 1;
        *(uint32_t *)ptr = htonl(len);
        ptr += 4;

        // 3.放置data数据
        memcpy(ptr, body.data(), body.size());

        return result;
}
#endif

void Encode::release(const std::pair<uint8_t *, uint32_t> &data) {
        delete data.first;
}

/***************************************
 *	二、JdtDecode实现
 **************************************/

bool Decode::parse(uint8_t *data, uint32_t len) {
        for (uint32_t i = 0; i < len; i++)
                data_parsing_.push_back(data[i]);

        return true;
}

bool Decode::parseHead(uint8_t *data, uint32_t len) {
        if (len < HEAD_SIZE)
                return false;

        return true;
}
} // namespace jdt
