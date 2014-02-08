#include "QuailStatusSelector.h"


#include "libpurple/conversation.h"
#include "libpurple/status.h"

#include "global.h"

#include <QActionGroup>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

quail_status_selector::quail_status_selector(QWidget *parent) :
    QWidget(parent)
{
    qDebug() << "QuailStatusSelector::QuailStatusSelector";
    QHBoxLayout *hbox = new QHBoxLayout(this);
    QVBoxLayout *vbox = new QVBoxLayout(this);
    m_status_cbo = new QComboBox(this);
    connect(m_status_cbo, SIGNAL(activated(int)),
            this, SLOT(slot_status_changed(int)));

    m_mood = new QLineEdit(this);
    vbox->addWidget(m_status_cbo);
    vbox->addWidget(m_mood);
    hbox->addLayout(vbox);
    m_buddy_icon = new QLabel(this);
    m_buddy_icon->setFixedSize(32,32);

    hbox->addWidget(m_buddy_icon);
    this->setLayout(hbox);
    //m_status_cbo->setIconSize(QSize(32,32));
    //TODO: Fix overlapping icons on the dropdown
    m_status_cbo->adjustSize();
    m_mood_timeout = new QTimer(this);
    m_mood_timeout->setInterval(5000);
    slot_update_status_selector();
}

void
quail_status_selector::slot_update_status_selector()
{
    qDebug() << "QuailStatusSelector::updateStatusSelector()";
    /* Fetch a list of possible states */
    m_status_cbo->addItem(QIcon(":/data/images/status/available.png"),
                            tr("Available"),
                            PURPLE_STATUS_AVAILABLE);
    m_status_cbo->addItem(QIcon(":/data/images/status/away.png"),
                            tr("Away"),
                            PURPLE_STATUS_AWAY);
    m_status_cbo->addItem(QIcon(":/data/images/status/busy.png"),
                            tr("Do not disturb"),
                            PURPLE_STATUS_UNAVAILABLE);
    m_status_cbo->addItem(QIcon(":/data/images/status/invisible.png"),
                            tr("Invisible"),
                            PURPLE_STATUS_INVISIBLE);
    m_status_cbo->addItem(QIcon(":/data/images/status/offline.png"),
                            tr("Offline"),
                            PURPLE_STATUS_OFFLINE);

    //statusSelector->insertSeparator(statusSelector->count()+1);
    //TODO: Add popular statuses

    m_status_cbo->insertSeparator(m_status_cbo->count()+1);
    m_status_cbo->addItem(tr("New..."), QUAIL_ACTION_STATUS_NEW);
    m_status_cbo->addItem(tr("Saved..."), QUAIL_ACTION_STATUS_SAVED);
    m_status_cbo->adjustSize();

}

void
quail_status_selector::slot_status_changed(int index)
{
    qDebug() << "quail_status_selector::slotStatusChanged";
    Q_UNUSED(index)

    switch (m_status_cbo->itemData(index).toInt())
    {
    case QUAIL_ACTION_STATUS_NEW:
        qDebug() << "quail_status_selector::slotStatusChanged.NEW";
        break;
    case QUAIL_ACTION_STATUS_SAVED:
        qDebug() << "quail_status_selector::slotStatusChanged.SAVED";
        break;
    case PURPLE_STATUS_OFFLINE:
        qDebug() << "quail_status_selector::slotStatusChanged.OFFLINE";
        slot_set_status(index);
        break;
    case PURPLE_STATUS_INVISIBLE:
        qDebug() << "quail_status_selector::slotStatusChanged.INVISIBLE";
        slot_set_status(index);
        break;
    case PURPLE_STATUS_UNAVAILABLE:
        qDebug() << "quail_status_selector::slotStatusChanged.UNAVAILABLE";
        slot_set_status(index);
        break;
    case PURPLE_STATUS_AWAY:
        qDebug() << "quail_status_selector::slotStatusChanged.AWAY";
        slot_set_status(index);
        break;
    case PURPLE_STATUS_AVAILABLE:
        qDebug() << "quail_status_selector::slotStatusChanged.AVAILABLE";
        slot_set_status(index);
        break;
    default:
        /* Must be a custom status */
        break;

    }

}

void
quail_status_selector::slot_set_status(int statusId)
{
    qDebug() << "quail_status_selector::slotSetStatus" << statusId;
    GList *accounts = purple_accounts_get_all_active();
    GList *node;
    PurpleAccount *account;
    qDebug() << "quail_status_selector::slotSetStatus.1";

    for (node = accounts; node != NULL; node = node->next)
    {
        qDebug() << "quail_status_selector::slotSetStatus.2";
//        PurpleStatusType activeStatus;
        PurpleStatusType *status_type;
        //PurpleStatus *status;
        account = (PurpleAccount*)node->data;
        const char *id = NULL;
        qDebug() << "quail_status_selector::slotSetStatus.3";
//        activeStatus = purple_account_get_active_status(account);
        status_type = find_status_type_by_index(account, statusId);
        id = purple_status_type_get_id(status_type);
        qDebug() << "quail_status_selector::slotSetStatus.4";
        purple_account_set_status(account, id, TRUE, NULL);
        qDebug() << "quail_status_selector::slotSetStatus.5";
    }
    qDebug() << "quail_status_selector::slotSetStatus.end";
}


/* COPIED from gtkstatusbox.c, this should be in purple */
PurpleStatusType *
quail_status_selector::find_status_type_by_index(const PurpleAccount *account,
                                               gint active)
{
    GList *l = purple_account_get_status_types(account);
    gint i;

    for (i = 0; l; l = l->next) {
        PurpleStatusType *status_type = (PurpleStatusType*)l->data;
        if (!purple_status_type_is_user_settable(status_type) ||
                purple_status_type_is_independent(status_type))
            continue;

        if (active == i)
            return status_type;
        i++;
    }

    return NULL;
}


void
quail_status_selector::slot_pick_buddy_icon()
{
    qDebug() << "quail_status_selector::slotPickBuddyIcon()";

}

void
quail_status_selector::slot_mood_timeout()
{
    //TODO: When this times out set the users mood

}
