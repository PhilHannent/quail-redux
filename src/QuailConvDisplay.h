#ifndef QUAILCONVDISPLAY_H
#define QUAILCONVDISPLAY_H

#ifdef USE_QT5
#include <QtWebKitWidgets/QWebView>
#else
#include <QWebView>
#endif

class QuailConvDisplay : public QWebView
{
    Q_OBJECT
public:
    QuailConvDisplay(QWidget *parent = 0);
    ~QuailConvDisplay();

signals:
    
public slots:
    void slotAddMessage(bool bLeftAligned, QString sHtml);
};

#endif // QUAILCONVDISPLAY_H
