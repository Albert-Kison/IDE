#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <QString>

struct Relationship {
    QString parentTableName;
    QString parentColumnName;
    QString childTableName;
    QString childColumnName;

    // Constructor
    Relationship(const QString& parentTableName, const QString& parentColumnName,
                 const QString& childTableName, const QString& childColumnName)
        : parentTableName(parentTableName), parentColumnName(parentColumnName),
        childTableName(childTableName), childColumnName(childColumnName) {}
};

#endif // RELATIONSHIP_H
