#ifndef PAAPPIDLEDETECT_H
#define PAAPPIDLEDETECT_H

#include <QObject>
#include <QTimer>
#include <QPoint>
#include <QTime>

class QTimerPtrList : public QList<QTimer*> {
public:
    void startAll() {
        foreach (QTimer* l_t, *this) {
            l_t->start();
        }
    }

    void stopAll() {
        foreach (QTimer* l_t, *this) {
            l_t->stop();
        }
    }
};

class PaAppIdleDetect : public QObject
{
    Q_OBJECT

private:
    explicit PaAppIdleDetect();

public:
    static PaAppIdleDetect* instance();

    void addKeyboardIdleTimer(QTimer* a_timer);
    void addMouseIdleTimer(QTimer* a_timer);
    void addUserIdleTimer(QTimer* a_timer);

    void rmvKeyboardIdleTimer(QTimer* a_timer);
    void rmvMouseIdleTimer(QTimer* a_timer);
    void rmvUserIdleTimer(QTimer* a_timer);

    const QTime &keyboardIdleTime() const;

    const QTime &mouseIdleTime() const;

    const QTime &userIdleTime() const;

public slots:
    void startKeyboardIdleTimers();
    void startMouseIdleTimers();
    void startUserIdleTimers();

protected slots:
    void keyboardTimerDestroyed(QObject* a_obj);
    void mouseTimerDestroyed(QObject* a_obj);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void keyboardInteration();
    void mouseInteration();
    void userInteration();

private:
    static PaAppIdleDetect* s_this;

    QTimerPtrList m_keyboardIdleTimers;
    QTimerPtrList m_mouseIdleTimers;

    QTime m_keyboardIdleTime;
    QTime m_mouseIdleTime;

    QPoint m_lastMousePos;
};

#define iPaAppIdleDetect PaAppIdleDetect::instance()

#endif // PAAPPIDLEDETECT_H
