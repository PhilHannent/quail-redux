#include "quailglibthread.h"

QuailGlibThread::QuailGlibThread(QObject *parent) :
    QThread(parent)
{

}

QuailGlibThread::~QuailGlibThread()
{
    stop();
}

bool QuailGlibThread::start()
{
    QMutexLocker locker(&m);
    QThread::start();
    w.wait(&m);
    return success;
}

void QuailGlibThread::stop()
{
    QMutexLocker locker(&m);
    if(mainLoop)
    {
        // thread-safe ?
        g_main_loop_quit(mainLoop);
        w.wait(&m);
    }

    wait();
}

GMainContext *QuailGlibThread::getMainContext()
{
    QMutexLocker locker(&m);
    return mainContext;
}

void QuailGlibThread::run()
{
    // this will be unlocked as soon as the mainloop runs
    m.lock();

    success = true;

    mainContext = g_main_context_new();
    mainLoop = g_main_loop_new(mainContext, FALSE);

    // deferred call to loop_started()
    GSource *timer = g_timeout_source_new(0);
    g_source_attach(timer, mainContext);
    g_source_set_callback(timer, cb_loop_started, this, NULL);

    // kick off the event loop
    g_main_loop_run(mainLoop);

    QMutexLocker locker(&m);
    g_main_loop_unref(mainLoop);
    mainLoop = 0;
    g_main_context_unref(mainContext);
    mainContext = 0;
    w.wakeOne();
}
