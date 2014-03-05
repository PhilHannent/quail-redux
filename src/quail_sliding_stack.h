#ifndef QUAIL_SLIDING_STACK_H
#define QUAIL_SLIDING_STACK_H

#include <QStackedWidget>
#include <QEasingCurve>

class quail_sliding_stack : public QStackedWidget
{
    Q_OBJECT
public:
    explicit quail_sliding_stack(QWidget *parent = 0);

    enum stack_direction {
        LEFT2RIGHT,
        RIGHT2LEFT,
        TOP2BOTTOM,
        BOTTOM2TOP,
        AUTOMATIC
    };

public slots:
    void set_speed(int speed);   /* animation duration in milliseconds */
    void set_animation(QEasingCurve::Type animation_type); //check out the QEasingCurve documentation for different styles
    void set_vertical_mode(bool vertical = true);
    void set_wrap(bool wrap);    //wrapping is related to slideInNext/Prev;it defines the behaviour when reaching last/first page

    /* The Animation / Page Change API */
    void slide_in_next();
    void slide_in_prev();
    void slide_in_idx(int idx, enum stack_direction direction=AUTOMATIC);

signals:
    /* this is used for internal purposes in the class engine */
    void animation_finished(void);

protected slots:
    /* this is used for internal purposes in the class engine */
    void animation_done_slot(void);

protected:
    /* this is used for internal purposes in the class engine */
    void slide_in_widget(QWidget * widget, enum stack_direction direction=AUTOMATIC);

    int m_speed;
    QEasingCurve::Type m_animation_type;
    bool m_vertical;
    int m_now;
    int m_next;
    bool m_wrap;
    QPoint m_pnow;
    bool m_active;
};

#endif // QUAIL_SLIDING_STACK_H
