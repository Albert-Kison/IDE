#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QtWidgets/qboxlayout.h"
#include "codeeditor.h"
#include "projectwindow.h"
#include "Project.h"
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
    void createButtonPressed();

private:    

    void displayProjects();
    void saveProjects();
    void initUI();
    void deleteLayout(QLayout *layout);
    void createProjectsLayout(QVBoxLayout *projectsVerticalLayout, QList<Project> *projects);
    void createJsonFile(const QString& filePath, const Project &project);
    void createTextFile(const QString& filePath, const Project &project);
    void createFolder(const QString& folderPath);
    void openProjectButtonPressed(const Project& project);
    void deleteProjectButtonPressed(const Project& project);


    QList<Project> *projects;
    QVBoxLayout *projectsVerticalLayout;

    Ui::MainWindow *ui;
    QVBoxLayout *layout;
    CodeEditor *editor;
    QTabWidget *tabWidget;
    ProjectWindow *projectWindow;

signals:
    void generateDiagramClicked(QString &code);
    void openProject(const Project& project);
};
#endif // MAINWINDOW_H
