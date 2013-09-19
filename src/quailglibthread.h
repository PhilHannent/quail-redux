#ifndef QUAILGLIBTHREAD_H
#define QUAILGLIBTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <glib/gmain.h>


class QuailGlibThread : public QThread
{
    Q_OBJECT

public:
    QuailThread(QObject *parent = 0);
    ~QuailThread();

    bool start();
    void stop();
    void run();

    GMainContext *mainContext();

protected:
    bool success;
    GMainContext *mainContext;
    GMainLoop *mainLoop;
    QMutex m;
    QWaitCondition w;

    static gboolean cb_loop_started(gpointer data)
    {
        return ((Private *)data)->loop_started();
    }

    gboolean loop_started()
    {
        w.wakeOne();
        m.unlock();
        return FALSE;
    }

};


#endif // QUAILGLIBTHREAD_H
