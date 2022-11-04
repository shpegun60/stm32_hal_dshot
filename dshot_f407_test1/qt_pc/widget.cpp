#include "widget.h"
#include "ui_widget.h"
#include "kbusserialport.h"
#include "kbustestworker.h"

#include <QInputDialog>

#define USER_DATA_COPY(n, from, to, pos, direction) do {\
        if(direction) {\
            while((n)--) {\
                *(to)++ = *((from) + (n));\
                (*(pos))++;\
            }\
        } else {\
            while((n)--) {\
                *(to)++ = *(from)++;\
                (*(pos))++;\
            }\
        }\
} while(0UL)

float readFloat(uint8_t *data, uint16_t *pos, bool bn) {
    uint8_t n = 4;
    float value = 0.0;
    uint8_t * ptr_from = (uint8_t *)(data + *pos);
    uint8_t * ptr_to = (uint8_t *)(&value);
    USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
    return value;
}


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    worker = new KBusTestWorker(this);
    auto port = new KBusSerialPort(this);
    port->port = new QSerialPort(this);
    auto txt = QInputDialog::getText(nullptr, "com name", "", QLineEdit::Normal, "COM5");
    port->port->setPortName(txt);
    port->port->setParity(QSerialPort::NoParity);
    port->port->setStopBits(QSerialPort::OneStop);
    port->port->setFlowControl(QSerialPort::NoFlowControl);
    //port->port->setBaudRate(QSerialPort::Baud38400); //Baud115200 Baud38400
    port->port->setBaudRate((QSerialPort::Baud115200)); //Baud115200 Baud38400
    port->port->open(QIODevice::ReadWrite);
    worker->setupBus(port);


    connect(worker, &KBusTestWorker::packetReceived, this, [this](const QByteArray s) {
        //ui->pTE_log->appendPlainText(s.toHex('.'));

        float temperature[4];
        float voltage[4];
        float current[4];
        float consumption[4];
        float ERpmp[4];

        uint16_t Rpos = 0;
        //int i =0;
        for(int i = 0; i < 4; ++i) {
            temperature[i] = readFloat((uint8_t *)s.data(), &Rpos, false);
            voltage[i] = readFloat((uint8_t *)s.data(), &Rpos, false);
            current[i] = readFloat((uint8_t *)s.data(), &Rpos, false);
            consumption[i] = readFloat((uint8_t *)s.data(), &Rpos, false);
            ERpmp[i] = readFloat((uint8_t *)s.data(), &Rpos, false);
        }

        ui->dsb_M1_Temp->setValue(temperature[0]);
        ui->dsb_M2_Temp->setValue(temperature[1]);
        ui->dsb_M3_Temp->setValue(temperature[2]);
        ui->dsb_M4_Temp->setValue(temperature[3]);

        ui->dsb_M1_Voltage->setValue(voltage[0]);
        ui->dsb_M2_Voltage->setValue(voltage[1]);
        ui->dsb_M3_Voltage->setValue(voltage[2]);
        ui->dsb_M4_Voltage->setValue(voltage[3]);

        ui->dsb_M1_Current->setValue(current[0]);
        ui->dsb_M2_Current->setValue(current[1]);
        ui->dsb_M3_Current->setValue(current[2]);
        ui->dsb_M4_Current->setValue(current[3]);

        ui->dsb_M1_Consumption->setValue(consumption[0]);
        ui->dsb_M2_Consumption->setValue(consumption[1]);
        ui->dsb_M3_Consumption->setValue(consumption[2]);
        ui->dsb_M4_Consumption->setValue(consumption[3]);

        ui->dsb_M1_Rpm->setValue(ERpmp[0]);
        ui->dsb_M2_Rpm->setValue(ERpmp[1]);
        ui->dsb_M3_Rpm->setValue(ERpmp[2]);
        ui->dsb_M4_Rpm->setValue(ERpmp[3]);

    });

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        getTelemetry();
    });
    timer->start(300);
}

Widget::~Widget()
{
    for(int i = 0; i < 5; ++i) {
        on_pB_parking_clicked();
    }
    delete ui;
}

void Widget::on_pB_send_clicked()
{
    worker->send(ui->lE_cmd->text());
}
void Widget::getTelemetry()
{
    QByteArray ar;
    ar.resize(2);
    ar[0] = 0x02;
    ar[1] = 0x00;
    worker->sendPacket(ar);
}


void Widget::sendMotors()
{
    QByteArray ar;
    ar.resize(20);
    int pos = 0;
    ar[pos++] = 0x01;

    float M[4];
    if(ui->cB_fix_motors->checkState() == Qt::Checked) {
        M[0] = ui->sB_M1->value()/100.0;
        M[1] = M[0];
        M[2] = M[0];
        M[3] = M[0];
    } else {
        M[0] = ui->sB_M1->value()/100.0;
        M[1] = ui->sB_M2->value()/100.0;
        M[2] = ui->sB_M3->value()/100.0;
        M[3] = ui->sB_M4->value()/100.0;
    }

    for(int i = 0; i < 4; ++i) {
        quint8* ptr = (quint8*)&M[i];
        ar[pos++] = ptr[0];
        ar[pos++] = ptr[1];
        ar[pos++] = ptr[2];
        ar[pos++] = ptr[3];
    }
    worker->sendPacket(ar);
}

///////////////////M1////////////////////////////////////
void Widget::on_hS_M1_valueChanged(int value)
{
    ui->sB_M1->setValue(value);
    sendMotors();
}

void Widget::on_sB_M1_valueChanged(double arg1)
{
    ui->hS_M1->setValue(arg1);
    sendMotors();
}



///////////////////M2////////////////////////////////////
void Widget::on_hS_M2_valueChanged(int value)
{
    ui->sB_M2->setValue(value);
    sendMotors();
}
void Widget::on_sB_M2_valueChanged(double arg1)
{
    ui->hS_M2->setValue(arg1);
    sendMotors();
}




///////////////////M3////////////////////////////////////
void Widget::on_hS_M3_valueChanged(int value)
{
    ui->sB_M3->setValue(value);
    sendMotors();
}

void Widget::on_sB_M3_valueChanged(double arg1)
{
    ui->hS_M3->setValue(arg1);
    sendMotors();
}


///////////////////M4////////////////////////////////////
void Widget::on_hS_M4_valueChanged(int value)
{
    ui->sB_M4->setValue(value);
    sendMotors();
}

void Widget::on_sB_M4_valueChanged(double arg1)
{
    ui->hS_M4->setValue(arg1);
    sendMotors();
}


void Widget::on_pB_parking_clicked()
{
    ui->sB_M1->setValue(0);
    ui->hS_M1->setValue(0);

    ui->sB_M2->setValue(0);
    ui->hS_M2->setValue(0);

    ui->sB_M3->setValue(0);
    ui->hS_M3->setValue(0);

    ui->sB_M4->setValue(0);
    ui->hS_M4->setValue(0);

    sendMotors();
}


void Widget::on_cB_fix_motors_clicked(bool checked)
{
    (void)checked;
    sendMotors();
}

