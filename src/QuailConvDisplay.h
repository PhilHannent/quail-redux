#ifndef QUAILCONVDISPLAY_H
#define QUAILCONVDISPLAY_H

#include <QtWebKitWidgets/QWebView>

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
