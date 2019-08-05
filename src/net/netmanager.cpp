/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#include "netmanager.h"

namespace HYKT {


NetManager::NetManager()
{
    kcp_mode_ = Comm::KcpMode::UDP;
    net_type_ = Comm::NetType::Server;
    comm_p = nullptr;
}

NetManager::~NetManager()
{
    if (comm_p != nullptr) Close();
}

void NetManager::SetKcpConv(unsigned int conv) {
    kcp_conv_ = conv;
}

void NetManager::SetKcpMode(Comm::KcpMode mode) {
    kcp_mode_ = mode;
}

void NetManager::SetKcpMode(int mode) {
    kcp_mode_ = mode;
}

void NetManager::SetNetType(Comm::NetType type) {
    net_type_ = type;
}

void NetManager::SetNetType(int type) {
    net_type_ = type;
}

int NetManager::Open(QString des_ip, quint16 des_port) {
    if(kcp_mode_ == Comm::KcpMode::UDP) {
        comm_p = new UdpComm();
        comm_p->SetNetType(net_type_);
        ConnectSignals();
        return comm_p->Open(des_ip, des_port);
    } else {
        comm_p = new KcpComm();
        comm_p->SetKcpConv(kcp_conv_);
        comm_p->SetKcpMode(kcp_mode_);
        comm_p->SetNetType(net_type_);
        ConnectSignals();
        return comm_p->Open(des_ip, des_port);
    }
}

void NetManager::Close() {
    DisconnectSignals();
    comm_p->Close();
    delete comm_p;
    comm_p = nullptr;
}

int NetManager::Send(char *data, int len) {
    return comm_p->Send(data, len);
}

bool NetManager::IsOpen() {
    if(comm_p == nullptr) {
        return false;
    }
    return comm_p->IsOpen();
}

void NetManager::ConnectSignals() {
    connect(comm_p, &Comm::onRecv, this, &NetManager::onRecv);
    connect(comm_p, &Comm::onError, this, &NetManager::onError);
}

void NetManager::DisconnectSignals() {
    disconnect(comm_p, &Comm::onRecv, this, &NetManager::onRecv);
    disconnect(comm_p, &Comm::onError, this, &NetManager::onError);
}


} //namespace HYKT
