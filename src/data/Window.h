#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GLCommon.h"
#include <QMainWindow>
#include <QGLFormat>
#include "GLRenderWidget.h"

namespace Ui {
    class MainWindow;
}

/**
 * @brief Holds a GLRenderWidget and can launch a window
 * A class to encapsulate holding a window and loading a
 * GLRenderWidget with the correct parameters.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    GLRenderWidget* m_glWidget;
};

#endif // MAINWINDOW_H
