#include "QuailConvDisplay.h"

#ifdef USE_QT5
#include <QtWebKitWidgets/QWebFrame>
#else
#include <QWebFrame>
#endif
#include <QDebug>

QuailConvDisplay::QuailConvDisplay(QWidget *parent) :
    QWebView(parent)
{
    qDebug() << "QuailConvDisplay::QuailConvDisplay";
    QString css(".leftEntry { color:blue; }\n.rightEntry { color:red; text-align:right}\n ");
    QString javaScript("function addRow(data)\n { var t = document.getElementById('convTable\');\n r = t.insertRow(t.rows.length);\n r.innerHTML = data;\n alert(); }");
    QString htmlHead("<html><head><style>body { background-color: green; }\n ");

    setHtml(htmlHead +
            css +
            "</style><script type=\"text/javascript\">" + javaScript +
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
    qDebug() << frame->toHtml();

}
