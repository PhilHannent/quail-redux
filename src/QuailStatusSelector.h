#ifndef QUAILSTATUSSELECTOR_H
#define QUAILSTATUSSELECTOR_H

#include <QWidget>

#include <glib.h>
#include <libpurple/account.h>
#include <libpurple/status.h>


class QComboBox;
class QLabel;

class QuailStatusSelector : public QWidget
{
    Q_OBJECT
public:
    explicit QuailStatusSelector(QWidget *parent = 0);
    
signals:
    void signalBuddyIconChanged(QPixmap &pix, QString fileName);
    
public slots:
    void updateStatusSelector();
    void slotPickBuddyIcon();
    void slotStatusChanged(int index);
    void slotSetStatus(int statusId);

private:
    PurpleStatusType *find_status_type_by_index(const PurpleAccount *account,
                                                   gint active);

    QComboBox *statusSelector;
    QLabel *buddyIcon;

};

#endif // QUAILSTATUSSELECTOR_H
