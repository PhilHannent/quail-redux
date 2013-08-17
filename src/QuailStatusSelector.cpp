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
}

void
QuailStatusSelector::slotPickBuddyIcon()
{
    qDebug() << "QuailStatusSelector::slotPickBuddyIcon()";

}
