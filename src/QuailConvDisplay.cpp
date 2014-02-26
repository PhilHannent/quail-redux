#include "QuailConvDisplay.h"

#ifdef USE_QT5
#include <QtWebKitWidgets/QWebFrame>
#else
#include <QWebFrame>
#endif
#include <QDebug>
#include <QFile>

quail_conv_display::quail_conv_display(QWidget *parent) :
    QWebView(parent)
{
    qDebug() << "QuailConvDisplay::QuailConvDisplay";
    QFile res(":/conv.html");
    res.open(QIODevice::ReadOnly|QIODevice::Text);
    setHtml(res.readAll());
}

quail_conv_display::~quail_conv_display()
{

}

void
quail_conv_display::slotAddMessage(bool bLeftAligned, QString sHtml)
{
    qDebug() << "QuailConvDisplay::slotAddMessage" << sHtml;
    QString newMessage("addRow(\'");
    if (bLeftAligned)
        newMessage += "<td>";
    else
        newMessage += "<td class=\"rightEntry\">";

    sHtml.replace("\n", "");
    sHtml.replace("\'", "\\'");
    newMessage += sHtml;
    newMessage += "</td>";
    newMessage.replace("\"", "\\\"");
    newMessage += "\');";

    QWebFrame *frame = this->page()->mainFrame();
    qDebug() << frame->evaluateJavaScript(newMessage);


}
