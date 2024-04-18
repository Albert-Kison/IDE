#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <QString>

struct Relationship {
    QString parentTable;
    QString parentColumn;
    QString childTable;
    QString childColumn;

    // Constructor
    Relationship(const QString& parentTable, const QString& parentColumn,
                 const QString& childTable, const QString& childColumn)
        : parentTable(parentTable), parentColumn(parentColumn),
        childTable(childTable), childColumn(childColumn) {}
};

#endif // RELATIONSHIP_H
