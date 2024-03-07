#include "fileviewer.h"
#include "QtCore/qdiriterator.h"
#include "QtWidgets/qgridlayout.h"
#include "QHeaderView"
#include <QDebug>
#include <iostream>


// std::ostream&  operator <<(std::ostream &stream,const QString &str)
// {
//     stream << str.toLatin1().constData(); //or: stream << str.toStdString(); //??
//     return stream;
// }

FileViewer::FileViewer(QWidget *parent)
    : QWidget(parent) {

    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(filesTable);
}


void FileViewer::find() {
    filesTable->setRowCount(0);

    // QString path = currentDir.absolutePath();
    QString path = "/Users/albertkison/eclipse-workspace/HelloWorld/src";
    // std::cout << path << std::endl;
    currentDir = QDir(path);

    QStringList filter;
    QDirIterator it(path, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
        files << it.next();
    files.sort();
    showFiles(files);
}

enum { absoluteFileNameRole = Qt::UserRole + 1 };

void FileViewer::showFiles(const QStringList &paths) {
    for (const QString &filePath : paths) {

        const QString toolTip = QDir::toNativeSeparators(filePath);
        const QString relativePath = QDir::toNativeSeparators(currentDir.relativeFilePath(filePath));
        const qint64 size = QFileInfo(filePath).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(relativePath);
        fileNameItem->setData(absoluteFileNameRole, QVariant(filePath));
        fileNameItem->setToolTip(toolTip);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *sizeItem = new QTableWidgetItem(QLocale().formattedDataSize(size));
        sizeItem->setData(absoluteFileNameRole, QVariant(filePath));
        sizeItem->setToolTip(toolTip);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
}


void FileViewer::createFilesTable() {
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
    filesTable->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(filesTable, &QTableWidget::itemActivated, this, &FileViewer::openFileOfItem);

    find();
}

void FileViewer::openFileOfItem(QTableWidgetItem *item)
{
    if (item == nullptr) {
        return; // No item activated
    }

    // Get the absolute file path from the item's data
    QString absoluteFilePath = item->data(absoluteFileNameRole).toString();

    emit fileSelected(absoluteFilePath); // Emit the signal with the file path
}
