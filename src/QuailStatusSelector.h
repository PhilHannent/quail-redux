#ifndef QUAILSTATUSSELECTOR_H
#define QUAILSTATUSSELECTOR_H

#include <QWidget>

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

private:
    QComboBox *statusSelector;
    QLabel *buddyIcon;

};

#endif // QUAILSTATUSSELECTOR_H
