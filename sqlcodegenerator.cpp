#include "sqlcodegenerator.h"

SQLCodeGenerator::SQLCodeGenerator() {}

void SQLCodeGenerator::onGenerateSqlCodeClicked() {
    emit codeGenerated("Hello");
}
