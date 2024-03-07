#ifndef FILEVIEWER_H
#define FILEVIEWER_H

#include "QtCore/qdir.h"
#include <QWidget>
#include <QTableWidget>

class FileViewer : public QWidget
{
    Q_OBJECT
public:
    FileViewer(QWidget *parent = nullptr);

private:
    void find();
    void showFiles(const QStringList &paths);
    void createFilesTable();

    QTableWidget *filesTable;
    QDir currentDir;
};

#endif // FILEVIEWER_H
