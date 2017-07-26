#include "wndmain.h"
#include "ui_wndmain.h"

#include "paappidledetect.h"

#include <QDesktopWidget>
#include <QTime>
#include <QStandardPaths>
#include <QScroller>
#include <QEasingCurve>
#include <QtAwesome.h>
#include <QResizeEvent>
#include <QProxyStyle>

void configScroll(QAbstractScrollArea* scrollArea)
{
    QScroller *scroller = QScroller::scroller(scrollArea);

    QScrollerProperties prop = scroller->scrollerProperties();

    prop.setScrollMetric(QScrollerProperties::AxisLockThreshold, 0.66);
    prop.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve(QEasingCurve::OutExpo));
    prop.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.05);
    prop.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.635);
    prop.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.33);
    prop.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.33);
    prop.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.93);
    prop.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);

    scroller->setScrollerProperties(prop);

    QAbstractItemView* l_itemView = qobject_cast<QAbstractItemView*>(scrollArea);
    if (l_itemView)
        l_itemView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    scroller->grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

WndMain::WndMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WndMain), m_player(new QMediaPlayer(this)), m_playlist(new QMediaPlaylist(this))
{
    m_playlistMonitor = new PlaylistMonitor(m_playlist);
    ui->setupUi(this);
    m_player->setVideoOutput(ui->wdgVideo);
    m_player->setPlaylist(m_playlist);
    ui->lstFiles->setModel(m_playlistMonitor);
    ui->lstFiles->setAlternatingRowColors(true);

    configScroll(ui->lstFiles);
#ifdef Q_OS_IOS
    m_playlistMonitor->addDirs(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
#else
    m_playlistMonitor->addDirs(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation));
#endif

    qtAwesome->setDefaultOption("color", QColor(200,200,200));
    qtAwesome->setDefaultOption("color-disabled", QColor(150,150,150,150));
    qtAwesome->setDefaultOption("color-active", QColor(255,255,255));
    qtAwesome->setDefaultOption("color-selected", QColor(255,255,255));

    ui->btnBackward->setText(QString( QChar(static_cast<int>(fa::backward)) ));
    ui->btnBackward->setFont(qtAwesome->font(32));

    ui->btnPlay->setText(QString( QChar(static_cast<int>(fa::play)) ));
    ui->btnPlay->setFont(qtAwesome->font(32));

    ui->btnFoward->setText(QString( QChar(static_cast<int>(fa::forward)) ));
    ui->btnFoward->setFont(qtAwesome->font(32));

    ui->btnStop->setText(QString( QChar(static_cast<int>(fa::stop)) ));
    ui->btnStop->setFont(qtAwesome->font(32));

    ui->btnViewList->setText(QString( QChar(static_cast<int>(fa::bars)) ));
    ui->btnViewList->setFont(qtAwesome->font(32));

    ui->btnPlaybackMode->setText(QString( QChar(static_cast<int>(fa::listol)) ));
    ui->btnPlaybackMode->setFont(qtAwesome->font(32));

    ui->stackedWidget->setCurrentWidget(ui->pagePlayList);

    makeConnections();
    createPlaybackModeMenu();

    m_userIdleTimer.setInterval(3000);
    m_userIdleTimer.setSingleShot(true);

    connect(iPaAppIdleDetect, &PaAppIdleDetect::userInteration, ui->wdgBotoes, &QWidget::show);
    connect(&m_userIdleTimer, &QTimer::timeout, ui->wdgBotoes, &QWidget::hide);
}

void WndMain::resolveBugResolucaoIos()
{
    // para resolver bug no qt que quando app é colocado para background e volta o video ficava erradoqDebug() << "app is restored";
    ui->wdgVideo->setMaximumSize(0, 0);
    QTimer::singleShot(100, this, [=]() {
        ui->wdgVideo->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    });
}

