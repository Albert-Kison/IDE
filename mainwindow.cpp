#include "mainwindow.h"
#include "QtCore/qfile.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtWidgets/qlabel.h"
#include "QtWidgets/qlineedit.h"
#include "QtWidgets/qpushbutton.h"
#include "./ui_mainwindow.h"
#include "QtWidgets/qscrollarea.h"
// #include "diagram.h"
#include <QVBoxLayout>
#include <QDebug>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QInputDialog>

// std::ostream&  operator <<(std::ostream &stream,const QString &str)
// {
//     stream << str.toLatin1().constData(); //or: stream << str.toStdString(); //??
//     return stream;
// }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , layout(nullptr), projects(nullptr) // Initialize pointers
{
    ui->setupUi(this);
    setWindowTitle("Projects Manager");
    std::cout << "path in main" << std::endl;
    initUI();
}

MainWindow::~MainWindow()
{
    delete layout;
    delete projects; // Delete the projects list
    delete ui;
}



void MainWindow::displayProjects()
{
    if (projects) {
        delete projects; // Delete previous projects list to prevent memory leaks
        projects = nullptr; // Reset pointer
    }

    projects = new QList<Project>;

    QFile file("projects.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
        QJsonArray jsonArray = jsonDoc.array();

        foreach(const QJsonValue &value, jsonArray) {
            QJsonObject obj = value.toObject();
            Project project(obj["name"].toString(), obj["path"].toString());
            projects->append(project);
        }

        file.close();
    }

    // projectsVerticalLayout = new QVBoxLayout;
    // projectsVerticalLayout->setAlignment(Qt::AlignTop);
    createProjectsLayout(projectsVerticalLayout, projects);
    layout->addLayout(projectsVerticalLayout);
}



void MainWindow::saveProjects() {
    QFile file("projects.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray jsonArray;
        foreach(const Project &project, *projects) {
            QJsonObject obj;
            obj["name"] = project.name;
            obj["path"] = project.path;
            jsonArray.append(obj);
        }

        QJsonDocument jsonDoc(jsonArray);
        file.write(jsonDoc.toJson());

        file.close();
    }
}



void MainWindow::createProjectsLayout(QVBoxLayout *projectsVerticalLayout, QList<Project> *projects) {

    for (int i = 0; i < projects->size(); ++i) {
        QHBoxLayout *horizontalLayout = new QHBoxLayout;

        QVBoxLayout *projectInfoVerticalLayout = new QVBoxLayout;
        QLabel *projectNameLabel = new QLabel(projects->at(i).name);
        QLabel *projectPathLabel = new QLabel(projects->at(i).path);
        projectInfoVerticalLayout->addWidget(projectNameLabel);
        projectInfoVerticalLayout->addWidget(projectPathLabel);
        horizontalLayout->addLayout(projectInfoVerticalLayout);

        QPushButton *openProjectButton = new QPushButton("Open");
        openProjectButton->setFixedWidth(100);
        horizontalLayout->addWidget(openProjectButton);

        connect(openProjectButton, &QPushButton::clicked, [this, i, projects]() {
            openProjectButtonPressed(projects->at(i));
        });

        QPushButton *deleteProjectButton = new QPushButton("Delete");
        deleteProjectButton->setFixedWidth(100);
        horizontalLayout->addWidget(deleteProjectButton);

        connect(deleteProjectButton, &QPushButton::clicked, [this, i, projectsVerticalLayout, projects]() {
            deleteProjectButtonPressed(projects->at(i));

            projects->removeAt(i); // Remove the project at the specified index
            saveProjects(); // Save the updated project list
            deleteLayout(projectsVerticalLayout);
            createProjectsLayout(projectsVerticalLayout, projects);
        });

        projectsVerticalLayout->addLayout(horizontalLayout);
    }
}



