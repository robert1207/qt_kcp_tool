/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#ifndef COMM_H
#define COMM_H

#include <QObject>
#include <QString>
#include <QByteArray>


namespace HYKT {
class Comm : public QObject
{
    Q_OBJECT

public:
    Comm();
    virtual ~Comm();

    enum ErrorCode {
        NoError = 0,
        ErrorOpen,
        ErrorRecv,
        ErrorSend
    };

    enum KcpMode {
        UDP,
        KCP_Default,
        KCP_Normal,
        KCP_Fast
    };

    enum NetType {
        Server,
        Client
    };

    virtual void SetKcpMode(int mode) = 0;
    virtual void SetNetType(int type) = 0;
    virtual int Open(QString des_ip, quint16 des_port) = 0;
    virtual void Close() = 0;
    virtual int Send(char *data, int len) = 0;
    virtual bool IsOpen() = 0;
private:
    virtual void DoRecv() = 0;

signals:
    void onRecv(QByteArray *data);
    void onError(int error_code, qint64 extra_error_code);

};

} //namespace HYKT


#endif // COMM_H
