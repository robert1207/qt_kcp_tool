/*
 * @Name       kcp_tool
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-02
 */

#include "kcpdialog.h"
#include "ui_dialog.h"

KcpDialog::KcpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{

    kcp_type_list_.append("UDP");
    kcp_type_list_.append("KCP Default");
    kcp_type_list_.append("KCP Normal");
    kcp_type_list_.append("KCP Fast");

    server_client_type_list_.append("UDPServer");
    server_client_type_list_.append("UDP Client");



    ui->setupUi(this);
    InitUI();

    netmanager_p = NetManager::GetInstance();
    connect(netmanager_p, &NetManager::onRecv, this, &KcpDialog::OnRecv);
    connect(netmanager_p, &NetManager::onError, this, &KcpDialog::OnError);
}

KcpDialog::~KcpDialog()
{
    delete ui;
}

void KcpDialog::InitUI() {

    for (int a = 0; a < kcp_type_list_.size(); ++a) {
        ui->comboBoxKcpMode->addItem(kcp_type_list_[a]);
    }

    for(int a = 0; a < server_client_type_list_.size(); ++a) {
         ui->comboBoxNetType->addItem(server_client_type_list_[a]);
    }

    ui->comboBoxKcpMode->setCurrentIndex(0);
    ui->lineEditDesIp->setText("127.0.0.1");
    ui->lineEditDesPort->setText("2009");

    recv_str_cache_ = "";

    ui->lineEditKCPconv->setText("11223344");
}

void KcpDialog::SetNetTypeStatus(bool is_enable) {
     ui->comboBoxKcpMode->setEnabled(is_enable);
     ui->comboBoxNetType->setEnabled(is_enable);
     ui->lineEditDesIp->setEnabled(is_enable);
     ui->lineEditDesPort->setEnabled(is_enable);
}

void KcpDialog::SetOpenBtnStatus(bool is_open) {
    if(is_open) {
         ui->pushButtonOpen->setText(QApplication::translate("Dialog", "Open"));
    } else {
         ui->pushButtonOpen->setText(QApplication::translate("Dialog", "Close"));
    }
}

void KcpDialog::on_pushButtonOpen_clicked()
{
    if(netmanager_p->IsOpen()) { //do close
        netmanager_p->Close();
        SetOpenBtnStatus(true);
        SetNetTypeStatus(true);
    } else { //do open

        QString des_ip = ui->lineEditDesIp->text();
        QString des_port = ui->lineEditDesPort->text();

        if(des_ip.isEmpty()) {
            Toast::showTip(QApplication::translate("Dialog", "Please set destination IP !"));
            return;
        }

        if(des_port.isEmpty()) {
            Toast::showTip(QApplication::translate("Dialog", "Please set destination port !"));
            return;
        }

        //kcp mode
        int kcp_mode_index = ui->comboBoxKcpMode->currentIndex();
        netmanager_p->SetKcpMode(kcp_mode_index);

        if(kcp_mode_index != 0) {
            QString kcp_conv_str = ui->lineEditKCPconv->text();
            if(kcp_conv_str.isEmpty()) {
                Toast::showTip(QApplication::translate("Dialog", "Please set kcp conv in hex !"));
                return;
            }

            QByteArray temp;
            StringUtil::StringToHex(kcp_conv_str, temp);
            unsigned int kcp_conv_uint = static_cast<unsigned int>(temp.toInt());

            netmanager_p->SetKcpConv(kcp_conv_uint);
        }

        //net type
        int net_type_index = ui->comboBoxNetType->currentIndex();
        netmanager_p->SetNetType(net_type_index);

        //ip, port
        des_ip_ = des_ip;
        des_port_ = static_cast<quint16>(des_port.toInt());
        netmanager_p->Open(des_ip_, des_port_);

        SetOpenBtnStatus(false);
        SetNetTypeStatus(false);
    }
}

void KcpDialog::on_pushButtonSend_clicked()
{
    if(!netmanager_p->IsOpen()) {
        Toast::showTip(QApplication::translate("Dialog", "Please click 'open' button first !"));
        return;
    }

    QString send_str = ui->textEditSend->toPlainText();

    if(send_str.isEmpty()) {
        Toast::showTip(QApplication::translate("Dialog", "Please input sending string !"));
        return;
    }

    QByteArray send_ba;
    send_ba = send_str.toUtf8();

    if(ui->checkBoxSendHex->isChecked()) {
        send_ba = send_ba.toHex();
    }

    netmanager_p->Send(send_ba.data(), send_ba.size());
}

void KcpDialog::OnRecv(QByteArray *data) {

        QString str;
        if(ui->checkBoxRecvHex->isChecked()) {
             QByteArray ba;
             QString temp(*data);
             StringUtil::StringToHex(temp, ba);
             str += ba;
        } else {
             str += *data;
        }
        str += "\n";

        AddRecvMsg(str);
}

void KcpDialog::AddRecvMsg(QString line) {
    recv_str_cache_ += line;
    ui->textEditRecv->setText(recv_str_cache_);

    QScrollBar *scrollbar = ui->textEditRecv->verticalScrollBar();
    if (scrollbar) {
        scrollbar->setSliderPosition(scrollbar->maximum());
    }
}

void KcpDialog::ClearRecvMsg() {
    recv_str_cache_ = "";
    ui->textEditRecv->setText(recv_str_cache_);
}

void KcpDialog::ClearSendMsg() {
    ui->textEditSend->setText("");
}

void KcpDialog::on_pushButtonRecvClear_clicked()
{
    ClearRecvMsg();
}

void KcpDialog::on_pushButtonSendClear_clicked()
{
    ClearSendMsg();
}

void KcpDialog::OnError(int error_code, qint64 extra_error_code) {
     QString error_str;
     error_str += "e:"+ QString::number(error_code);
     error_str += "ex:"+ QString::number(extra_error_code);

     Toast::showTip(error_str);
}

void KcpDialog::on_comboBoxNetType_currentIndexChanged(int index)
{
    if(index == 0) { //server
        ui->lineEditDesIp->hide();
        ui->destinationLabel->hide();
    } else { //client
        ui->lineEditDesIp->show();
        ui->destinationLabel->show();
    }
}

void KcpDialog::on_comboBoxKcpMode_currentIndexChanged(int index)
{
    if(index == 0) { //udp
        ui->lineEditKCPconv->hide();
        ui->labelkcpconv->hide();
    } else {
        ui->lineEditKCPconv->show();
        ui->labelkcpconv->show();
    }
}
