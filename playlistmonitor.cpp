#include "playlistmonitor.h"

#include <QDirIterator>
#include <QDebug>

PlaylistMonitor::PlaylistMonitor(QMediaPlaylist* parent) : QAbstractListModel(parent), m_playlist(parent)
{
    m_timeToRefresh.setInterval(30000);
    connect(&m_timeToRefresh, &QTimer::timeout, this, &PlaylistMonitor::refresh);
    m_timeToRefresh.start();
}

void PlaylistMonitor::addDir(QString l_dir, bool a_refreshNow)
{
    addDirs(QStringList() << l_dir, a_refreshNow);
}

void PlaylistMonitor::addDirs(QStringList l_dirs, bool a_refreshNow)
{
    m_dirsToScan += l_dirs;
    if (a_refreshNow) refresh();
}

void PlaylistMonitor::refresh()
{
    qDebug() << "PlaylistMonitor::refresh()";
    QStringList l_filesRemoved = m_files;
    QStringList l_filesCompleteNameRemoved = m_filesCompleteName;

    foreach (QString l_d, m_dirsToScan) {
        QDirIterator it(l_d,
                        (QStringList() << "*.avi" << "*.AVI" << "*.wmv"<< "*.WMV"
                        << "*.flv"<< "*.FLV"<< "*.mpg"<< "*.MPG"<< "*.mp4"<< "*.MP4"),
                        QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();

            QString l_fileCompleteName = it.fileInfo().absoluteFilePath();
            QString l_file = l_fileCompleteName.mid(l_d.size() + 1);

            // insertion sort
            int i = 0;
            for (; i < m_filesCompleteName.size(); ++i) {
                const QString& l_f = m_filesCompleteName[i];
                if (l_f == l_fileCompleteName) {
                    qint32 l_idx = l_filesCompleteNameRemoved.indexOf(l_fileCompleteName);
                    l_filesRemoved.removeAt(l_idx);
                    l_filesCompleteNameRemoved.removeAt(l_idx);
                    i = -1;
                    break;
                }
                if (l_f > l_fileCompleteName) break;
            }

            if (i == -1) continue;

            qDebug() << "inserting" << l_file;
            emit beginInsertRows(QModelIndex(), i, i);
            m_files.insert(i, l_file);
            m_filesCompleteName.insert(i, l_fileCompleteName);
            m_playlist->insertMedia(i, QUrl::fromLocalFile(l_fileCompleteName));
            emit endInsertRows();
        }
    }

    foreach (QString l_fileRemoved, l_filesCompleteNameRemoved) {
        qint32 l_idx = m_filesCompleteName.indexOf(l_fileRemoved);
        qDebug() << "removing" << m_files[l_idx];
        emit beginRemoveRows(QModelIndex(), l_idx, l_idx);
        m_files.removeAt(l_idx);
        m_filesCompleteName.removeAt(l_idx);
        m_playlist->removeMedia(l_idx);
        emit endRemoveRows();
    }
}

int PlaylistMonitor::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_files.size();
}

QVariant PlaylistMonitor::data(const QModelIndex& index, int role) const
{
    QVariant l_ret;

    if (role == Qt::DisplayRole)
        l_ret = m_files[index.row()];

    return l_ret;
}