void MainWindow::initUI() {

    // Create a container widget to hold the CodeEditor
    QWidget *centralWidget = new QWidget(this);
    layout = new QVBoxLayout(centralWidget);

    // Create a scroll area to hold the projects layout
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true); // Allow the scroll area to resize its widget
    QWidget *scrollWidget = new QWidget(scrollArea);
    projectsVerticalLayout = new QVBoxLayout(scrollWidget); // Initialize projects layout inside the scroll widget
    projectsVerticalLayout->setAlignment(Qt::AlignTop);
    scrollWidget->setLayout(projectsVerticalLayout);
    scrollArea->setWidget(scrollWidget);
    layout->addWidget(scrollArea);

    displayProjects();

    QPushButton *createButton = new QPushButton("Create");
    layout->addWidget(createButton);
    connect(createButton, &QPushButton::clicked, this, &MainWindow::createButtonPressed);

    centralWidget->setLayout(layout);

    // Set the central widget of MainWindow to the container widget
    setCentralWidget(centralWidget);
}



void MainWindow::deleteLayout(QLayout *layout) {
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (QLayout *childLayout = child->layout()) {
            // Recursively delete child layouts
            deleteLayout(childLayout);
        } else if (QWidget *widget = child->widget()) {
            // Remove and delete widgets
            layout->removeWidget(widget);
            delete widget;
        }
        // Delete the layout item
        delete child;
    }
}



void MainWindow::openProjectButtonPressed(const Project& project) {
    hide();

    projectWindow = new ProjectWindow(this);
    connect(this, &MainWindow::openProject, projectWindow, &ProjectWindow::onOpenProject);
    connect(projectWindow, &ProjectWindow::finished, this, [this]() {
        show();
        delete projectWindow;
    });

    std::cout << "Emit open project" << std::endl;
    emit openProject(project);

    projectWindow->show();
}



void MainWindow::deleteProjectButtonPressed(const Project& project) {
    QDir dir(project.path);
    if (dir.exists()) {
        // Remove all files and directories within the project directory
        dir.removeRecursively();
    } else {
        qDebug() << "Error: Project directory does not exist";
    }
}



void MainWindow::createButtonPressed() {
    hide();

    // Get the desired directory name from the user
    QString projectName = QInputDialog::getText(this, tr("Enter Project Name"), tr("Project Name:"), QLineEdit::Normal, "");

    if (!projectName.isEmpty()) {
        // Open file dialog to get the folder location
        QString folderName = QFileDialog::getExistingDirectory(this, tr("Save Project"), QDir::homePath());

        if (!folderName.isEmpty()) {

            // Create the folder with the provided name
            QString folderPath = folderName + QDir::separator() + projectName;
            createFolder(folderPath);

            // Add the project using the selected folder name
            Project newProject(projectName, folderPath);
            projects->append(newProject);

            // Save the projects list
            saveProjects();

            // Create the JSON file in the folder
            QString jsonFilePath = folderPath + QDir::separator() + "project_data.json";
            createJsonFile(jsonFilePath, newProject);

            // Create the text file in the folder
            QString textFilePath = folderPath + QDir::separator() + "project_text.txt";
            createTextFile(textFilePath, newProject);

            // Open the project window
            projectWindow = new ProjectWindow(this);

            connect(this, &MainWindow::openProject, projectWindow, &ProjectWindow::onOpenProject);
            connect(projectWindow, &ProjectWindow::finished, this, [this]() {
                deleteLayout(projectsVerticalLayout);
                createProjectsLayout(projectsVerticalLayout, projects);
                show();
                delete projectWindow;
            });

            emit openProject(newProject);

            projectWindow->show();
        }
    } else {
        // Show the main window again if the user cancels entering the directory name
        show();
    }
}



void MainWindow::createFolder(const QString& folderPath) {
    // Create the folder
    QDir dir;
    if (!dir.mkpath(folderPath)) {
        // Handle error if unable to create the folder
        qDebug() << "Error: Unable to create folder";
    }
}

void MainWindow::createJsonFile(const QString& filePath, const Project &project) {
    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["project_name"] = project.name;

    // Convert the JSON object to a JSON document
    QJsonDocument jsonDoc(jsonObject);

    // Open the file for writing
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        // Write the JSON data to the file
        file.write(jsonDoc.toJson());
        file.close();
    } else {
        // Handle error if unable to open the file
        qDebug() << "Error: Unable to open file for writing";
    }
}

void MainWindow::createTextFile(const QString& filePath, const Project &project) {
    // Open the file for writing
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Create a text stream to write to the file
        QTextStream out(&file);
        out << "-- " + project.name + "\n";
        file.close();
    } else {
        // Handle error if unable to open the file
        qDebug() << "Error: Unable to open file for writing";
    }
}
