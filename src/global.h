#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

#define QUAIL_ACTION_STATUS_NEW 30000
#define QUAIL_ACTION_STATUS_SAVED 30001

static QString
stripUnderscores(QString text)
{
    if (text.startsWith("_")) {
        text = text.remove(0,1);
    }
    return text;
}


#endif // GLOBAL_H
