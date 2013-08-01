#include "QuailConvDisplay.h"

#include <QtWebKitWidgets/QWebFrame>
#include <QDebug>
#include <QFile>

QuailConvDisplay::QuailConvDisplay(QWidget *parent) :
    QWebView(parent)
{
    qDebug() << "QuailConvDisplay::QuailConvDisplay";
    QFile res(":/conv.html");
    res.open(QIODevice::ReadOnly|QIODevice::Text);
    setHtml(res.readAll());
}

QuailConvDisplay::~QuailConvDisplay()
{

}

void
QuailConvDisplay::slotAddMessage(bool bLeftAligned, QString sHtml)
{
    qDebug() << "QuailConvDisplay::slotAddMessage" << sHtml;
    QString newMessage("addRow(\'");
    if (bLeftAligned)
        newMessage += "<td>";
    else
        newMessage += "<td class=\"rightEntry\">";

    sHtml.replace("\n", "");
    newMessage += sHtml;
    newMessage += "</td>";
    newMessage.replace("\"", "\\\"");
    newMessage += "\');";

    QWebFrame *frame = this->page()->mainFrame();
    qDebug() << frame->evaluateJavaScript(newMessage);


}
