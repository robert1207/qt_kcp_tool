/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#ifndef NETMANAGER_H
#define NETMANAGER_H

#include "../singletone/singleton.h"
#include "comm.h"
#include "udpcomm.h"
#include "kcpcomm.h"

#include <QObject>
#include <QString>
#include <QByteArray>

namespace HYKT {
class NetManager : public QObject
{
    Q_OBJECT
public:
    NetManager();
    ~NetManager();


    static inline NetManager* GetInstance() {
         return &Singleton<HYKT::NetManager>::instance();
    }

    void SetKcpConv(unsigned int conv);

    void SetKcpMode(Comm::KcpMode mode);
    void SetKcpMode(int mode);

    void SetNetType(Comm::NetType type);
    void SetNetType(int type);

    int Open(QString des_ip, quint16 des_port);
    void Close();
    int Send(char *data, int len);
    bool IsOpen();

signals:
    void onRecv(QByteArray *data);
    void onError(int error_code, qint64 extra_error_code);
private:
    void ConnectSignals();
    void DisconnectSignals();

private:
    unsigned int kcp_conv_;
    int kcp_mode_;
    int net_type_;
    Comm *comm_p;
};
} //namespace HYKT

#endif // NETMANAGER_H
