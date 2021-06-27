/***********************************************************
 *	This file is responsible for encoding and decoding the data
 **********************************************************/

#ifndef JDT_PARSER_HPP
#define JDT_PARSER_HPP

#include "jdt_body.hpp"
#include "jdt_value.hpp"

#include <deque>
#include <memory>
#include <queue>

namespace jdt {

/**********************************************************************************************
 * class:Encode
 * description:encode the data to be sent
 *********************************************************************************************/
class Encode {
      public:
        // encode string data
        std::shared_ptr<uint8_t> encode(std::string data, uint32_t &len, ServiceType service,
                                        DetailType type);

#ifdef JSONCPP
        // encode json data
        std::shared_ptr<uint8_t> encode(Json::Value data, uint32_t &len, ServiceType service,
                                        DetailType type);
#endif

      private:
        // encode head data
        void encodeHead(uint8_t *ptr, uint32_t len, ServiceType service, DetailType type);
};

/***********************************************************************************************
 * class:Decode
 * description:decode the received data
 **********************************************************************************************/
class Decode {
        enum ParserStatus {
                PARSING_INIT = 1, // Initial state
                PARSING_HEAD = 2, // Head being pared
                PARSING_BODY = 3, // Body being pared
        };

      public:
        Decode();

        // function:parse the received data
        // description:parse a stream of bytes starting with data and of length
        // len
        bool parse(uint8_t *data, uint32_t len);

        // check if it is empty
        bool empty() const;

        // view the number of parsed data
        size_t size() const;

        // get a msg
        Value front();
        void pop();

        // hot many byters are needed for the message currently being parsed
        uint32_t getLenStillNeed() const;

      private:
        Head curr_head_;                   // message header for the data currently being parsed
        std::deque<uint8_t> data_parsing_; // data currently being parsed
        std::queue<std::shared_ptr<Body>> data_parsed_; // parsed data
        ParserStatus status_;                           // current parsing status

        // initialize
        void init();

        // function:parse head
        // description:parse the data saved in data_parsing_,and if success,set
        // curr_head_;
        bool parseHead();

        // function:parse body
        // description:parse the data saved in data_parsing_,if the parsing is
        // successfule,clear data_parsing_ and add the new msg to data_parsed_;
        bool parseBody();

        // parse json data
        std::shared_ptr<Body> parseJson();

        // parse string data
        std::shared_ptr<Body> parseString();
};

} // namespace jdt

#endif
