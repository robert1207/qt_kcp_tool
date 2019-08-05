/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#include "kcp.h"
#include "my_log.h"

namespace HYKT {

Kcp * Kcp::instance_ = nullptr;

Kcp::Kcp()
{
    is_open_ = false;
    timer_p = new QTimer(this);
    connect(timer_p, &QTimer::timeout, this, &Kcp::CallUpdateClock);
}

Kcp::~Kcp()
{
    delete timer_p;
}

Kcp*  Kcp::GetInstance() {
    if(instance_ == nullptr) {
        instance_ = new Kcp();
    }
    return instance_;
}

void Kcp::SetKcpConv(unsigned int conv) {
    conv_ = conv;
}

void  Kcp::SetMode(Mode mode) {
    mode_ = mode;
}

void  Kcp::SetMode(int mode) {
    mode_ = mode;
}

void Kcp::WriteKcpLog(const char *log, struct IKCPCB *kcp, void *user) {
    Q_UNUSED(kcp);
    Q_UNUSED(user);
    I << "kcp: " << log;
}

void Kcp::Open() {

    kcp_p = ikcp_create(conv_, static_cast<void*>(0));
    kcp_p->output = DoOutput;
    /*
    kcp_p->writelog = WriteKcpLog;
    kcp_p->logmask = IKCP_LOG_OUTPUT
            | IKCP_LOG_INPUT
            | IKCP_LOG_SEND
            | IKCP_LOG_RECV
            | IKCP_LOG_IN_DATA
            | IKCP_LOG_IN_ACK
            | IKCP_LOG_IN_PROBE
            | IKCP_LOG_IN_WINS
            | IKCP_LOG_OUT_DATA
            | IKCP_LOG_OUT_ACK
            | IKCP_LOG_OUT_PROBE
            | IKCP_LOG_OUT_WINS;
            */
    ikcp_wndsize(kcp_p, 32, 32);
    //ikcp_setmtu(kcp_p, 256);

    if(mode_ == Mode::Default) {
        ikcp_nodelay(kcp_p, 0, 10, 0, 0); //default mode
    } else if(mode_ == Mode::Normal) {
        ikcp_nodelay(kcp_p, 0, 10, 0, 1); //normal mode, close flow-control
    } else {
        //Fast mode
        // 第二个参数 nodelay-启用以后若干常规加速将启动
        // 第三个参数 interval为内部处理时钟，默认设置为 10ms
        // 第四个参数 resend为快速重传指标，设置为2
        // 第五个参数 为是否禁用常规流控，这里禁止
        ikcp_nodelay(kcp_p, 1, 10, 2, 1);
        kcp_p->rx_minrto = 10;
        kcp_p->fastresend = 1;
    }

    is_open_ = true;
    timer_p->start(10);
}

void Kcp::Close() {
    is_open_ = false;
    timer_p->stop();
    ikcp_release(kcp_p);
}

bool Kcp::IsOpen() {
    return is_open_;
}

void Kcp::KcpInput(const char *buf, int len) {
    ikcp_input(kcp_p, buf, len);
}

void Kcp::KcpSend(const  char *buf, int len) {
    ikcp_send(kcp_p, buf, len);
}

int Kcp::DoOutput(const char *buf, int len, ikcpcb *kcp, void *user) {
    return GetInstance()->DoOutput2(buf, len, kcp, user);
}

int Kcp::DoOutput2(const char *buf, int len, ikcpcb *kcp, void *user) {
    Q_UNUSED(kcp);
    Q_UNUSED(user);
    emit onKcpOutput(buf, len);
    return 0;
}

void Kcp::CallUpdateClock() {
    ikcp_update(kcp_p, iclock());
    int peek_size = ikcp_peeksize(kcp_p);
    if(peek_size > 0) {
        if(peek_size > buffer_.size()) {
            buffer_.resize(peek_size+10);
        }

        int read_size = ikcp_recv(kcp_p, buffer_.data(), buffer_.size());
     //   D << "read_size =" << read_size;

        if (read_size > 0) {
            emit onKcpRecv(buffer_, read_size);
        }
    }
}



} //namespace HYKT
