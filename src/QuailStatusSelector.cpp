#include "QuailStatusSelector.h"


#include "libpurple/conversation.h"
#include "libpurple/status.h"

#include "global.h"

#include <QActionGroup>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

QuailStatusSelector::QuailStatusSelector(QWidget *parent) :
    QWidget(parent)
{
    qDebug() << "QuailStatusSelector::QuailStatusSelector";
    QHBoxLayout *hbox = new QHBoxLayout(this);
    statusSelector = new QComboBox(this);
    connect(statusSelector, SIGNAL(activated(int)),
            this, SLOT(slotStatusChanged(int)));

    hbox->addWidget(statusSelector);
    buddyIcon = new QLabel(this);
    buddyIcon->setFixedSize(32,32);

    hbox->addWidget(buddyIcon);
    this->setLayout(hbox);
    statusSelector->setIconSize(QSize(32,32));
    //TODO: Fix overlapping icons on the dropdown
    statusSelector->adjustSize();
    updateStatusSelector();
}

void
QuailStatusSelector::updateStatusSelector()
{
    qDebug() << "QuailStatusSelector::updateStatusSelector()";
    /* Fetch a list of possible states */
    statusSelector->addItem(QIcon(":/data/images/status/available.png"),
                            tr("Available"),
                            PURPLE_STATUS_AVAILABLE);
    statusSelector->addItem(QIcon(":/data/images/status/away.png"),
                            tr("Away"),
                            PURPLE_STATUS_AWAY);
    statusSelector->addItem(QIcon(":/data/images/status/busy.png"),
                            tr("Do not disturb"),
                            PURPLE_STATUS_UNAVAILABLE);
    statusSelector->addItem(QIcon(":/data/images/status/invisible.png"),
                            tr("Invisible"),
                            PURPLE_STATUS_INVISIBLE);
    statusSelector->addItem(QIcon(":/data/images/status/offline.png"),
                            tr("Offline"),
                            PURPLE_STATUS_OFFLINE);

    //statusSelector->insertSeparator(statusSelector->count()+1);
    //TODO: Add popular statuses

    statusSelector->insertSeparator(statusSelector->count()+1);
    statusSelector->addItem(tr("New..."), QUAIL_ACTION_STATUS_NEW);
    statusSelector->addItem(tr("Saved..."), QUAIL_ACTION_STATUS_SAVED);
    statusSelector->adjustSize();

}

void
QuailStatusSelector::slotStatusChanged(int index)
{
    qDebug() << "QuailStatusSelector::slotStatusChanged";
    Q_UNUSED(index)

    switch (statusSelector->itemData(index).toInt())
    {
    case QUAIL_ACTION_STATUS_NEW:
        qDebug() << "QuailStatusSelector::slotStatusChanged.NEW";
        break;
    case QUAIL_ACTION_STATUS_SAVED:
        qDebug() << "QuailStatusSelector::slotStatusChanged.SAVED";
        break;
    case PURPLE_STATUS_OFFLINE:
        qDebug() << "QuailStatusSelector::slotStatusChanged.OFFLINE";
        slotSetStatus(index);
        break;
    case PURPLE_STATUS_INVISIBLE:
        qDebug() << "QuailStatusSelector::slotStatusChanged.INVISIBLE";
        slotSetStatus(index);
        break;
    case PURPLE_STATUS_UNAVAILABLE:
        qDebug() << "QuailStatusSelector::slotStatusChanged.UNAVAILABLE";
        slotSetStatus(index);
        break;
    case PURPLE_STATUS_AWAY:
        qDebug() << "QuailStatusSelector::slotStatusChanged.AWAY";
        slotSetStatus(index);
        break;
    case PURPLE_STATUS_AVAILABLE:
        qDebug() << "QuailStatusSelector::slotStatusChanged.AVAILABLE";
        slotSetStatus(index);
        break;
    default:
        /* Must be a custom status */
        break;

    }

}

void
QuailStatusSelector::slotSetStatus(int statusId)
{
    qDebug() << "QuailStatusSelector::slotSetStatus" << statusId;
    GList *accounts = purple_accounts_get_all_active();
    GList *node;
    PurpleAccount *account;
    qDebug() << "QuailStatusSelector::slotSetStatus.1";

    for (node = accounts; node != NULL; node = node->next)
    {
        qDebug() << "QuailStatusSelector::slotSetStatus.2";
//        PurpleStatusType activeStatus;
        PurpleStatusType *status_type;
        //PurpleStatus *status;
        account = (PurpleAccount*)node->data;
        const char *id = NULL;
        qDebug() << "QuailStatusSelector::slotSetStatus.3";
//        activeStatus = purple_account_get_active_status(account);
        status_type = find_status_type_by_index(account, statusId);
        id = purple_status_type_get_id(status_type);
        qDebug() << "QuailStatusSelector::slotSetStatus.4";
        purple_account_set_status(account, id, TRUE, NULL);
        qDebug() << "QuailStatusSelector::slotSetStatus.5";
    }
    qDebug() << "QuailStatusSelector::slotSetStatus.end";
}


/* COPIED from gtkstatusbox.c, this should be in purple */
PurpleStatusType *
QuailStatusSelector::find_status_type_by_index(const PurpleAccount *account,
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
QuailStatusSelector::slotPickBuddyIcon()
{
    qDebug() << "QuailStatusSelector::slotPickBuddyIcon()";

}
