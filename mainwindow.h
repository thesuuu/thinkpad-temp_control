// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QAbstractItemView>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    // 添加鼠标事件声明
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateData();
    void setFanLevel(int level);

private:
    void readSensorData();
    void writeFanLevel(const QString& level);

    QLabel *cpuTempLabel;
    QLabel *gpuTempLabel;
    QLabel *fanSpeedLabel;
    QComboBox *fanLevelCombo;
    QTimer *updateTimer;

    // 添加窗口拖动相关成员变量
    bool m_dragging;
    QPoint m_dragPosition;

    double cpuTemp = 0.0;
    double gpuTemp = 0.0;
    int fanSpeed = 0;
};

#endif // MAINWINDOW_H