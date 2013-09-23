#ifndef QUAILGLIBTHREAD_H
#define QUAILGLIBTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <glib.h>

class QuailGlibThread : public QThread
{
    Q_OBJECT

public:
    QuailGlibThread(QObject *parent = 0);
    ~QuailGlibThread();

    bool start();
    void stop();
    void run();

    GMainContext *getMainContext();

protected:
    bool success;
    GMainContext *mainContext;
    GMainLoop *mainLoop;
    QMutex m;
    QWaitCondition w;

    static gboolean cb_loop_started(gpointer data)
    {
        return ((QuailGlibThread *)data)->loop_started();
    }

    gboolean loop_started()
    {
        w.wakeOne();
        m.unlock();
        return FALSE;
    }

};


#endif // QUAILGLIBTHREAD_H
