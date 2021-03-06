#include "QuailBlistItem.h"
#include "QuailImageUtils.h"
#include "QuailProtocolUtils.h"

#include <QDebug>
#include <QPixmap>

quail_blist_item::quail_blist_item(QTreeWidget *parent, PurpleBlistNode *node)
    : QTreeWidgetItem(parent), node(node), dirty(true)
{
    init();
}

quail_blist_item::quail_blist_item(QTreeWidgetItem *parent, PurpleBlistNode *node)
    : QTreeWidgetItem(parent), node(node), dirty(true)
{
    init();
}

quail_blist_item::~quail_blist_item()
{
    if (node != NULL)
        node->ui_data = NULL;
}

PurpleBlistNode *
quail_blist_item::getBlistNode() const
{
    return node;
}

void
quail_blist_item::updateInfo()
{
    qDebug() << "QQuailBListItem::updateInfo";
    gsize len;
    const guchar *data = NULL;
    dirty = true;

    if (PURPLE_BLIST_NODE_IS_CONTACT(node))
    {
        PurpleContact *contact = (PurpleContact *)node;
        PurpleBuddy *buddy = purple_contact_get_priority_buddy(contact);
        PurplePresence *presence = purple_buddy_get_presence(buddy);
        char *tmp;

        if (buddy == NULL)
            return;


        if (isExpanded())
        {
            setIcon(0, QIcon(QPixmap(":/data/images/logo.png")));
        }
        else
        {
            QString text("");

//			if (buddy->evil > 0)
//				text += QString("%1%").arg(buddy->evil);

            if (purple_prefs_get_bool("/quail/blist/show_idle_times"))
            {
                int idle_secs = 0;
                /* Idle */
                if (purple_presence_is_idle(presence))
                {
                    idle_secs = purple_presence_get_idle_time(presence);
                    if (idle_secs > 0)
                    {
                        tmp = purple_str_seconds_to_string(time(NULL) - idle_secs);
                        //purple_notify_user_info_add_pair(user_info, tr("Idle"), tmp);

                        g_free(tmp);
                        time_t t;
                        int ihrs, imin;
                        QString idle;

                        time(&t);

                        ihrs = (t - idle_secs) / 3600;
                        imin = ((t - idle_secs) / 60) % 60;

                        if (ihrs > 0)
                            idle = QString("(%1:%2)").arg(ihrs).arg(imin, 2);
                        else
                            idle = QString("(%1)").arg(imin);

                        if (text != "")
                            text += "  ";

                        text += idle;

                    }
                }

            }
            setIcon(1, quail_blist_item::getBuddyStatusIcon((PurpleBlistNode *)buddy));
            setToolTip(1, text);
            //PurpleBuddyIcon *purpleIcon = purple_buddy_get_icon(buddy);
            PurpleBuddyIcon *purple_icon = purple_buddy_icons_find(buddy->account, buddy->name);
            qDebug() << "QQuailBListItem::updateInfo.Contact.3.2";
            if (purple_icon != NULL)
            {
                data = (guchar *)purple_buddy_icon_get_data(purple_icon, &len);
                qDebug() << "QQuailBListItem::updateInfo.Contact.3.3";
                QImage buddy_icon = QImage::fromData(data, len);
                purple_buddy_icon_unref(purple_icon);
                if (buddy_icon.size().width() > 0) {
                    qDebug() << "QQuailBListItem::updateInfo.Contact.3a" << buddy_icon.size().width();
                } else {
                    qDebug() << "QQuailBListItem::updateInfo.Contact.3b";
                }
                setIcon(2, QPixmap::fromImage(buddy_icon));
                purple_buddy_icon_unref(purple_icon);
            } else {
                qDebug() << "QQuailBListItem::updateInfo.Contact.4== No buddy icon";
            }
        }
        QString buddy_alias = getAlias(buddy);
        setText(1, buddy_alias);
    }
    else if (PURPLE_BLIST_NODE_IS_BUDDY(node))
    {
        qDebug() << "QQuailBListItem::updateInfo.Buddy";
        PurpleBuddy *buddy = (PurpleBuddy *)node;
        PurplePresence *presence = purple_buddy_get_presence(buddy);
        QString text = "";

        //if (buddy->evil > 0)
            //text += QString("%1%").arg(buddy->evil);

        if (purple_prefs_get_bool("/quail/blist/show_idle_times"))
        {
            int idle_secs = 0;
            /* Idle */
            if (purple_presence_is_idle(presence))
            {
                idle_secs = purple_presence_get_idle_time(presence);
                if (idle_secs > 0)
                {
                    //tmp = purple_str_seconds_to_string(time(NULL) - idle_secs);
                    //purple_notify_user_info_add_pair(user_info, tr("Idle"), tmp);

                    //g_free(tmp);
                    time_t t;
                    int ihrs, imin;
                    QString idle;

                    time(&t);

                    ihrs = (t - idle_secs) / 3600;
                    imin = ((t - idle_secs) / 60) % 60;

                    if (ihrs > 0)
                        idle = QString("(%1:%2)").arg(ihrs).arg(imin, 2);
                    else
                        idle = QString("(%1)").arg(imin);

                    if (text != "")
                        text += "  ";

                    text += idle;

                }
            }
        }
        setIcon(1, QIcon(quail_blist_item::getBuddyStatusIcon(node)));
        QString buddy_alias = getAlias(buddy);
        qDebug() << "QQuailBListItem::updateInfo.Buddy.5" << buddy_alias;
        setText(1, buddy_alias);
    }
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
    {
        qDebug() << "QQuailBListItem::updateInfo.Chat";
        PurpleChat *chat = (PurpleChat *)node;
        QString chatName = chat->alias;
        if (chatName.isEmpty())
            chatName = QString::fromStdString(purple_chat_get_name(chat));

        setIcon(1, QIcon(quail_protocol_utils::getProtocolIcon(chat->account)));
        setText(1, chatName);
    }
    else if (PURPLE_BLIST_NODE_IS_GROUP(node))
    {
        PurpleGroup *group = (PurpleGroup *)node;
        setText(1, group->name);
    }
    node->ui_data = this;
}

