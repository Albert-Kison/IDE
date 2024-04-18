#ifndef SQLCODEGENERATOR_H
#define SQLCODEGENERATOR_H

#include <QObject>
#include "Table.h"
#include "Relationship.h"

class SQLCodeGenerator: public QObject
{
    Q_OBJECT
public:
    SQLCodeGenerator();

signals:
    void codeGenerated(QString code);

public slots:
    void onGenerateSqlCodeClicked(QList<Table> &tables);
};

#endif // SQLCODEGENERATOR_H
