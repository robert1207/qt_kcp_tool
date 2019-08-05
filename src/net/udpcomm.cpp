/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */

#include "udpcomm.h"


namespace HYKT {
UdpComm::UdpComm()
{
}

UdpComm::~UdpComm()
{
}

void UdpComm::SetKcpConv(unsigned int conv) {
    Q_UNUSED(conv);
    //unimpliment
}

void UdpComm::SetKcpMode(int mode) {
    Q_UNUSED(mode);
    //unimpliment
}

void UdpComm::SetNetType(int type) {
    net_type_ = type;
}

int UdpComm::Open(QString des_ip, quint16 des_port) {
    upd_p = new QUdpSocket(this);
    QHostAddress addr(des_ip);
    if(net_type_ == NetType::Client) {
        des_ip_ = des_ip;
        des_port_ = des_port;
        upd_p->bind();
        upd_p->open(QIODevice::OpenModeFlag::ReadWrite);
    } else {
        upd_p->bind(des_port);
        upd_p->open(QIODevice::OpenModeFlag::ReadWrite);
    }

    connect(upd_p, &QUdpSocket::readyRead, this, &UdpComm::DoRecv);
    return NoError;
}

void UdpComm::Close() {
    disconnect(upd_p, &QUdpSocket::readyRead, this, &UdpComm::DoRecv);
    upd_p->close();
}

bool UdpComm::IsOpen() {
    return upd_p->isOpen();
}

int UdpComm::Send(char *data, int len) {
    qint64 res;
    if(net_type_ == NetType::Client) {
        QHostAddress addr(des_ip_);
        res = upd_p->writeDatagram(data, len, addr, des_port_);
    } else {
        QHostAddress addr(connected_client_ip_);
        res = upd_p->writeDatagram(data, len, addr, connected_client_port_);
    }

    if(res < 0) {
        emit onError(ErrorSend, res);
        return  ErrorSend;
    }

    return NoError;
}

void UdpComm::DoRecv() {
    QByteArray *datagram;
    datagram = new QByteArray();
    datagram->resize(static_cast<int>(upd_p->pendingDatagramSize()));

    qint64 res;
    if(net_type_ == NetType::Client) {
        res = upd_p->readDatagram(datagram->data(),datagram->size());
    } else {
        QHostAddress host;
        quint16 port;

        res = upd_p->readDatagram(datagram->data(),datagram->size(), &host, &port);
        QHostAddress ipv4(host.toIPv4Address());
        connected_client_ip_ = ipv4.toString();
        connected_client_port_ = port;
    }
    if (res < 0) {
        emit onError(ErrorRecv, res);
    } else {
        emit onRecv(datagram);
    }

    delete datagram;
}



} //namespace HYKT