QString
quail_blist_item::getAlias(PurpleBuddy *buddy)
{
    if(!QString(buddy->alias).isEmpty())
        return QString(buddy->alias);
    else if(!QString(buddy->server_alias).isEmpty())
        return QString(buddy->server_alias);
    return QString(buddy->name);
}

void
quail_blist_item::init()
{
    updateInfo();
}

QSize
quail_blist_item::sizeHint ( int column ) const
{
    if (column == 0 || column == 2)
        return QSize(50, 50);
    else
        return QSize(100,50);
}

//void
//QQuailBListItem::paintBuddyInfo(QPainter *p, const QPalette &cg, int column,
//							   int width, int align, int lmarg, int itMarg)
//{
//    qDebug() << "QQuailBListItem::paintBuddyInfo";
//    PurpleBuddy *buddy;
//	PurpleContact *contact = NULL;

//    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
//		buddy = (PurpleBuddy *)node;
//	else
//	{
//		contact = (PurpleContact *)node;
//		buddy = purple_contact_get_priority_buddy(contact);
//	}

//    if (purple_prefs_get_bool("/quail/blist/show_large_icons"))
//	{
//		if (column > 0)
//			return;

//		if (!isExpanded())
//		{
//			if (dirty)
//			{
//				PurplePlugin *prpl = NULL;
//				PurplePluginProtocolInfo *prplInfo = NULL;
//				QRect topRect, bottomRect;
//				QString statusText;
//				QString idleTime;
//				QString warning;

//				prpl = purple_plugins_find_with_id(
//					purple_account_get_protocol_id(buddy->account));

//				if (prpl != NULL)
//                    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

