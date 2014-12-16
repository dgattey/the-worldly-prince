#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CS123Common.h"
#include <QMainWindow>
#include <QGLFormat>
#include "glrunner.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    GLWidget* m_glWidget;
};

#endif // MAINWINDOW_H
