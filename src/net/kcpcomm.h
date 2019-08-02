/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#ifndef KCPCOMM_H
#define KCPCOMM_H


#include "comm.h"
#include "kcp/kcp.h"

#include <QUdpSocket>
#include <QString>
#include <QByteArray>

namespace HYKT {
class KcpComm :  public Comm
{
    Q_OBJECT
public:
    KcpComm();
    ~KcpComm() override;

    virtual int Open(QString des_ip, quint16 des_port) override;
    virtual void Close() override;
    virtual int Send(char *data, int len) override;
    virtual bool IsOpen() override;
    virtual void SetKcpMode(int mode) override;
    virtual void SetNetType(int type) override;
private:
    virtual void DoRecv() override;

    void DoKcpOutput(const char *data, int len);
    void DoKcpRecv(const char *buf, int len);

private:
    QUdpSocket *upd_p;
    Kcp *kcp_p;
    int net_type_;

    QString des_ip_;
    quint16 des_port_;

    QString connected_client_ip_;
    quint16 connected_client_port_;
};
} //namespace HYKT


#endif // KCPCOMM_H
