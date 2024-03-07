#include "mainwindow.h"
#include "codeeditor.h"
#include "./ui_mainwindow.h"
#include "fileviewer.h"
#include "highlighter.h"
#include <QVBoxLayout>
#include <QDebug>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::cout << "path in main" << std::endl;

    // Create a container widget to hold the CodeEditor
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Create the CodeEditor widget and Highlighter
    CodeEditor *editor = new CodeEditor(centralWidget);
    Highlighter *highlighter = new Highlighter(editor->document());

    FileViewer *fileViewer = new FileViewer(centralWidget);

    // Add the CodeEditor widget to the layout
    layout->addWidget(editor);
    layout->addWidget(fileViewer);

    // Set the central widget of MainWindow to the container widget
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_textEdit_textChanged()
{

}

