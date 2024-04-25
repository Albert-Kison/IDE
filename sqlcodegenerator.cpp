#include "sqlcodegenerator.h"
#include <iostream>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlQuery>

// #include "../Desktop/cpp_sql/sql-parser/src/SQLParser.h"
// #include "../Desktop/cpp_sql/sql-parser/src/util/sqlhelper.h"

std::ostream&  operator <<(std::ostream &stream,const QString &str)
{
    stream << str.toLatin1().constData();
    return stream;
}

SQLCodeGenerator::SQLCodeGenerator() {}

void SQLCodeGenerator::onGenerateSqlCodeClicked(QList<Table> &tables, QList<Relationship> &relationships) {
    QString sqlCode = "";

    // tables code
    foreach (Table table, tables) {
        sqlCode += "CREATE TABLE " + table.name + " (\n";
        QString primaryName = "";

        for (int i = 0; i < table.columns.size(); ++i) {
            sqlCode += "    " + table.columns[i].name + " " + table.columns[i].type + ",\n";
            if (table.columns[i].isPrimary) {
                primaryName = table.columns[i].name;
            }
        }

        // Add primary key constraint
        sqlCode += "    PRIMARY KEY (" + primaryName + "),\n";

        // Add foreign key constraints
        foreach (Relationship rel, relationships) {
            if (rel.childTableName == table.name) {
                sqlCode += "    FOREIGN KEY (" + rel.childColumnName + ") REFERENCES " + rel.parentTableName + "(" + rel.parentColumnName + ") ON DELETE CASCADE ON UPDATE CASCADE,\n";
            }
        }

        // Remove the trailing comma from the last column definition
        sqlCode.chop(2);

        sqlCode += "\n);\n";
    }

    emit codeGenerated(sqlCode);
}




void SQLCodeGenerator::parseSql(QString &code) {
    std::cout << "Clicked parse sql" << std::endl;
    std::cout << code << std::endl;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // Use in-memory database for this example
    if (!db.open()) {
        std::cout << "Error opening database" << std::endl;
        return;
    }

    // Split the SQL code into individual statements
    QStringList statements = code.split(";", Qt::SkipEmptyParts);
    QSqlQuery query(db);

    // Map to store foreign key relationships
    QMap<QString, Relationship> foreignKeyMap;

    // Execute SQL statements
    for (const QString& statement : statements) {
        // Execute each statement separately
        if (!query.exec(statement)) {
            std::cout << "Error executing SQL statement:" << std::endl;
            db.close();
            return;
        }
    }

    // Retrieve foreign key relationships
    for (const QString& tableName : db.tables()) {
        QString foreignKeyQuery = QString("PRAGMA foreign_key_list(%1)").arg(tableName);
        if (query.exec(foreignKeyQuery)) {
            while (query.next()) {
                QString columnName = query.value(3).toString();
                QString referencedTable = query.value(2).toString();
                QString referencedColumn = query.value(4).toString();

                // Check if referenced table exists in the database
                if (db.tables().contains(referencedTable)) {
                    // Store the foreign key relationship
                    foreignKeyMap.insert(columnName, Relationship(referencedTable, referencedColumn, tableName, columnName));
                }
            }
        }
    }

    // Retrieve table names
    QStringList tables = db.tables();
    QList<Table> diagramTables;
    QList<Relationship> diagramRelationships;
    for (const QString& tableName : tables) {
        std::cout << "Table:" << tableName << std::endl;
        Table table(tableName);

        // Retrieve table columns and their data types
        QSqlRecord record = db.record(tableName);
        for (int i = 0; i < record.count(); ++i) {
            QSqlField field = record.field(i);
            std::cout << "    Column:" << field.name() << ", Type:" << field.metaType().name() << std::endl;
            table.addColumn(field.name(), field.metaType().name());
        }

        // Retrieve primary key columns using PRAGMA table_info
        QString primaryKeyQuery = QString("PRAGMA table_info(%1)").arg(tableName);
        if (query.exec(primaryKeyQuery)) {
            while (query.next()) {
                bool isPrimaryKey = query.value(5).toBool();
                if (isPrimaryKey) {
                    QString columnName = query.value(1).toString();
                    std::cout << "    Primary Key:" << columnName << std::endl;

                    for (int i = 0; i < table.columns.size(); ++i) {
                        if (table.columns[i].name == columnName) {
                            table.columns[i].isPrimary = true;
                            break;
                        }
                    }
                }
            }
        }
        diagramTables << table;

        // Display foreign key relationships
        for (auto it = foreignKeyMap.constBegin(); it != foreignKeyMap.constEnd(); ++it) {
            const Relationship& relationship = it.value();
            if (relationship.childTableName == tableName) {
                std::cout << "    Foreign Key:" << relationship.parentColumnName
                          << ", Referenced Column:" << relationship.childColumnName
                          << ", Referenced Table:" << relationship.parentTableName << std::endl;
            }
            diagramRelationships << relationship;
        }
    }

    // Close the database connection
    db.close();

    emit codeParsed(diagramTables, diagramRelationships);
}
