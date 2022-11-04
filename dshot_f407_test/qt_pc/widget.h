#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class Widget;
}

class KBusTestWorker;
class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void sendMotors();

    void getTelemetry();
private slots:
    void on_pB_send_clicked();

    void on_hS_M1_valueChanged(int value);

    void on_hS_M2_valueChanged(int value);

    void on_hS_M3_valueChanged(int value);

    void on_hS_M4_valueChanged(int value);

    void on_sB_M1_valueChanged(double arg1);

    void on_sB_M2_valueChanged(double arg1);

    void on_sB_M3_valueChanged(double arg1);

    void on_sB_M4_valueChanged(double arg1);

    void on_pB_parking_clicked();

    void on_cB_fix_motors_clicked(bool checked);

private:
    Ui::Widget *ui;
    KBusTestWorker * worker;
    QTimer *timer;
};

#endif // WIDGET_H
