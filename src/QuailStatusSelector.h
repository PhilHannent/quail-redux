#ifndef QUAILSTATUSSELECTOR_H
#define QUAILSTATUSSELECTOR_H

#include <QWidget>

#include <glib.h>
#include <libpurple/account.h>
#include <libpurple/status.h>

class QComboBox;
class QLabel;
class QLineEdit;

const int QUAIL_ACTION_STATUS_NEW = 30000;
const int QUAIL_ACTION_STATUS_SAVED  = 30001;

class quail_status_selector : public QWidget
{
    Q_OBJECT
public:
    explicit quail_status_selector(QWidget *parent = 0);
    
signals:
    void signal_buddy_icon_changed(QPixmap &pix, QString fileName);
    
public slots:
    void slot_update_status_selector();
    void slot_pick_buddy_icon();
    void slot_status_changed(int index);
    void slot_set_status(int status_id);

private slots:
    void slot_mood_timeout();

private:
    PurpleStatusType *find_status_type_by_index(const PurpleAccount *account,
                                                   gint active);

    QComboBox *m_status_cbo;
    QLabel *m_buddy_icon;
    QLineEdit *m_mood;
    QTimer *m_mood_timeout;
};

#endif // QUAILSTATUSSELECTOR_H
