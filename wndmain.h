#ifndef WNDMAIN_H
#define WNDMAIN_H

#include "playlistmonitor.h"

#include <QMediaPlayer>
#include <QMenu>
#include <QTime>
#include <QWidget>

namespace Ui {
    class WndMain;
}

class WndMain : public QWidget
{
    Q_OBJECT

public:
    explicit WndMain(QWidget *parent = 0);
    ~WndMain();

private slots:
    void resolveBugResolucaoIos();
    
private:
    void makeConnections();
    void createPlaybackModeMenu();

private:
    Ui::WndMain *ui;
    QMediaPlayer* m_player;
    QMediaPlaylist* m_playlist;
    PlaylistMonitor* m_playlistMonitor;
    QTimer m_userIdleTimer;
};

#endif // WNDMAIN_H
