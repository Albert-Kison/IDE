#include "projectwindow.h"
#include "QtWidgets/qboxlayout.h"
#include "QtWidgets/qpushbutton.h"
#include "QtWidgets/qtabwidget.h"
#include "codeeditor.h"
#include "diagram.h"
#include "sqlcodegenerator.h"
#include "ui_projectwindow.h"
#include "mainwindow.h"
#include <iostream>

ProjectWindow::ProjectWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProjectWindow)
{
    ui->setupUi(this);

    resize(1000, 800);

    std::cout << "path in project window" << std::endl;

    SQLCodeGenerator *sqlCodeGenerator = new SQLCodeGenerator();

    // Create a container widget to hold the CodeEditor
    // QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;

    QTabWidget *tabWidget = new QTabWidget;
    CodeEditor *editor = new CodeEditor();

    QWidget *editorWidget = new QWidget;
    QVBoxLayout *editorLayout = new QVBoxLayout;

    editorLayout->addWidget(editor);

    QPushButton *generateButton = new QPushButton("Generate");
    editorLayout->addWidget(generateButton);

    connect(generateButton, &QPushButton::clicked, [this, sqlCodeGenerator, editor]() {
        QString code = editor->toPlainText();
        sqlCodeGenerator->parseSql(code);
    });

    editorWidget->setLayout(editorLayout);

    // QWidget *parentWidget = new QWidget(this);
    Diagram *diagramWidget = new Diagram();

    tabWidget->addTab(editorWidget, tr("Editor"));
    tabWidget->addTab(diagramWidget, tr("Diagram"));

    // FileViewer *fileViewer = new FileViewer(centralWidget);


    // Add the CodeEditor widget to the layout
    layout->addWidget(tabWidget);
    // layout->addWidget(fileViewer);
    // layout->addWidget(diagramWidget);

    setLayout(layout);

    // centralWidget->setLayout(layout);

    // // Set the central widget of MainWindow to the container widget
    // setCentralWidget(centralWidget);

    // connect(fileViewer, &FileViewer::fileSelected, editor, &CodeEditor::onFileSelected);
    connect(qobject_cast<MainWindow*>(parent), &MainWindow::openProject, this, &ProjectWindow::onOpenProject);
    connect(diagramWidget, &Diagram::generateSqlClicked, sqlCodeGenerator, &SQLCodeGenerator::onGenerateSqlCodeClicked);
    connect(sqlCodeGenerator, &SQLCodeGenerator::codeGenerated, editor, &CodeEditor::onCodeGenerated);
    connect(sqlCodeGenerator, &SQLCodeGenerator::codeParsed, diagramWidget, &Diagram::drawDiagram);
}



ProjectWindow::~ProjectWindow()
{
    delete ui;
}



void ProjectWindow::onOpenProject(const Project &project) {
    std::cout << "Open project" << std::endl;
}
