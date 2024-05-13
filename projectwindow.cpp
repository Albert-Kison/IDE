#include "projectwindow.h"
#include "QtCore/qfile.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtWidgets/qboxlayout.h"
#include "QtWidgets/qmenubar.h"
#include "QtWidgets/qpushbutton.h"
#include "QtWidgets/qtabwidget.h"
#include "sqlcodegenerator.h"
#include "ui_projectwindow.h"
#include "mainwindow.h"
#include <iostream>
#include <QDir>
#include <QMessageBox>

ProjectWindow::ProjectWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProjectWindow)
    , project(Project("", ""))
{
    ui->setupUi(this);

    resize(1000, 800);

    std::cout << "path in project window" << std::endl;

    SQLCodeGenerator *sqlCodeGenerator = new SQLCodeGenerator();

    // Create a menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = menuBar->addMenu(tr("File"));
    QAction *saveAction = fileMenu->addAction(tr("Save"));
    saveAction->setShortcut(QKeySequence::Save);
    // Connect the save action to a slot
    connect(saveAction, &QAction::triggered, this, &ProjectWindow::saveProject);

    // Create a container widget to hold the CodeEditor
    // QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;

    QTabWidget *tabWidget = new QTabWidget;
    editor = new CodeEditor();

    QWidget *editorWidget = new QWidget;
    QVBoxLayout *editorLayout = new QVBoxLayout;

    editorLayout->addWidget(editor);

    QPushButton *generateButton = new QPushButton("Generate");
    editorLayout->addWidget(generateButton);

    connect(generateButton, &QPushButton::clicked, [this, sqlCodeGenerator]() {
        QString code = this->editor->toPlainText();
        sqlCodeGenerator->parseSql(code);
    });

    editorWidget->setLayout(editorLayout);

    // QWidget *parentWidget = new QWidget(this);
    diagram = new Diagram();

    tabWidget->addTab(editorWidget, tr("Editor"));
    tabWidget->addTab(diagram, tr("Diagram"));

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
    connect(diagram, &Diagram::generateSqlClicked, sqlCodeGenerator, &SQLCodeGenerator::onGenerateSqlCodeClicked);
    connect(sqlCodeGenerator, &SQLCodeGenerator::codeGenerated, editor, &CodeEditor::onCodeGenerated);
    connect(sqlCodeGenerator, &SQLCodeGenerator::codeParsed, diagram, &Diagram::drawDiagram);
}



ProjectWindow::~ProjectWindow()
{
    delete ui;
}



void ProjectWindow::onOpenProject(const Project &project) {
    std::cout << project.path.toStdString() << std::endl;
    this->project = project;

    // open the text file
    QString textFilePath = project.path + QDir::separator() + "project_text.txt";
    QFile textFile(textFilePath);
    if (textFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&textFile);
        QString text = in.readAll();
        editor->setPlainText(text); // Set text to the code editor
        textFile.close();
    } else {
        qDebug() << "Error: Unable to open file for reading" << textFilePath;
    }

    // open the scene
    QString diagramFilePath = project.path + QDir::separator() + "project_data.json";
    QFile diagramFile(diagramFilePath);
    if (diagramFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = diagramFile.readAll();
        diagramFile.close();

        QJsonDocument loadDoc(QJsonDocument::fromJson(jsonData));
        QJsonObject sceneDataJSON = loadDoc.object();
        diagram->openScene(sceneDataJSON);
    } else {
        qDebug() << "Error: Unable to open file for reading" << diagramFilePath;
    }
}



void ProjectWindow::saveProject() {

    // save the text file
    QString textFilePath = project.path + QDir::separator() + "project_text.txt";
    QFile textFile(textFilePath);
    if (textFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&textFile);
        out << editor->toPlainText();
        textFile.close();
    } else {
        qDebug() << "Error: Unable to open file for writing" << textFilePath;
    }

    // save the diagram
    QString diagramFilePath = project.path + QDir::separator() + "project_data.json";
    QFile diagramFile(diagramFilePath);
    if (diagramFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        diagramFile.resize(0);
        QJsonDocument saveDoc(diagram->getSceneData());
        diagramFile.write(saveDoc.toJson());
        diagramFile.close();
    } else {
        // Handle error opening the file
        QMessageBox::warning(this, tr("Error"), tr("Unable to open file for writing."));
    }
}
