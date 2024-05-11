#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QDialog>
#include "Project.h"

namespace Ui {
class ProjectWindow;
}

class ProjectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectWindow(QWidget *parent = nullptr);
    ~ProjectWindow();

signals:
    void generateDiagramClicked(QString &code);

private:
    Ui::ProjectWindow *ui;

private slots:
    void onOpenProject(const Project &project);
};

#endif // PROJECTWINDOW_H
