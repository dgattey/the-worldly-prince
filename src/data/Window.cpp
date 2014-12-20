#include "Window.h"
#include "ui_Window.h"
#include <QGridLayout>

/**
 * @brief Loads a main window with correct arguments
 * Makes sure that the GLRenderWidget it loads has a
 * format argument so it uses at least OpenGL 4.1
 * @param parent The parent of the GLRenderWidget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow) {

    // Create a format argument
    QGLFormat qglFormat;
    qglFormat.setVersion(4,1);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setDoubleBuffer(true);
    qglFormat.setSwapInterval(1);

    // Setup the UI with a render widget in the middle
    m_ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralWidget);
    m_glWidget = new GLRenderWidget(qglFormat, this);
    gridLayout->addWidget(m_glWidget, 0, 1);
    gridLayout->setContentsMargins(0,0,0,0);

    // Title of the window
    setWindowTitle("The Little Prince");
}

/**
 * @brief Simply deletes the ui object
 */
MainWindow::~MainWindow() {
    delete m_ui;
}
