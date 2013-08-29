#ifndef QUAILBLISTITEM_H
#define QUAILBLISTITEM_H

#include <QTreeWidgetItem>

#include <libpurple/blist.h>

class QQuailBListItem : public QTreeWidgetItem
{

    public:
        QQuailBListItem(QTreeWidget *parent, PurpleBlistNode *node);
        QQuailBListItem(QTreeWidgetItem *parent, PurpleBlistNode *node);
        ~QQuailBListItem();

        static QPixmap getBuddyStatusIcon(PurpleBlistNode *node);
//        static QPixmap getBuddyStatusIcon2(PurpleBlistNode *node);

        PurpleBlistNode *getBlistNode() const;

        void updateInfo();
        //TODO: Find out how to calculate this
        bool recentSignedOnOff()
        { return false; }
        /*reimp*/QSize sizeHint ( int column ) const;

    protected:
        void init();

//        void paintBuddyInfo(QPainter *p, const QPalette &cg, int column,
//							int width, int align, int lmarg, int itMarg);
//        void paintGroupInfo(QPainter *p, const QPalette &cg, int column,
//							int width, int align, int lmarg, int itMarg);

    private:
        PurpleBlistNode *node;
        bool dirty;
        QString topText, bottomText;
        int textY1, textY2;

        QString getAlias(PurpleBuddy *buddy);
};


#endif // QUAILBLISTITEM_H
