/******************************************
 *	exception class
 *****************************************/

#ifndef JDT_EXCEPTION
#define JDT_EXCEPTION

#include <stdexcept>
#include <string>

namespace jdt {

// parse format error
class parse_error : public std::logic_error {
      public:
        explicit parse_error(const std::string &msg, const std::string &file,
                             const int &line, const std::string &function)
            : std::logic_error(msg), file_(file), line_(line),
              function_(function) {}
        std::string getFile() { return file_; }
        int getLine() { return line_; }
        std::string getFunction() { return function_; }

      private:
        std::string file_;
        int line_;
        std::string function_;
};

} // namespace jdt

#endif
