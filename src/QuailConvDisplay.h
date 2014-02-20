#ifndef QUAILCONVDISPLAY_H
#define QUAILCONVDISPLAY_H

#ifdef USE_QT5
#include <QtWebKitWidgets/QWebView>
#else
#include <QWebView>
#endif

class quail_conv_display : public QWebView
{
    Q_OBJECT
public:
    quail_conv_display(QWidget *parent = 0);
    ~quail_conv_display();

signals:
    
public slots:
    void slotAddMessage(bool bLeftAligned, QString sHtml);
};

#endif // QUAILCONVDISPLAY_H
