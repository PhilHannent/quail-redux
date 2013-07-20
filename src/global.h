#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

static QString
stripUnderscores(QString text)
{
    if (text.startsWith("_")) {
        text = text.remove(0,1);
    }
    return text;
}


#endif // GLOBAL_H
