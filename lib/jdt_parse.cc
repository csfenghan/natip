#include "jdt_exception.hpp"
#include "jdt_parse.hpp"
#include <string.h>

namespace jdt {

/**********************************************************************************************
 * class:Encode
 * description:encode the data to be sent
 *********************************************************************************************/
// encode string data
std::shared_ptr<uint8_t> Encode::encode(std::string data, uint32_t &len, ServiceType service,
                                        DetailType type) {
        uint8_t *ptr;

        len = HeadSize + data.size();
        std::shared_ptr<uint8_t> result(new uint8_t[len], [](uint8_t *p) { delete p; });
        // encode head
        ptr = result.get();
        encodeHead(ptr, len, service, type);

        // encode body
        ptr += HeadSize;
        memcpy(ptr, data.c_str(), data.size());

        return result;
}

#ifdef JSONCPP
// encode json data
std::shared_ptr<uint8_t> Encode::encode(Json::Value data, uint32_t &len,
                                       ServiceType service, DetailType type) {
        uint8_t *ptr;
        Json::FastWriter fwriter;
        std::string body;

        // convert json to string format
        body = fwriter.write(data);

        // allocate memory
        len = HeadSize + body.size();

        std::shared_ptr<uint8_t> result(new uint8_t[len],
                                        [](uint8_t *p) { delete p; });

        // encode head
        ptr = result.get();
        encodeHead(ptr, len, service, type);

        // encode body
        ptr += HeadSize;
        memcpy(ptr, body.c_str(), body.size());

        return result;
}

#endif

// encode head data
void Encode::encodeHead(uint8_t *ptr, uint32_t len, ServiceType service, DetailType type) {
        // set the service type
        *(uint8_t *)ptr = (uint8_t)service;
        ptr += 1;

        // set the detail transfer type
        *(uint8_t *)ptr = (uint8_t)type.value;
        ptr += 1;

        // set the crc16
        *(uint16_t *)ptr = 0;
        ptr += 2;

        // set the seq number
        *(uint16_t *)ptr = 0;
        ptr += 2;

        // set the ack number
        *(uint16_t *)ptr = 0;
        ptr += 2;

        // set the len of byte stream
        *(uint32_t *)ptr = len;
}

/***********************************************************************************************
 * class:Decode
 * description:decode the received data
 **********************************************************************************************/
Decode::Decode() { init(); }

// function:parse the received data
// description:parse a stream of bytes starting with data and of length
// len
bool Decode::parse(uint8_t *data, uint32_t len) {

        // save the data
        for (uint32_t i = 0; i < len; i++)
                data_parsing_.push_back(data[i]);

        try {
                // parse data
                while (!data_parsing_.empty()) {
                        // parse head
                        if (status_ == PARSING_INIT || status_ == PARSING_HEAD) {
                                if (!parseHead()) {
                                        return false;
                                }
                                status_ = PARSING_BODY;
                        }

                        // parse body
                        if (status_ == PARSING_BODY) {
                                if (!parseBody()) {
                                        return false;
                                }
                                status_ = PARSING_HEAD;
                        }
                }
        } catch (parse_error &e) {
                fprintf(stderr, "Error: file: %s, line: %d, function:%s\ndetail: %s\n",
                        e.getFile().data(), e.getLine(), e.getFile().data(), e.what());
                return false;
        }

        return true;
}

// check if it is empty
bool Decode::empty() const { return data_parsed_.empty(); }

// view the number of parsed data
size_t Decode::size() const { return data_parsed_.size(); }

// initialize
void Decode::init() { status_ = PARSING_INIT; }

// get a msg
Value Decode::front() { return Value(data_parsed_.front()); }
void Decode::pop() { data_parsed_.pop(); }

// hot many byters are needed for the message currently being parsed
uint32_t Decode::getLenStillNeed() const { return curr_head_.len - data_parsing_.size(); }

// function:parse head
// description:parse the data saved in data_parsing_,and if success,set
// curr_head_;
bool Decode::parseHead() {
        if (data_parsing_.size() < HeadSize)
                return false;

        uint8_t *ptr;

        ptr = &data_parsing_[0];

        // get service type
        switch (*(uint8_t *)ptr) {
        case 1:
                curr_head_.service_type = SEND_DATA;
                break;
        case 2:
                curr_head_.service_type = SEND_CMD;
                break;
        case 3:
                curr_head_.service_type = SEND_ERROR;
                break;
        default:
                throw parse_error("Error:parseHead() error,unknow service type", __FILE__, __LINE__,
                                  __FUNCTION__);
                break;
        }
        ptr += 1;

        // get detail type
        curr_head_.detail_type.value = *(uint8_t *)ptr;
        ptr += 1;

        // get crc16
        curr_head_.crc16 = *(uint16_t *)ptr;
        ptr += 2;

        // get seq number
        curr_head_.seq = *(uint16_t *)ptr;
        ptr += 2;

        // get ack number
        curr_head_.ack = *(uint16_t *)ptr;
        ptr += 2;

        // get len
        curr_head_.len = *(uint32_t *)ptr;

        return true;
}

// function:parse body
// description:parse the data saved in data_parsing_,if the parsing is
// successfule,clear data_parsing_ and add the new msg to data_parsed_;
bool Decode::parseBody() {
        if (data_parsing_.size() < curr_head_.len)
                return false;

        std::shared_ptr<Body> data;

        // parse body by service type
        if (curr_head_.service_type == SEND_DATA) {
                if (curr_head_.detail_type.data_type == JSON_DATA) {
#ifndef JSONCPP
                        throw parse_error("Error:parseBody() error,not found jsoncpp "
                                          "library,can't parse json file!",
                                          __FILE__, __LINE__, __FUNCTION__);
#endif
#ifdef JSONCPP
                        data = parseJson();
#endif
                } else if (curr_head_.detail_type.data_type == IMAGE_DATA) {
                        throw parse_error("Error:not support image type "
                                          "currently,parseBody() error!",
                                          __FILE__, __LINE__, __FUNCTION__);
                }
        } else if (curr_head_.service_type == SEND_CMD) {
                data = parseString();
        } else if (curr_head_.service_type == SEND_ERROR) {
                data = parseString();
        }

        // clean up the parsed parts of data_parsing_
        data_parsing_.erase(data_parsing_.begin(), data_parsing_.begin() + curr_head_.len);
        memset(&curr_head_, 0, sizeof(curr_head_));

        return true;
}

// parse string data
std::shared_ptr<Body> Decode::parseString() {
        auto data = std::make_shared<ExtendBody<std::string>>();

        data->setHead(curr_head_);
        data->setData(
            std::string(data_parsing_.begin() + HeadSize, data_parsing_.begin() + curr_head_.len));

        return data;
}

#ifdef JSONCPP
// parse json data
std::shared_ptr<Body> Decode::parseJson() {
        auto data = std::make_shared<ExtendBody<Json::Value>>();
        Json::Reader reader;
        Json::Value value;
        char *ptr;

        // parse the json data
        ptr = (char *)&data_parsing_[0];
        if (!reader.parse(ptr + HeadSize, ptr + curr_head_.len, value, true)) {
                throw parse_error("json data parse error!", __FILE__, __LINE__, __FUNCTION__);
        }

        // set the head
        data->setHead(curr_head_);
        data->setData(value);

        return data;
}
#endif

} // namespace jdt
