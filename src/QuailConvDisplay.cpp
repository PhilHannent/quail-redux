#include "QuailConvDisplay.h"

#include <QtWebKitWidgets/QWebFrame>
#include <QDebug>

QuailConvDisplay::QuailConvDisplay(QWidget *parent) :
    QWebView(parent)
{
    qDebug() << "QuailConvDisplay::QuailConvDisplay";
    QString css(".leftEntry { color:blue; }\n.rightEntry { color:red; text-align:right}\n ");
    QString javaScript("function addRow(data) { var t = document.getElementById('convTable\'); r = t.insertRow(t.rows.length); r.innerHTML = data; }");
    QString htmlHead("<html><head><style>");

    setHtml(htmlHead +
            css +
            "</style><script>" + javaScript +
            "</script></head><body><table id=\"convTable\"></table></body></html>");
}

QuailConvDisplay::~QuailConvDisplay()
{

}

void
QuailConvDisplay::slotAddMessage(bool bLeftAligned, QString sHtml)
{
    qDebug() << "QuailConvDisplay::slotAddMessage" << sHtml;
    QString newMessage("");
    if (bLeftAligned)
        newMessage += "<td class=\"leftEntry\">";
    else
        newMessage += "<td class=\"rightEntry\">";

    newMessage += sHtml;
    newMessage += "</td>";

    QWebFrame *frame = this->page()->mainFrame();
    frame->evaluateJavaScript("addRow('" + newMessage + "');");
}
