#include "mainwindow.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_dragging(false)
{
    // 窗口属性设置
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // 全透明样式表
    setStyleSheet(
        "QMainWindow { background: transparent; }"
        "QLabel {"
        "   color: #00ff00;"  // 使用亮绿色保证可见性
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   background: transparent;"
        "}"
        "QComboBox {"
        "   min-width: 120px;"
        "   font-size: 14px;"
        "   color: #00ff00;"
        "   background: rgba(50, 50, 50, 0.3);"  // 半透明深色背景
        "   border: 1px solid rgba(100, 255, 100, 0.4);"  // 浅绿色边框
        "   border-radius: 3px;"
        "   padding: 2px 20px 2px 5px;"
        "}"
        "QComboBox:hover {"
        "   background: rgba(50, 50, 50, 0.5);"
        "}"
        "QComboBox QAbstractItemView {"
        "   background: rgba(30, 30, 30, 0.9);"  // 深色半透明下拉框
        "   border: 1px solid rgba(100, 255, 100, 0.3);"
        "   color: #00ff00;"
        "   outline: none;"
        "   margin: 2px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "   padding: 3px 8px;"
        "   background: transparent;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "   background-color: rgba(100, 255, 100, 0.2);"
        "}"
    );

    // 中央部件设置
    QWidget *centralWidget = new QWidget();
    centralWidget->setAttribute(Qt::WA_TranslucentBackground);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(12, 12, 12, 12);  // 增加边距防止裁剪
    
    // 初始化UI组件
    cpuTempLabel = new QLabel("CPU温度: --°C");
    gpuTempLabel = new QLabel("GPU温度: --°C");
    fanSpeedLabel = new QLabel("风扇转速: -- RPM");
    
    fanLevelCombo = new QComboBox();
    fanLevelCombo->addItems({"auto", "0", "1", "2", "3", "4", "5", "6", "7", "full-speed"});
    fanLevelCombo->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);  // 保证下拉框弹出层样式
    fanLevelCombo->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    connect(fanLevelCombo, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::setFanLevel);

    layout->addWidget(cpuTempLabel);
    layout->addWidget(gpuTempLabel);
    layout->addWidget(fanSpeedLabel);
    layout->addWidget(fanLevelCombo);

    // 定时器设置
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateData);
    updateTimer->start(2000);
    updateData();
}

void MainWindow::updateData()
{
    readSensorData();

    // 动态颜色设置
    QString cpuColor = (cpuTemp > 100) ? "#ff0000" : "#00ff00";
    QString gpuColor = (gpuTemp > 100) ? "#ff0000" : "#00ff00";

    cpuTempLabel->setStyleSheet(QString("color: %1;").arg(cpuColor));
    gpuTempLabel->setStyleSheet(QString("color: %1;").arg(gpuColor));

    cpuTempLabel->setText(QString("CPU温度: %1°C").arg(cpuTemp, 0, 'f', 1));
    gpuTempLabel->setText(QString("GPU温度: %1°C").arg(gpuTemp, 0, 'f', 1));
    fanSpeedLabel->setText(QString("风扇转速: %1 RPM").arg(fanSpeed));
}

void MainWindow::readSensorData()
{
    QProcess process;
    process.start("sensors", QStringList() << "-j");
    process.waitForFinished(500);

    if (process.exitCode() == 0) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(process.readAllStandardOutput(), &error);
        
        if (error.error == QJsonParseError::NoError) {
            QJsonObject root = doc.object();

            // 解析传感器数据
            if (root.contains("thinkpad-isa-0000")) {
                QJsonObject thinkpad = root["thinkpad-isa-0000"].toObject();
                fanSpeed = thinkpad["fan1"].toObject()["fan1_input"].toInt();
                cpuTemp = thinkpad["CPU"].toObject()["temp1_input"].toDouble();
                gpuTemp = thinkpad["GPU"].toObject()["temp2_input"].toDouble();
            }
        }
    }
}

void MainWindow::setFanLevel(int index)
{
    QString level = fanLevelCombo->itemText(index);
    writeFanLevel(level);
}

void MainWindow::writeFanLevel(const QString& level)
{
    QProcess process;
    process.start("pkexec", QStringList() << "bash" << "-c" 
                 << QString("echo 'level %1' > /proc/acpi/ibm/fan").arg(level));
    process.waitForFinished(1000);
}

// 鼠标事件处理
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}