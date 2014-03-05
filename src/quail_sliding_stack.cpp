#include "quail_sliding_stack.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

quail_sliding_stack::quail_sliding_stack(QWidget *parent) :
    QStackedWidget(parent)
  , m_speed(500)
  , m_animation_type(QEasingCurve::OutBack)
  , m_vertical(false)
  , m_now(0)
  , m_next(0)
  , m_wrap(false)
  , m_pnow(0,0)
  , m_active(false)
{
}

void quail_sliding_stack::set_vertical_mode(bool vertical) {
    m_vertical = vertical;
}

void quail_sliding_stack::set_speed(int speed) {
    m_speed = speed;
}

void quail_sliding_stack::set_animation(enum QEasingCurve::Type animation_type) {
    m_animation_type = animation_type;
}

void quail_sliding_stack::set_wrap(bool wrap) {
    m_wrap=wrap;
}

void quail_sliding_stack::slide_in_next() {
    int now = currentIndex();
    if (m_wrap || (now < this->count()-1))
        slide_in_idx(now + 1);
}


void quail_sliding_stack::slide_in_prev() {
    int now = currentIndex();
    if (m_wrap || (now > 0))
        slide_in_idx(now - 1);
}

void quail_sliding_stack::slide_in_idx(int idx, enum stack_direction direction) {
    if (idx > this->count()-1) {
        direction=m_vertical ? TOP2BOTTOM : RIGHT2LEFT;
        idx=(idx)%this->count();
    }
    else if (idx < 0) {
        direction= m_vertical ? BOTTOM2TOP: LEFT2RIGHT;
        idx = (idx + this->count())%this->count();
    }
    slide_in_widget(this->widget( idx ), direction);
}

void quail_sliding_stack::slide_in_widget(QWidget * new_widget, enum stack_direction  direction) {

    if (m_active) {
        return;
        /* at the moment, do not allow re-entrance before an animation is completed.
        // other possibility may be to finish the previous animation abrupt, or
        // to revert the previous animation with a counter animation, before going ahead
        // or to revert the previous animation abrupt
        // and all those only, if the newwidget is not the same as that of the previous running animation. */
    }
    else
        m_active = true;

    enum stack_direction direction_hint;
    int now = this->currentIndex();
    int next = indexOf(new_widget);
    if (now == next) {
        m_active = false;
        return;
    }
    else if (now<next){
        direction_hint = m_vertical ? TOP2BOTTOM : RIGHT2LEFT;
    }
    else {
        direction_hint = m_vertical ? BOTTOM2TOP : LEFT2RIGHT;
    }
    if (direction == AUTOMATIC) {
        direction = direction_hint;
    }
    /* calculate the shifts */
    int offsetx = frameRect().width();
    int offsety = frameRect().height();

    /*the following is important, to ensure that the new widget
      has correct geometry information when sliding in first time */
    this->widget(next)->setGeometry ( 0,  0, offsetx, offsety );

    if (direction == BOTTOM2TOP)  {
            offsetx = 0;
            offsety =-offsety;
    }
    else if (direction == TOP2BOTTOM) {
            offsetx = 0;
            //offsety = offsety;
    }
    else if (direction == RIGHT2LEFT) {
            offsetx =-offsetx;
            offsety = 0;
    }
    else if (direction == LEFT2RIGHT) {
            //offsetx = offsetx;
            offsety = 0;
    }
    /* re-position the next widget outside/aside of the display area */
    QPoint pnext = widget(next)->pos();
    QPoint pnow = widget(now)->pos();
    m_pnow = pnow;

    widget(next)->move(pnext.x()-offsetx,pnext.y()-offsety);
    /*make it visible/show */
    widget(next)->show();
    widget(next)->raise();

    /* animate both, the now and next widget to the side, using animation
     * framework */
    QPropertyAnimation *animnow = new QPropertyAnimation(widget(now), "pos");

    animnow->setDuration(m_speed);
    animnow->setEasingCurve(m_animation_type);
    animnow->setStartValue(QPoint(pnow.x(), pnow.y()));
    animnow->setEndValue(QPoint(offsetx+pnow.x(), offsety+pnow.y()));
    QPropertyAnimation *animnext = new QPropertyAnimation(widget(next), "pos");
    animnext->setDuration(m_speed);
    animnext->setEasingCurve(m_animation_type);
    animnext->setStartValue(QPoint(-offsetx+pnext.x(), offsety+pnext.y()));
    animnext->setEndValue(QPoint(pnext.x(), pnext.y()));

    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup;

    animgroup->addAnimation(animnow);
    animgroup->addAnimation(animnext);

    QObject::connect(animgroup, SIGNAL(finished()),this,SLOT(animation_done_slot()));
    m_next = next;
    m_now = now;
    m_active = true;
    animgroup->start();

    //note; the rest is done via a connect from the animation ready;
    //animation->finished() provides a signal when animation is done;
    //so we connect this to some post processing slot,
    //that we implement here below in animationDoneSlot.
}


void quail_sliding_stack::animation_done_slot(void) {
    //when ready, call the QStackedWidget slot setCurrentIndex(int)
    setCurrentIndex(m_next);  //this function is inherit from QStackedWidget
    //then hide the outshifted widget now, and  (may be done already implicitely by QStackedWidget)
    widget(m_now)->hide();
    //then set the position of the outshifted widget now back to its original
    widget(m_now)->move(m_pnow);
    //so that the application could also still call the QStackedWidget original functions/slots for changings
    //widget(m_now)->update();
    //setCurrentIndex(m_next);  //this function is inherit from QStackedWidget
    m_active = false;
    emit animation_finished();
}
