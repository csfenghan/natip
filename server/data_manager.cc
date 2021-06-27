
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "data_manager.hpp"
#include "mysql_connection.h"

#include <iostream>

namespace natip {

/*******************************************
 * class ClientData
 ******************************************/
// 打印数据
void ClientData::printClientData() {
        printf("data:\n\tname:%s\n\taddr:%s\n\tport::%d\n\tpid:%"
               "d\n\tinfo:%s\n\n",
               getName().data(), getAddr().data(), getPort(), getPid(), getInfo().data());
}

// 设置所有的数据
void ClientData::setAllData(std::string name, std::string addr, int port, int pid,
                            std::string info) {
        setName(name);
        setAddr(addr);
        setPort(port);
        setPid(pid);
        setInfo(info);
}
// 读取数据
std::string ClientData::getName() { return name_; }
std::string ClientData::getAddr() { return addr_; }
int ClientData::getPort() { return port_; }
int ClientData::getPid() { return pid_; }
std::string ClientData::getInfo() { return info_; }

// 写入数据
void ClientData::setName(std::string name) { name_ = name; }
void ClientData::setAddr(std::string addr) { addr_ = addr; }
void ClientData::setPort(int port) { port_ = port; }
void ClientData::setPid(int pid) { pid_ = pid; }
void ClientData::setInfo(std::string info) { info_ = info; }

/*******************************************
 * 数据库接口
 ******************************************/

// 管理用户信息
enum ServiceType {
        ADD_CLIENT = 1,     // 添加用户信息
        DEL_CLIENT = 2,     // 删除用户信息
        FIND_CLIENT = 3,    // 寻找用户信息
        TRAVERS_CLIENT = 4, // 遍历用户信息
};

// 说明：根据service的类型执行对应的功能
// 参数：service:
// 		ADD_CLIENT:将data的内容写入数据库
// 		DEL_CLIENT:如果pid被设置，则删除数据库中pid对应的数据，否则删除name对应的数据
// 		FIND_CLIENT:同DEL_CLIENT，不过是将返回的数据写入data中
static bool manageMySQL(ServiceType service, ClientData &data, std::string name = std::string(),
                        int pid = -1) {
        bool result = true;
        sql::Driver *driver = nullptr;
        sql::Connection *con = nullptr;
        sql::Statement *stmt = nullptr;
        sql::ResultSet *res = nullptr;

        char sql_str[1024];

        try {
                // 初始化连接
                driver = get_driver_instance();
                con = driver->connect("127.0.0.1:3306", "root", "");
                con->setSchema("natip");
                stmt = con->createStatement();

                // 如果表不存在，则创建表
                snprintf(sql_str, sizeof(sql_str),
                         "CREATE TABLE IF NOT EXISTS client_data(name CHAR(64) NOT "
                         "NULL, addr CHAR(64) NOT NULL,port INT NOT NULL,pid INT "
                         "UNIQUE NOT NULL , info VARCHAR(128) DEFAULT NULL, PRIMARY "
                         "KEY(name)) ENGINE =InnoDB "
                         "DEFAULT CHARSET = utf8 ");
                stmt->execute(sql_str);

                // 执行操作
                switch (service) {
                case ADD_CLIENT: // 添加
                        snprintf(sql_str, sizeof(sql_str),
                                 "INSERT INTO client_data "
                                 "VALUES('%s','%s',%d,%d,'%s')",
                                 data.getName().c_str(), data.getAddr().c_str(), data.getPort(),
                                 data.getPid(), data.getInfo().c_str());
                        stmt->execute(sql_str);
                        break;
                case DEL_CLIENT: // 删除
                        if (pid != -1)
                                snprintf(sql_str, sizeof(sql_str),
                                         "DELETE FROM client_data WHERE pid=%d", pid);
                        else
                                snprintf(sql_str, sizeof(sql_str),
                                         "DELETE FROM client_data WHERE name='%s'", name.c_str());
                        stmt->execute(sql_str);

                        break;
                case FIND_CLIENT: // 搜索
                        if (pid != -1)
                                snprintf(sql_str, sizeof(sql_str),
                                         "SELECT * FROM client_data WHERE pid=%d", pid);
                        else
                                snprintf(sql_str, sizeof(sql_str),
                                         "SELECT * FROM client_data WHERE name='%s'", name.data());
                        res = stmt->executeQuery(sql_str);

                        if (res->next()) {
                                data.setAllData(res->getString("name"), res->getString("addr"),
                                                res->getInt("port"), res->getInt("pid"),
                                                res->getString("info"));
                        } else {
                                result = false;
                        }
                        break;
                case TRAVERS_CLIENT:
                        break;
                default:
                        break;
                }
        } catch (sql::SQLException &e) {
                std::cout << "Error: SQLException in " << __FILE__;
                std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
                std::cout << "Error: " << e.what();
                std::cout << "(MySQL error code: " << e.getErrorCode() << " )";
                std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
                result = false;
        }

        // 释放内存(有可能再分配第一个的内存时就出错了，所以要检测一下)
        if (res)
                delete res;
        if (con)
                delete con;
        if (stmt)
                delete stmt;

        return result;
}

// 将data写入数据库
bool write_to_mysql(ClientData &data) { return manageMySQL(ADD_CLIENT, data); }

// 从数据库总读取对应name的数据
bool read_from_mysql(ClientData &data, std::string name) {
        return manageMySQL(FIND_CLIENT, data, name);
}

bool read_from_mysql(std::vector<ClientData> &data) {
        sql::Driver *driver = nullptr;
        sql::Connection *con = nullptr;
        sql::Statement *stmt = nullptr;
        sql::ResultSet *res = nullptr;

        char sql_str[1024];

        // 初始化连接
        driver = get_driver_instance();
        con = driver->connect("127.0.0.1:3306", "root", "");
        con->setSchema("natip");
        stmt = con->createStatement();

        // 如果表不存在，则创建表
        snprintf(sql_str, sizeof(sql_str),
                 "CREATE TABLE IF NOT EXISTS client_data(name CHAR(64) NOT "
                 "NULL, addr CHAR(64) NOT NULL,port INT NOT NULL,pid INT "
                 "UNIQUE NOT NULL , info VARCHAR(128) DEFAULT NULL, PRIMARY "
                 "KEY(name)) ENGINE =InnoDB "
                 "DEFAULT CHARSET = utf8 ");
        stmt->execute(sql_str);
        snprintf(sql_str, sizeof(sql_str), "SELECT * FROM client_data");
        res = stmt->executeQuery(sql_str);

        while (res->next()) {
                ClientData cd;
                cd.setAllData(res->getString("name"), res->getString("addr"), res->getInt("port"),
                              res->getInt("pid"), res->getString("info"));
                data.push_back(cd);
        }
        return true;
}

// 从数据库总读取对应pid的数据
bool read_from_mysql(ClientData &data, int pid) {
        return manageMySQL(FIND_CLIENT, data, std::string(), pid);
}

// 从数据库删除对应name的数据
bool del_from_mysql(std::string name) {
        ClientData data;
        return manageMySQL(DEL_CLIENT, data, name);
}
// 从数据库中删除对应pid的数据
bool del_from_mysql(int pid) {
        ClientData data;
        return manageMySQL(DEL_CLIENT, data, std::string(), pid);
}
} // namespace natip
