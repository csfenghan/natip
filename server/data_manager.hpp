#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include <iostream>
#include <string>

namespace natip {
// 用户信息的数据类型
class ClientData {
      public:
        // 打印数据
        void printClientData();

        // 设置所有的数据
        void setAllData(std::string name, std::string addr, int port, int pid,
                        std::string info);
        // 读取数据
        std::string getName();
        std::string getAddr();
        int getPort();
        int getPid();
        std::string getInfo();

        // 写入数据
        void setName(std::string name);
        void setAddr(std::string addr);
        void setPort(int port);
        void setPid(int pid);
        void setInfo(std::string info);

      private:
        std::string name_;
        std::string addr_;
        uint16_t port_;
        pid_t pid_;
        std::string info_;
};
/*****************************************************
 * 数据库接口
 ****************************************************/

// 将data写入数据库
bool write_to_mysql(ClientData &data);

// 从数据库总读取对应name的数据
bool read_from_mysql(ClientData &data, std::string name);

// 从数据库总读取对应pid的数据
bool read_from_mysql(ClientData &data, int pid);

// 从数据库删除对应name的数据
bool del_from_mysql(std::string name);

// 从数据库中删除对应pid的数据
bool del_from_mysql(int pid);
} // namespace natip

#endif