void WndMain::makeConnections()
{
#ifdef Q_OS_IOS
    connect(qApp, &QApplication::applicationStateChanged, this, [=](Qt::ApplicationState state) {
        qDebug() << "applicationStateChanged" << state;
        if (state == Qt::ApplicationActive) {
            QTimer::singleShot(100, this, &WndMain::resolveBugResolucaoIos);
        }
    });
#endif

    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, [=](int i) {
        qDebug() << "QMediaPlaylist::currentIndexChanged";
#ifdef Q_OS_IOS
        QTimer::singleShot(100, this, &WndMain::resolveBugResolucaoIos);
#endif
        ui->lstFiles->selectionModel()->select(ui->lstFiles->model()->index(i, 0),
                                               QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    });

    connect(ui->lstFiles, &QListView::doubleClicked, this, [=](const QModelIndex& i) {
        m_player->stop();
        m_playlist->setCurrentIndex(i.row());
        m_player->play();
    });

    connect(m_player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state) {
        if (state == QMediaPlayer::PlayingState) {
            iPaAppIdleDetect->addUserIdleTimer(&m_userIdleTimer);
            ui->stackedWidget->setCurrentWidget(ui->pageVideo);
            ui->btnPlay->setText(QString( QChar(static_cast<int>(fa::pause)) ));
        } else {
            iPaAppIdleDetect->rmvUserIdleTimer(&m_userIdleTimer);
            ui->btnPlay->setText(QString( QChar(static_cast<int>(fa::play)) ));
            if (state == QMediaPlayer::StoppedState) {
                ui->stackedWidget->setCurrentWidget(ui->pagePlayList);
                ui->sldProgress->setEnabled(false);
            }
        }
    });

    connect(ui->btnBackward, &QToolButton::clicked, m_playlist, &QMediaPlaylist::previous);

    connect(ui->btnPlay, &QToolButton::clicked, this, [=]() {
        if (m_player->state() == QMediaPlayer::PlayingState) {
            m_player->pause();
        } else {
            if (m_player->state() == QMediaPlayer::StoppedState)
                m_playlist->setCurrentIndex(qMax(1, ui->lstFiles->currentIndex().row()));
            m_player->play();
        }
    });

    connect(ui->btnFoward, &QToolButton::clicked, m_playlist, &QMediaPlaylist::next);

    connect(ui->btnStop, &QToolButton::clicked, this, [=]() {
        m_player->stop();
    });

    connect(ui->btnViewList, &QToolButton::clicked, this, [=]() {
        if (ui->stackedWidget->currentWidget() == ui->pageVideo)
            ui->stackedWidget->setCurrentWidget(ui->pagePlayList);
        else
            ui->stackedWidget->setCurrentWidget(ui->pageVideo);
    });

    connect(m_player, &QMediaPlayer::seekableChanged, ui->sldProgress, &QSlider::setEnabled);

    connect(m_player, &QMediaPlayer::durationChanged, ui->sldProgress, [=](qint64 a_position) {
        ui->sldProgress->setMaximum(int(a_position / 1000));
    });

    connect(m_player, &QMediaPlayer::positionChanged, ui->sldProgress, [=](qint64 a_position) {
        QTime l_timeElapsed = QTime(0, 0, 0).addSecs(int(a_position / 1000));
        QString l_text = l_timeElapsed.toString("hh:mm:ss");
        if (l_text.startsWith("00:")) l_text = l_text.mid(3);
        ui->lblElapsed->setText(" " + l_text);

        ui->sldProgress->blockSignals(true);
        ui->sldProgress->setValue(int(a_position / 1000));
        ui->sldProgress->blockSignals(false);
    });

    connect(ui->sldProgress, &QSlider::valueChanged, m_player, [=](int a_value) {
        m_player->setPosition(qint64(a_value) * 1000);
    });

    connect(m_playlist, &QMediaPlaylist::playbackModeChanged, this, [=](QMediaPlaylist::PlaybackMode a_playbackMode) {
        switch (a_playbackMode) {
            case QMediaPlaylist::CurrentItemOnce:
                ui->btnPlaybackMode->setText(QString( QChar(static_cast<int>(fa::handstopo)) ));
                break;
            case QMediaPlaylist::CurrentItemInLoop:
                ui->btnPlaybackMode->setText(QString( QChar(static_cast<int>(fa::handpointero)) ));
                break;
            case QMediaPlaylist::Sequential:
                ui->btnPlaybackMode->setText(QString( QChar(static_cast<int>(fa::listol)) ));
                break;
            case QMediaPlaylist::Loop:
                ui->btnPlaybackMode->setText(QString( QChar(static_cast<int>(fa::repeat)) ));
                break;
            case QMediaPlaylist::Random:
                ui->btnPlaybackMode->setText(QString( QChar(static_cast<int>(fa::random)) ));
                break;
        }
    });

    m_playlist->setPlaybackMode(QMediaPlaylist::Random);
}

void WndMain::createPlaybackModeMenu()
{
    static QMenu* sl_menu = NULL;
    if (!sl_menu) {
        QAction* l_action = NULL;

        sl_menu = new QMenu(tr("Playback Mode"));
        ui->btnPlaybackMode->setMenu(sl_menu);
        l_action = sl_menu->addAction(qtAwesome->icon(fa::handstopo), tr("Current Item Once"));
        connect(l_action, &QAction::triggered, this, [=]() {
            m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
        });
        l_action = sl_menu->addAction(qtAwesome->icon(fa::handpointero), tr("Current Item In Loop"));
        connect(l_action, &QAction::triggered, this, [=]() {
            m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        });
        l_action = sl_menu->addAction(qtAwesome->icon(fa::listol), tr("Sequential"));
        connect(l_action, &QAction::triggered, this, [=]() {
            m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
        });
        l_action = sl_menu->addAction(qtAwesome->icon(fa::repeat), tr("Loop"));
        connect(l_action, &QAction::triggered, this, [=]() {
            m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
        });
        l_action = sl_menu->addAction(qtAwesome->icon(fa::random), tr("Random"));
        connect(l_action, &QAction::triggered, this, [=]() {
            m_playlist->setPlaybackMode(QMediaPlaylist::Random);
        });

        connect(ui->btnPlaybackMode, &QToolButton::clicked, this, [=]() {
            sl_menu->exec(ui->btnPlaybackMode->mapToGlobal(QPoint(0,0)));
        });
    }
}

WndMain::~WndMain()
{
    delete ui;
}
