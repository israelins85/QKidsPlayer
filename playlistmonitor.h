#ifndef PLAYLISTMONITOR_H
#define PLAYLISTMONITOR_H

#include <QDir>
#include <QMediaPlaylist>
#include <QAbstractListModel>
#include <QTimer>

class PlaylistMonitor : public QAbstractListModel {
    Q_OBJECT
public:
    explicit PlaylistMonitor(QMediaPlaylist *parent = 0);

    void addDir(QString l_dir, bool a_refreshNow = true);
    void addDirs(QStringList l_dirs, bool a_refreshNow = true);

signals:

public slots:
    void refresh();

private:
    QMediaPlaylist* m_playlist;
    QStringList m_dirsToScan;
    QStringList m_files;
    QStringList m_filesCompleteName;
    QTimer m_timeToRefresh;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
};

#endif // PLAYLISTMONITOR_H
