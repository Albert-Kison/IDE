#include "mainwindow.h"
#include "codeeditor.h"
#include "./ui_mainwindow.h"
#include "fileviewer.h"
#include <QVBoxLayout>
#include <QDebug>
#include <iostream>

std::ostream&  operator <<(std::ostream &stream,const QString &str)
{
    stream << str.toLatin1().constData(); //or: stream << str.toStdString(); //??
    return stream;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::cout << "path in main" << std::endl;

    // Create a container widget to hold the CodeEditor
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    tabWidget = new QTabWidget;
    editor = new CodeEditor();

    tabWidget->addTab(editor, tr("Editor"));

    FileViewer *fileViewer = new FileViewer(centralWidget);

    // Add the CodeEditor widget to the layout
    layout->addWidget(tabWidget);
    layout->addWidget(fileViewer);

    // Set the central widget of MainWindow to the container widget
    setCentralWidget(centralWidget);

    connect(fileViewer, &FileViewer::fileSelected, editor, &CodeEditor::onFileSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_textEdit_textChanged()
{

}

