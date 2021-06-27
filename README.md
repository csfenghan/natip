## 内网穿透
在我的jetson、dell和工作台式机间实现内网穿透和数据通信的工具

## 服务端安装
### 安装依赖包

    sudo apt install mysql-server libmysqlcppconn-dev libjsoncpp

    然后配置MySQL，给予用户权限

### 构建
    git clone git@github.com:csfenghan/natip.git && cd natip
    make server
    cd server
    ./Server


## 客户端安装

### 安装依赖

    sudo apt install libjsoncpp

### 构建

    git clone git@github.com:csfenghan/natip.git && cd natip
    make client
    cd client
    ./Client remote_server.json

然后会出现要求的配置信息，以及可以连接的客户机的选项

