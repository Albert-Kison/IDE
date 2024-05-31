#include "sqlcodegenerator.h"
#include <iostream>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlQuery>
#include <QSqlError>


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

        // loop over the columns
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
                sqlCode += "    FOREIGN KEY (" + rel.childColumnName + ") REFERENCES "
                           + rel.parentTableName + "(" + rel.parentColumnName + ") ON DELETE CASCADE ON UPDATE CASCADE,\n";
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
    std::cout << code.toStdString() << std::endl;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // Use in-memory database
    if (!db.open()) {
        std::cout << "Error opening database" << std::endl;
        return;
    }

    // Split the SQL code into individual statements
    QStringList statements = code.split(";", Qt::SkipEmptyParts);
    QSqlQuery query(db);

    // Execute SQL statements
    for (const QString& statement : statements) {

        // Skip empty lines
        if (statement.trimmed().isEmpty()) {
            continue;
        }

        // Execute each statement separately
        if (!query.exec(statement)) {
            std::cout << "Error executing SQL statement: " << query.lastError().text().toStdString() << std::endl;
            db.close();
            return;
        }
    }

    // Retrieve table names
    QStringList tables = db.tables();
    QList<Table> diagramTables;
    QList<Relationship> diagramRelationships;

    for (const QString& tableName : tables) {
        std::cout << "Table:" << tableName.toStdString() << std::endl;
        Table table(tableName);

        // Retrieve table columns and their data types
        QSqlRecord record = db.record(tableName);
        for (int i = 0; i < record.count(); ++i) {
            QSqlField field = record.field(i);
            QString typeName;
            switch (field.metaType().id()) {
                case QMetaType::Bool: typeName = "BOOLEAN"; break; // Map QVariant::Bool to SQL boolean type
                case QMetaType::Int: typeName = "INTEGER"; break;
                case QMetaType::UInt: typeName = "UINT"; break;
                case QMetaType::LongLong: typeName = "BIGINT"; break;
                case QMetaType::ULongLong: typeName = "BIGUINT"; break;
                case QMetaType::Double: typeName = "REAL"; break;
                case QMetaType::QString: typeName = "TEXT"; break;
                case QMetaType::QDate: typeName = "DATE"; break;
                case QMetaType::QDateTime: typeName = "DATE"; break;
                case QMetaType::QByteArray: typeName = "BLOB"; break;
                case QMetaType::Char: typeName = "CHAR"; break;
                // Add more cases for other SQL data types as needed
                default: typeName = "UNKNOWN";
            };
            std::cout << field.metaType().id() << std::endl;
            std::cout << "    Column:" << field.name().toStdString() << ", SQL Type:" << typeName.toStdString() << std::endl;
            table.addColumn(field.name(), typeName);
        }

        // Retrieve foreign key relationships using PRAGMA foreign_key_list
        QString foreignKeyQuery = QString("PRAGMA foreign_key_list(%1)").arg(tableName);
        if (query.exec(foreignKeyQuery)) {
            while (query.next()) {
                QString columnName = query.value(3).toString();
                QString referencedTable = query.value(2).toString();
                QString referencedColumn = query.value(4).toString();

                std::cout << "    Foreign Key:" << columnName.toStdString()
                          << ", Referenced Column:" << referencedColumn.toStdString()
                          << ", Referenced Table:" << referencedTable.toStdString() << std::endl;

                // Store the foreign key relationship
                diagramRelationships << Relationship(referencedTable, referencedColumn, tableName, columnName);
            }
        } else {
            std::cout << "Error retrieving foreign keys: " << query.lastError().text().toStdString() << std::endl;
        }

        // Retrieve primary key columns using PRAGMA table_info
        QString primaryKeyQuery = QString("PRAGMA table_info(%1)").arg(tableName);
        if (query.exec(primaryKeyQuery)) {
            while (query.next()) {
                bool isPrimaryKey = query.value(5).toBool();
                if (isPrimaryKey) {
                    QString columnName = query.value(1).toString();
                    std::cout << "    Primary Key:" << columnName.toStdString() << std::endl;

                    // Update primary key in the table
                    for (int i = 0; i < table.columns.size(); ++i) {
                        if (table.columns[i].name == columnName) {
                            table.columns[i].isPrimary = true;
                            break;
                        }
                    }
                }
            }
        } else {
            std::cout << "Error retrieving primary keys: " << query.lastError().text().toStdString() << std::endl;
        }

        diagramTables << table;
    }

    // Close the database connection
    db.close();

    emit codeParsed(diagramTables, diagramRelationships);
}
