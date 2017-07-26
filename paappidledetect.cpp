#include "paappidledetect.h"
#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>
#include <qmath.h>

PaAppIdleDetect* PaAppIdleDetect::s_this = NULL;

PaAppIdleDetect::PaAppIdleDetect() :
    QObject(qApp) {}

PaAppIdleDetect* PaAppIdleDetect::instance() {
    if (PaAppIdleDetect::s_this == NULL) {
        PaAppIdleDetect::s_this = new PaAppIdleDetect();
        qApp->installEventFilter(PaAppIdleDetect::s_this);
    }

    return PaAppIdleDetect::s_this;
}

void PaAppIdleDetect::addKeyboardIdleTimer(QTimer* a_timer)
{
    if (m_keyboardIdleTimers.contains(a_timer)) return;
    connect(a_timer, SIGNAL(destroyed(QObject*)), SLOT(keyboardTimerDestroyed(QObject*)));
    a_timer->start();
    m_keyboardIdleTimers += a_timer;
}

void PaAppIdleDetect::addMouseIdleTimer(QTimer *a_timer)
{
    if (m_mouseIdleTimers.contains(a_timer)) return;
    connect(a_timer, SIGNAL(destroyed(QObject*)), SLOT(mouseTimerDestroyed(QObject*)));
    a_timer->start();
    m_mouseIdleTimers += a_timer;
}

void PaAppIdleDetect::addUserIdleTimer(QTimer *a_timer)
{
    addKeyboardIdleTimer(a_timer);
    addMouseIdleTimer(a_timer);
}

void PaAppIdleDetect::rmvKeyboardIdleTimer(QTimer* a_timer)
{
    if (!m_keyboardIdleTimers.contains(a_timer)) return;
    disconnect(a_timer, SIGNAL(destroyed(QObject*)), this, SLOT(keyboardTimerDestroyed(QObject*)));
    a_timer->stop();
    m_keyboardIdleTimers.removeOne(a_timer);
}

void PaAppIdleDetect::rmvMouseIdleTimer(QTimer* a_timer)
{
    if (!m_mouseIdleTimers.contains(a_timer)) return;
    disconnect(a_timer, SIGNAL(destroyed(QObject*)), this, SLOT(mouseTimerDestroyed(QObject*)));
    a_timer->stop();
    m_mouseIdleTimers.removeOne(a_timer);
}

void PaAppIdleDetect::rmvUserIdleTimer(QTimer* a_timer)
{
    rmvKeyboardIdleTimer(a_timer);
    rmvMouseIdleTimer(a_timer);
}

void PaAppIdleDetect::startKeyboardIdleTimers()
{
    emit keyboardInteration();
    m_keyboardIdleTimers.startAll();
    m_keyboardIdleTime.start();
}

void PaAppIdleDetect::startMouseIdleTimers()
{
    emit mouseInteration();
    m_mouseIdleTimers.startAll();
    m_mouseIdleTime.start();
}

void PaAppIdleDetect::startUserIdleTimers()
{
    emit userInteration();
    startKeyboardIdleTimers();
    startMouseIdleTimers();
}

void PaAppIdleDetect::keyboardTimerDestroyed(QObject *a_obj)
{
    QTimer* l_timer = static_cast<QTimer*>(a_obj);
    m_keyboardIdleTimers.removeOne(l_timer);
}

void PaAppIdleDetect::mouseTimerDestroyed(QObject *a_obj)
{
    QTimer* l_timer = static_cast<QTimer*>(a_obj);
    m_mouseIdleTimers.removeOne(l_timer);
}

double qPointDistance(QPoint p, QPoint q)
{
    double dx   = p.x() - q.x();         //horizontal difference
    double dy   = p.y() - q.y();         //vertical difference
    double dist = qSqrt(dx * dx + dy * dy); //distance using Pythagoras theorem
    return dist;
}

bool PaAppIdleDetect::eventFilter(QObject *obj, QEvent *event)
{
    if (event->spontaneous()) {
        bool l_keyEvent = false;
        bool l_mouseEvent = false;

        if ((event->type() == QEvent::KeyPress)
        ||  (event->type() == QEvent::KeyRelease)) {
            l_keyEvent = true;
        } else
        if ((event->type() == QEvent::MouseButtonPress)
        ||  (event->type() == QEvent::MouseButtonRelease)) {
            l_mouseEvent = true;
            m_lastMousePos = static_cast<QMouseEvent*>(event)->globalPos();
        } else
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* l_mEvent = static_cast<QMouseEvent*>(event);

            if (m_lastMousePos != l_mEvent->globalPos()) {
                l_mouseEvent = true;
                m_lastMousePos = l_mEvent->globalPos();
            }
        }

        if (l_keyEvent) {
            startKeyboardIdleTimers();
        }

        if (l_mouseEvent) {
            startMouseIdleTimers();
        }

        if (l_keyEvent || l_mouseEvent)
            emit userInteration();
    }

    return QObject::eventFilter(obj, event);
}

const QTime& PaAppIdleDetect::keyboardIdleTime() const
{
    return m_keyboardIdleTime;
}

const QTime& PaAppIdleDetect::mouseIdleTime() const
{
    return m_mouseIdleTime;
}

const QTime& PaAppIdleDetect::userIdleTime() const
{
    return qMax(m_mouseIdleTime, m_keyboardIdleTime);
}
