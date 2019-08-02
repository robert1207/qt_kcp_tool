/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#include "kcpcomm.h"
#include "my_log.h"

namespace HYKT {

KcpComm::KcpComm()
{
    kcp_p = Kcp::GetInstance();
}

KcpComm::~KcpComm()
{
}

void KcpComm::SetKcpMode(int mode) {
    kcp_p->SetMode(mode);
}

void KcpComm::SetNetType(int type) {
    net_type_ = type;
}

int KcpComm::Open(QString des_ip, quint16 des_port) {
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
    connect(upd_p, &QUdpSocket::readyRead, this, &KcpComm::DoRecv);

    //kcp
    connect(kcp_p, &Kcp::onKcpOutput, this, &KcpComm::DoKcpOutput);
    connect(kcp_p, &Kcp::onKcpRecv, this, &KcpComm::DoKcpRecv);
    kcp_p->Open();

    return NoError;
}

void KcpComm::Close() {
    disconnect(upd_p, &QUdpSocket::readyRead, this, &KcpComm::DoRecv);
    upd_p->close();

    //kcp
    disconnect(kcp_p, &Kcp::onKcpOutput, this, &KcpComm::DoKcpOutput);
    disconnect(kcp_p, &Kcp::onKcpRecv, this, &KcpComm::DoKcpRecv);
    kcp_p->Close();
}

int KcpComm::Send(char *data, int len) {
    kcp_p->KcpSend(data, len);
    return NoError;
}

bool KcpComm::IsOpen() {
    return upd_p->isOpen();
}

void KcpComm::DoRecv() {
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
        //call kcp input
   //     D << "kcp input len=" << res;
        kcp_p->KcpInput(datagram->data(), datagram->size());
    }

    delete datagram;
}


void KcpComm::DoKcpOutput(const char *data, int len) {
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
    }
}

void KcpComm::DoKcpRecv(const char *buf, int len) {

  //  D << "DoKcpRecv read len = " << len;
    //todo: make is better
    QByteArray *datagram;
    datagram = new QByteArray();
    datagram->resize(len);
    memcpy(datagram->data(), buf, static_cast<size_t>(len));

    emit onRecv(datagram);
    delete datagram;
}



} //namespace HYKT

