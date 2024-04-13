#ifndef SQLCODEGENERATOR_H
#define SQLCODEGENERATOR_H

#include <QObject>

class SQLCodeGenerator: public QObject
{
    Q_OBJECT
public:
    SQLCodeGenerator();

signals:
    void codeGenerated(QString code);

public slots:
    void onGenerateSqlCodeClicked();
};

#endif // SQLCODEGENERATOR_H
