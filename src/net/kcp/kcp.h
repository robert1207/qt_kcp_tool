/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */

#ifndef KCP_H
#define KCP_H

//https://github.com/skywind3000/kcp
//https://wetest.qq.com/lab/view/391.html
//https://github.com/skywind3000/kcp/wiki

extern "C" {
#include "clock.h"
#include "ikcp.h"
}

#include <QObject>
#include <QTimer>

namespace HYKT {

class Kcp : public QObject
{
    Q_OBJECT
private:
       Kcp();
public:

    ~Kcp();

    enum Mode {
        Default,
        Normal,
        Fast
    };

    static Kcp* GetInstance();

    void SetMode(Mode mode);
    void SetMode(int mode);
    void Open();
    void Close();
    bool IsOpen();

    void KcpInput(const char *buf, int len); //kcp input
    void KcpSend(const  char *buf, int len); //kcp send


signals:
    void onKcpRecv(const char *buf, int len);  //kcp recv
    void onKcpOutput(const char *buf, int len); //kcp output

private:
    static int DoOutput(const char *buf, int len, ikcpcb *kcp, void *user);
    static void WriteKcpLog(const char *log, struct IKCPCB *kcp, void *user);

    int DoOutput2(const char *buf, int len, ikcpcb *kcp, void *user);
    void CallUpdateClock();


private:
    bool is_open_;
    int mode_;
    ikcpcb *kcp_p;
    static Kcp *instance_;
    QTimer *timer_p;

    QByteArray buffer_;
};

} //namespace HYKT

#endif // KCP_H
