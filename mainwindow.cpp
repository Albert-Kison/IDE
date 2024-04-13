#include "mainwindow.h"
#include "codeeditor.h"
#include "./ui_mainwindow.h"
#include "diagram.h"
#include "fileviewer.h"
#include "sqlcodegenerator.h";
#include <QVBoxLayout>
#include <QDebug>
#include <iostream>

// std::ostream&  operator <<(std::ostream &stream,const QString &str)
// {
//     stream << str.toLatin1().constData(); //or: stream << str.toStdString(); //??
//     return stream;
// }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::cout << "path in main" << std::endl;

    SQLCodeGenerator *sqlCodeGenerator = new SQLCodeGenerator();

    // Create a container widget to hold the CodeEditor
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    tabWidget = new QTabWidget;
    editor = new CodeEditor();

    // QWidget *parentWidget = new QWidget(this);
    Diagram *diagramWidget = new Diagram();

    tabWidget->addTab(editor, tr("Editor"));
    tabWidget->addTab(diagramWidget, tr("Diagram"));

    FileViewer *fileViewer = new FileViewer(centralWidget);


    // Add the CodeEditor widget to the layout
    layout->addWidget(tabWidget);
    // layout->addWidget(fileViewer);
    // layout->addWidget(diagramWidget);

    centralWidget->setLayout(layout);

    // Set the central widget of MainWindow to the container widget
    setCentralWidget(centralWidget);

    connect(fileViewer, &FileViewer::fileSelected, editor, &CodeEditor::onFileSelected);
    connect(diagramWidget, &Diagram::generateSqlClicked, sqlCodeGenerator, &SQLCodeGenerator::onGenerateSqlCodeClicked);
    connect(sqlCodeGenerator, &SQLCodeGenerator::codeGenerated, editor, &CodeEditor::onCodeGenerated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_textEdit_textChanged()
{

}

