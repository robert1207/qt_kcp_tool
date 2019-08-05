/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */

#ifndef UDPCOMM_H
#define UDPCOMM_H

#include "comm.h"

#include <QUdpSocket>
#include <QString>
#include <QByteArray>


namespace HYKT {

class UdpComm :  public Comm
{
    Q_OBJECT
public:
    UdpComm();
    ~UdpComm() override;

    virtual void SetKcpConv(unsigned int conv) override;
    virtual int Open(QString des_ip, quint16 des_port) override;
    virtual void Close() override;
    virtual int Send(char *data, int len) override;
    virtual bool IsOpen() override;
    virtual void SetKcpMode(int mode) override;
    virtual void SetNetType(int type) override;

private:
    virtual void DoRecv() override;

private:
    QUdpSocket *upd_p;
    int net_type_;

    QString des_ip_;
    quint16 des_port_;

    QString connected_client_ip_;
    quint16 connected_client_port_;
};

} //namespace HYKT



#endif // UDPCOMM_H
