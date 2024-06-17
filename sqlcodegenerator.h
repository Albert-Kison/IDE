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

    void parseSql(QString &code);

signals:
    void codeGenerated(QString code);
    void codeParsed(QList<Table> &tables, QList<Relationship> &relationships);
    void onError(QString &error);

public slots:
    void onGenerateSqlCodeClicked(QList<Table> &tables, QList<Relationship> &relationships);
};

#endif // SQLCODEGENERATOR_H
