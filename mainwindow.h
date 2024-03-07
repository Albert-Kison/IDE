#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "codeeditor.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_textEdit_textChanged();
    // void onFileSelected(const QString &filePath);

private:
    Ui::MainWindow *ui;
    CodeEditor *editor;
};
#endif // MAINWINDOW_H
