#ifndef PROJECT_H
#define PROJECT_H

#include "QtCore/qstring.h"
struct Project
{
    QString name;
    QString path;

    Project(const QString& name, const QString& path) : name(name), path(path){}
};

#endif // PROJECT_H
