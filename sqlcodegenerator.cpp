#include "sqlcodegenerator.h"

SQLCodeGenerator::SQLCodeGenerator() {}

void SQLCodeGenerator::onGenerateSqlCodeClicked(QList<Table> &tables) {
    QString sqlCode = "";
    foreach (Table table, tables) {
        sqlCode += "CREATE TABLE " + table.name + " (\n";
        QString primaryName = "";

        for (int i = 0; i < table.columns.size(); ++i) {
            sqlCode += "    " + table.columns[i].name + " " + table.columns[i].type + ",\n";
            if (table.columns[i].isPrimary) {
                primaryName = table.columns[i].name;
            }
        }

        sqlCode += "    PRIMARY KEY (" + primaryName + ")\n";

        sqlCode += ");\n";
    }

    emit codeGenerated(sqlCode);
}
