#include "Window.h"
#include "ui_Window.h"
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow) {
    QGLFormat qglFormat;
    qglFormat.setVersion(4,1);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setDoubleBuffer(true);
    qglFormat.setSwapInterval(1);

    m_ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralWidget);
    m_glWidget = new GLRenderWidget(qglFormat, this);
    gridLayout->addWidget(m_glWidget, 0, 1);
    gridLayout->setContentsMargins(0,0,0,0);
    setWindowTitle("The Little Prince");
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
