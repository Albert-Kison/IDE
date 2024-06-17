#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <QString>

struct Relationship {

    enum RelationshipType {
        OneToOne,
        OneToMany,
        ManyToMany
    };

    QString parentTableName;
    QString parentColumnName;
    QString childTableName;
    QString childColumnName;
    RelationshipType relationshipType;

    // Constructor
    Relationship(const QString& parentTableName, const QString& parentColumnName,
                 const QString& childTableName, const QString& childColumnName,
                 RelationshipType relationshipType)
        : parentTableName(parentTableName), parentColumnName(parentColumnName),
        childTableName(childTableName), childColumnName(childColumnName),
        relationshipType(relationshipType)    {}
};

#endif // RELATIONSHIP_H
