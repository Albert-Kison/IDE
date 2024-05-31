#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QDialog>
#include "Project.h"
#include "codeeditor.h"
#include "diagram.h"

namespace Ui {
class ProjectWindow;
}

class ProjectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectWindow(QWidget *parent = nullptr);
    ~ProjectWindow();

    CodeEditor *editor;

signals:
    void generateDiagramClicked(QString &code);

private:
    Ui::ProjectWindow *ui;    
    Diagram *diagram;
    Project project;

public slots:
    void onOpenProject(const Project &project);
    void saveProject();
    void exportDiagramToPdf();
};

#endif // PROJECTWINDOW_H
