/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */


#ifndef DIALOG_H
#define DIALOG_H

#include "../net/netmanager.h"
#include "../views/toast/toast.h"
#include "../utils/stringutil.h"

#include <QString>
#include <QDialog>
#include <QScrollBar>

using namespace HYKT;

namespace Ui {
class Dialog;
}

class KcpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KcpDialog(QWidget *parent = nullptr);
    ~KcpDialog();

private slots:
    void on_pushButtonOpen_clicked();
    void on_pushButtonSend_clicked();

    void on_pushButtonRecvClear_clicked();

    void on_pushButtonSendClear_clicked();

    void on_comboBoxNetType_currentIndexChanged(int index);

private:
    void InitUI();
    void SetOpenBtnStatus(bool is_open);
    void SetNetTypeStatus(bool is_enable);
    void OnRecv(QByteArray *data);
    void AddRecvMsg(QString line);
    void ClearRecvMsg();
    void ClearSendMsg();
    void OnError(int error_code, qint64 extra_error_code);
    
private:
    Ui::Dialog *ui;
    NetManager *netmanager_p;
    QString des_ip_;
    quint16 des_port_;
    QString recv_str_cache_;

    QList<QString> kcp_type_list_;
    QList<QString> server_client_type_list_;


};

#endif // DIALOG_H
