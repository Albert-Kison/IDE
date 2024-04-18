#ifndef TABLE_H
#define TABLE_H

#include <QString>
#include <QList>

struct Column {
    QString name;
    QString type;
    bool isPrimary;

    // Constructor
    Column(const QString& name, const QString& type, const bool isPrimary) : name(name), type(type), isPrimary(isPrimary) {}
};

struct Table {
    QString name;
    QList<Column> columns;

    // Constructor
    Table(const QString& name) : name(name) {}

    // Method to add a column
    void addColumn(const QString& name, const QString& type, const bool isPrimary = false) {
        columns.append(Column(name, type, isPrimary));
    }
};

#endif // TABLE_H