//				if (prpl != NULL && prplInfo->status_text != NULL &&
//					purple_account_get_connection(buddy->account) != NULL)
//				{
//					char *tmp = prplInfo->status_text(buddy);

//					if (tmp != NULL)
//					{
//						g_strdelimit(tmp, "\n", ' ');

//						statusText = QString(tmp) + " ";

//						g_free(tmp);
//					}
//				}

//                if (!isExpanded() &&
//                    purple_prefs_get_bool("/quail/blist/show_idle_times"))
//				{
//					time_t t;
////					int ihrs, imin;

//					time(&t);
////					ihrs = (t - buddy->idle) / 3600;
////					imin = ((t - buddy->idle) / 60) % 60;

////					if (ihrs)
////					{
////                        idleTime = tr("Idle (%1h%2m) ").args( ihrs, imin);
////					}
////					else
////                        idleTime = tr("Idle (%1m) ").arg(imin);
//				}

////				if (!isExpanded() && buddy->evil > 0 &&
////                    purple_prefs_get_bool("/quail/blist/show_warning_levels"))
////				{
////                    warning = tr("Warned (%1%) ").arg(buddy->evil);
////				}

//                if (!PURPLE_BUDDY_IS_ONLINE(buddy) && statusText.isEmpty())
//					statusText = QObject::tr("Offline ");

//				if (contact != NULL && !isExpanded() && contact->alias != NULL)
//					topText = contact->alias;
//				else
//                    topText = purple_contact_get_alias(contact);

//				bottomText = statusText + idleTime + warning;

//				/* Get the top rect info. */
////				topRect = p->boundingRect(lmarg, 0, width - lmarg - itMarg,
////										  height(), align | AlignVCenter,
////										  topText);

////				if (!bottomText.isEmpty())
////				{
////					bottomRect = p->boundingRect(lmarg, 0,
////												 width - lmarg - itMarg,
////												 height(),
////												 align | AlignVCenter,
////												 bottomText);
////				}

////				textY1 = (height() - topRect.height() -
////						  bottomRect.height() - 2) / 2;
////				textY2 = textY1 + topRect.height() + 2;
//			}

////			if (buddy->idle > 0 &&
////                purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
////			{
////				p->setPen(cg.dark());
////			}

////			p->drawText(lmarg, textY1, width - lmarg - itMarg, height(),
////						align, topText);

////			p->setPen(cg.dark());
////			p->drawText(lmarg, textY2, width - lmarg - itMarg, height(),
////						align, bottomText);
//		}
//		else
//		{
//            QPalette _cg(cg);

////			if (buddy->idle > 0 &&
////                purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
////			{
////                _cg.setColor(QPalette::Text, cg.dark());
////			}

////            QTreeWidgetItem::paintCell(p, _cg, column, width, align);
//		}
//	}
//	else
//	{
//        QPalette _cg(cg);

////		if (buddy->idle > 0 &&
////            purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
////		{
////            _cg.setColor(QPalette::Text, cg.dark());
////		}

////        QTreeWidgetItem::paintCell(p, _cg, column, width, align);
//	}
//}

//void
//QQuailBListItem::paintGroupInfo(QPainter *p, const QPalette &, int column,
//							   int width, int align, int lmarg, int itMarg)
//{
//    qDebug() << "QQuailBListItem::paintGroupInfo";
//    if (column > 0)
//		return;

//	PurpleGroup *group = (PurpleGroup *)getBlistNode();
//	QString groupName, detail;
////	QFont f = p->font();

//	groupName = group->name;

//    if (purple_prefs_get_bool("/quail/blist/show_group_count"))
//	{
//		groupName += " ";
//		detail = QString("(%1/%2)").arg(
//			purple_blist_get_group_online_count(group)).arg(
//			purple_blist_get_group_size(group, FALSE));
//	}

////	f.setBold(true);
////	p->setFont(f);
////	p->drawText(lmarg, 0, width - lmarg - itMarg, height(),
////				align | AlignVCenter, groupName);

////	QRect r = p->boundingRect(lmarg, 0, width - lmarg - itMarg,
////							  height(),
////							  align | AlignVCenter, groupName);

////	f.setBold(false);
////	p->setFont(f);
////	p->drawText(lmarg + r.right(), 0, width - lmarg - itMarg, height(),
////				align | AlignVCenter, detail);
//}

QPixmap
quail_blist_item::getBuddyStatusIcon(PurpleBlistNode *node)
{
    QPixmap returnImage;
    quail_blist_item *qnode = (quail_blist_item*)node->ui_data;
    quail_blist_item *qbuddynode = NULL;
    PurpleBuddy *buddy = NULL;
    PurpleChat *chat = NULL;

    if(PURPLE_BLIST_NODE_IS_CONTACT(node)) {
        if(!qnode->isExpanded()) {
            buddy = purple_contact_get_priority_buddy((PurpleContact*)node);
            if (buddy != NULL)
                qbuddynode = (quail_blist_item*)((PurpleBlistNode*)buddy)->ui_data;
        }
    } else if(PURPLE_BLIST_NODE_IS_BUDDY(node)) {
        buddy = (PurpleBuddy*)node;
        qbuddynode = (quail_blist_item*)node->ui_data;
    } else if(PURPLE_BLIST_NODE_IS_CHAT(node)) {
        chat = (PurpleChat*)node;
    } else {
        return QPixmap();
    }

    if(buddy || chat) {
        PurpleAccount *account;
        PurplePlugin *prpl;

        if(buddy)
            account = buddy->account;
        else
            account = chat->account;

        prpl = purple_find_prpl(purple_account_get_protocol_id(account));
        if(!prpl) {
            return QPixmap();
        }
    }

    if(buddy) {
        PurplePresence *p;
        gboolean trans;

        p = purple_buddy_get_presence(buddy);
        trans = purple_presence_is_idle(p);

        if (PURPLE_BUDDY_IS_ONLINE(buddy) && qbuddynode && qbuddynode->recentSignedOnOff())
            returnImage = QPixmap(":/data/images/status/log-in.png");
        else if (qbuddynode && qbuddynode->recentSignedOnOff())
            returnImage = QPixmap(":/data/images/status/log-out.png");
        else if (purple_presence_is_status_primitive_active(p, PURPLE_STATUS_UNAVAILABLE))
            if (trans)
                returnImage = quail_image_utils::greyPixmap(
                            QPixmap(":/data/images/status/busy.png"),
                            "busy.png");
            else
                returnImage = QPixmap(":/data/images/status/busy.png");
        else if (purple_presence_is_status_primitive_active(p, PURPLE_STATUS_AWAY))
            if (trans)
                returnImage = quail_image_utils::greyPixmap(
                            QPixmap(":/data/images/status/away.png"),
                            "away.png");
            else
                returnImage = QPixmap(":/data/images/status/away.png");
        else if (purple_presence_is_status_primitive_active(p, PURPLE_STATUS_EXTENDED_AWAY))
            if (trans)
                returnImage = quail_image_utils::greyPixmap(
                            QPixmap(":/data/images/status/extended-away.png"),
                            "extended-away.png");
            else
                returnImage = QPixmap(":/data/images/status/extended-away.png");
        else if (purple_presence_is_status_primitive_active(p, PURPLE_STATUS_OFFLINE))
            returnImage = QPixmap(":/data/images/status/offline.png");
        else if (trans)
            returnImage = quail_image_utils::greyPixmap(
                        QPixmap(":/data/images/status/available.png"),
                        "available.png");
        else if (purple_presence_is_status_primitive_active(p, PURPLE_STATUS_INVISIBLE))
            returnImage = QPixmap(":/data/images/status/invisible.png");
        else
            returnImage = QPixmap(":/data/images/status/available.png");
    } else if (chat) {
        returnImage = QPixmap(":/data/images/status/chat.png");
    } else {
        returnImage = QPixmap(":/data/images/status/person.png");
    }

    return returnImage;
}

