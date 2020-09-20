#include "playerwindow.h"
#include "ui_playerwindow.h"
#include "trackdurationdelegate.h"
#include "taglibfilerefwrapper.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlRecord>
#include <QTime>


PlayerWindow::PlayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerWindow)
{
    ui->setupUi(this);
    restoreWindowState();
    initializeMediaPlayer();
    initializeLibraryModels();
    initializeLibraryView();
    setUpConnections();
}


PlayerWindow::~PlayerWindow()
{
    delete ui;
}


void PlayerWindow::restoreWindowState()
{
    move(session.value("PlayerWindow/position", pos()).toPoint());
    if (session.value("PlayerWindow/isMaximized", true).toBool())
    {
        setWindowState(Qt::WindowState::WindowMaximized);
    }
    else
    {
        resize(session.value("PlayerWindow/size", size()).toSize());
    }
}


void PlayerWindow::initializeMediaPlayer()
{
    mediaPlayer = new QMediaPlayer(this);
    restoreMediaPlayerVolume();
}


void PlayerWindow::restoreMediaPlayerVolume()
{
    const int DEFAULT_VOLUME = 100; // This should match DEFAULT_VOLUME in PlayerControls::restoreVolumeSliderState()
    int volume = session.value("MediaPlayer/volume", DEFAULT_VOLUME).toInt();
    mediaPlayer->setVolume(volume);
}


void PlayerWindow::initializeLibraryModels()
{
    librarySourceModel = new QSqlTableModel(this);
    librarySourceModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    librarySourceModel->setTable("Track"); // Track table from media database set up in main.cpp

    const int TRACK_ID_COLUMN = 0;
    const int TITLE_COLUMN = 1;
    const int ARTIST_COLUMN = 2;
    const int ALBUM_COLUMN = 3;
    const int TRACK_NUM_COLUMN = 4;
    const int YEAR_COLUMN = 5;
    const int GENRE_COLUMN = 6;
    const int DURATION_COLUMN = 7;
    const int LOCATION_COLUMN = 8;
    librarySourceModel->setHeaderData(TRACK_ID_COLUMN, Qt::Horizontal, tr("Track ID"));
    librarySourceModel->setHeaderData(TITLE_COLUMN, Qt::Horizontal, tr("Title"));
    librarySourceModel->setHeaderData(ARTIST_COLUMN, Qt::Horizontal, tr("Artist"));
    librarySourceModel->setHeaderData(ALBUM_COLUMN, Qt::Horizontal, tr("Album"));
    librarySourceModel->setHeaderData(TRACK_NUM_COLUMN, Qt::Horizontal, tr("Track Number"));
    librarySourceModel->setHeaderData(YEAR_COLUMN, Qt::Horizontal, tr("Year"));
    librarySourceModel->setHeaderData(GENRE_COLUMN, Qt::Horizontal, tr("Genre"));
    librarySourceModel->setHeaderData(DURATION_COLUMN, Qt::Horizontal, tr("Duration"));
    librarySourceModel->setHeaderData(LOCATION_COLUMN, Qt::Horizontal, tr("Location"));
    librarySourceModel->select();

    libraryProxyModel = new LibraryProxyModel(this);
    libraryProxyModel->setSourceModel(librarySourceModel);
    libraryProxyModel->setTrackIdColumn(TRACK_ID_COLUMN);
    libraryProxyModel->setTitleColumn(TITLE_COLUMN);
    libraryProxyModel->setArtistColumn(ARTIST_COLUMN);
    libraryProxyModel->setAlbumColumn(ALBUM_COLUMN);
    libraryProxyModel->setTrackNumColumn(TRACK_NUM_COLUMN);
    libraryProxyModel->setYearColumn(YEAR_COLUMN);
    libraryProxyModel->setGenreColumn(GENRE_COLUMN);
    libraryProxyModel->setDurationColumn(DURATION_COLUMN);
    libraryProxyModel->setLocationColumn(LOCATION_COLUMN);
    libraryProxyModel->setDynamicSortFilter(true);
}


void PlayerWindow::initializeLibraryView()
{
    ui->libraryView->setModel(libraryProxyModel);
    //ui->libraryView->hideColumn(libraryProxyModel->getTrackIdColumn());

    ui->libraryView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->libraryView->setItemDelegateForColumn(libraryProxyModel->getDurationColumn(), new TrackDurationDelegate(ui->libraryView));

    ui->libraryView->setCurrentIndex(QModelIndex());

    restoreLibraryViewState();
    /*
    for (int i = 0; i < ui->libraryView->model()->columnCount(); i++)
    {
        qDebug() << ui->libraryView->model()->headerData(i, Qt::Horizontal).toString();
    }
    */
}


void PlayerWindow::restoreLibraryViewState()
{
    // Restore the column widths
    const int DEFAULT_COLUMN_WIDTH = ui->libraryView->header()->defaultSectionSize();
    const int trackIdColumnWidth = session.value("LibraryView/trackIdColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int titleColumnWidth = session.value("LibraryView/titleColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int artistColumnWidth = session.value("LibraryView/artistColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int albumColumnWidth = session.value("LibraryView/albumColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int trackNumColumnWidth = session.value("LibraryView/trackNumColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int yearColumnWidth = session.value("LibraryView/yearColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int genreColumnWidth = session.value("LibraryView/genreColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int durationColumnWidth = session.value("LibraryView/durationColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int locationColumnWidth = session.value("LibraryView/locationColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    ui->libraryView->setColumnWidth(libraryProxyModel->getTrackIdColumn(), trackIdColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getTitleColumn(), titleColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getArtistColumn(), artistColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getAlbumColumn(), albumColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getTrackNumColumn(), trackNumColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getYearColumn(), yearColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getGenreColumn(), genreColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getDurationColumn(), durationColumnWidth);
    ui->libraryView->setColumnWidth(libraryProxyModel->getLocationColumn(), locationColumnWidth);

    // Restore which column was last sorted
    Qt::SortOrder sortOrder;
    if (session.value("LibraryView/sortOrder").toInt() == 0)
    {
        sortOrder = Qt::SortOrder::AscendingOrder;
    }
    else
    {
        sortOrder = Qt::SortOrder::DescendingOrder;
    }
    const int sortColumn = session.value("LibraryView/sortByColumn").toInt();
    ui->libraryView->sortByColumn(sortColumn, sortOrder);
}


void PlayerWindow::setUpConnections()
{
    connect(ui->actionAddToLibrary, &QAction::triggered, this, &PlayerWindow::onAddToLibraryActionTriggered);

    //connect(ui->viewTrackId, &QAction::triggered, this, &PlayerWindow::viewTrackIdTriggered);
    connect(ui->menuView, &QMenu::triggered, this, &PlayerWindow::menuViewActionTriggered);

    connect(ui->actionAddToLibrary, &QAction::triggered, this, &PlayerWindow::onAddToLibraryActionTriggered);

    connect(ui->libraryView, &QTreeView::activated, this, &PlayerWindow::setMediaForPlayback);
    connect(ui->libraryView, &QTreeView::customContextMenuRequested, this, &PlayerWindow::customContextMenu);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onMediaPlayerStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updateLibraryViewSelection);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::updatePlaybackState);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &PlayerWindow::updateTotalTime);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &PlayerWindow::updateTimePassed);

    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &PlayerWindow::onPlayOrPauseSignal);
    connect(ui->controls, &PlayerControls::nextClicked, this, &PlayerWindow::setNextMediaForPlayback);
    connect(ui->controls, &PlayerControls::prevClicked, this, &PlayerWindow::setPreviousMediaForPlayback);
}


void PlayerWindow::menuViewActionTriggered(QAction *action)
{
    int column;
    if (action == ui->viewTrackId)
    {
        column = libraryProxyModel->getTrackIdColumn();
    }
    else if (action == ui->viewTitle)
    {
        column = libraryProxyModel->getTitleColumn();
    }
    else if (action == ui->viewArtist)
    {
        column = libraryProxyModel->getArtistColumn();
    }
    else if (action == ui->viewAlbum)
    {
        column = libraryProxyModel->getAlbumColumn();
    }
    else if (action == ui->viewTrackNum)
    {
        column = libraryProxyModel->getTrackNumColumn();
    }
    else if (action == ui->viewYear)
    {
        column = libraryProxyModel->getYearColumn();
    }
    else if (action == ui->viewGenre)
    {
        column = libraryProxyModel->getGenreColumn();
    }
    else if (action == ui->viewDuration)
    {
        column = libraryProxyModel->getDurationColumn();
    }
    else // action == ui->viewLocation
    {
        column = libraryProxyModel->getLocationColumn();
    }

    if (action->isChecked())
    {
        ui->libraryView->showColumn(column);
    }
    else
    {
        ui->libraryView->hideColumn(column);
    }
}


void PlayerWindow::onAddToLibraryActionTriggered()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Add file(s) to library");
    for (const QString &filePath : filePaths)
    {
        TagLibFileRefWrapper tagLibFileRefWrapper(filePath);
        if (!tagLibFileRefWrapper.fileRefIsNull())
        {
            QString title = tagLibFileRefWrapper.getTitle();
            QString artist = tagLibFileRefWrapper.getArtist();
            QString album = tagLibFileRefWrapper.getAlbum();
            unsigned int trackNum = tagLibFileRefWrapper.getTrackNum();
            unsigned int year = tagLibFileRefWrapper.getYear();
            QString genre = tagLibFileRefWrapper.getGenre();
            int duration = tagLibFileRefWrapper.getDurationInMilliseconds();

            insertToTrackTable(title, artist, album, trackNum, year, genre, duration, filePath);
        }
    }
}


void PlayerWindow::insertToTrackTable(const QString &title,
                                      const QString &artist,
                                      const QString &album,
                                      unsigned int trackNum,
                                      unsigned int year,
                                      const QString &genre,
                                      int duration,
                                      const QString &location)
{
    QSqlRecord trackRecord = librarySourceModel->record();
    trackRecord.setGenerated("id", false);
    trackRecord.setValue("title", title);
    trackRecord.setValue("artist", artist);
    trackRecord.setValue("album", album);
    (trackNum > 0) ? trackRecord.setValue("track_num", trackNum) : trackRecord.setNull("track_num");
    (year > 0) ? trackRecord.setValue("year", year) : trackRecord.setNull("year");
    trackRecord.setValue("genre", genre);
    trackRecord.setValue("duration", duration);
    trackRecord.setValue("location", location);

    librarySourceModel->insertRecord(librarySourceModel->rowCount(), trackRecord);
}


void PlayerWindow::deleteFromLibrary()
{
    QModelIndex indexOfFileToDelete = libraryProxyModel->mapToSource(ui->libraryView->currentIndex());
    librarySourceModel->removeRow(indexOfFileToDelete.row());
    librarySourceModel->select();

    /* When deleting a file, if there's a media loaded in the mediaPlayer there are two cases in which you will
     * need to adjust its row index (srcIndexOfCurrMedia.row()) in order to keep it pointing to the correct media.
     *
     * Case I: Deleting the currently playing song. In this case you will need to stop the song and invalidate srcIndexOfCurrMedia.
     *
     * Case II: You are deleting a song that is listed before the current media inside the librarySourceModel:
     * E.g. if you have songs A, B, C respectively in librarySourceModel, where C is the current media (srcIndexOfCurrMedia.row() is 3)
     * and A is the file you want to delete. After deleting A, the row index of C is now actually 2.
    */
    if (srcIndexOfCurrMedia.isValid())
    {
        if (srcIndexOfCurrMedia.row() == indexOfFileToDelete.row())
        {
            srcIndexOfCurrMedia = QModelIndex();
            mediaPlayer->setMedia(QMediaContent());

        }
        else if (indexOfFileToDelete.row() < srcIndexOfCurrMedia.row())
        {
            srcIndexOfCurrMedia = srcIndexOfCurrMedia.siblingAtRow(srcIndexOfCurrMedia.row() - 1);
        }
    }
}


void PlayerWindow::customContextMenu(const QPoint &point)
{
    QMenu menu;
    menu.addAction("Delete", this, &PlayerWindow::deleteFromLibrary);

    QModelIndex index = ui->libraryView->indexAt(point);
    if (index.isValid())
    {
        menu.exec(ui->libraryView->viewport()->mapToGlobal(point));
    }
}


void PlayerWindow::setMediaForPlayback(const QModelIndex &selectedProxyIndex)
{
    if (selectedProxyIndex.isValid())
    {
        // Keep track of the index of the current media. This line must come before mediaPlayer->setMedia();
        srcIndexOfCurrMedia = libraryProxyModel->mapToSource(selectedProxyIndex);

        QModelIndex trackLocationIndex = selectedProxyIndex.siblingAtColumn(libraryProxyModel->getLocationColumn());
        QUrl trackLocation = libraryProxyModel->data(trackLocationIndex).toUrl();
        mediaPlayer->setMedia(trackLocation);
    }
    else
    {
        srcIndexOfCurrMedia = QModelIndex();
        mediaPlayer->setMedia(QMediaContent());
    }
}


void PlayerWindow::onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia)
    {
        mediaPlayer->play();
    }
    else if (status == QMediaPlayer::EndOfMedia)
    {
        setNextMediaForPlayback();
    }
    else if (status == QMediaPlayer::InvalidMedia)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "Invalid media.",
                                                                  "Invalid media. Unable to play. Would you like to delete file from library?",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            deleteFromLibrary();
        }
    }
}


void PlayerWindow::setNextMediaForPlayback()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    QModelIndex proxyIndexOfNextMedia = proxyIndexOfCurrMedia.siblingAtRow(proxyIndexOfCurrMedia.row()+1);
    setMediaForPlayback(proxyIndexOfNextMedia);
}


void PlayerWindow::setPreviousMediaForPlayback()
{
/* Plays previous media only if playback position in current media is less than 2000ms in.
 * Otherwise, restarts current media from beginning.
*/
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    if (mediaPlayer->position() < 2000)
    {
        QModelIndex proxyIndexOfPreviousMedia = proxyIndexOfCurrMedia.siblingAtRow(proxyIndexOfCurrMedia.row()-1);
        setMediaForPlayback(proxyIndexOfPreviousMedia);
    }
    else
    {
        setMediaForPlayback(proxyIndexOfCurrMedia);
    }
}


void PlayerWindow::updateCurrTrackLabel()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    if (proxyIndexOfCurrMedia.isValid())
    {
        QModelIndex trackTitleIndex = proxyIndexOfCurrMedia.siblingAtColumn(libraryProxyModel->getTitleColumn());
        QString currTrack = libraryProxyModel->data(trackTitleIndex).toString();
        QModelIndex trackArtistIndex = proxyIndexOfCurrMedia.siblingAtColumn(libraryProxyModel->getArtistColumn());
        QString currArtist = libraryProxyModel->data(trackArtistIndex).toString();
        if (!currArtist.isEmpty())
        {
            currArtist += " - ";
        }
        ui->currentTrackLabel->setText(currArtist + currTrack);
    }
    else
    {
        ui->currentTrackLabel->clear();
    }
}


void PlayerWindow::updateLibraryViewSelection()
{
    // Automatically highlight the currently playing track in the playlist view.
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    ui->libraryView->setCurrentIndex(proxyIndexOfCurrMedia);
}


QString formatDuration(int milliseconds)
{
    int remainingMillisec = milliseconds;
    int hours = milliseconds / 1000 / 60 / 60;
    remainingMillisec = milliseconds - (hours * 1000 * 60 * 60);
    int minutes = remainingMillisec / 1000 / 60;
    remainingMillisec = milliseconds - (minutes * 1000 * 60);
    int seconds = remainingMillisec / 1000;
    remainingMillisec = remainingMillisec - (seconds * 1000);

    QString format;
    if (hours)
    {
        format = "h:mm:ss";
    }
    else
    {
        format = "m:ss";
    }

    QTime time(hours, minutes, seconds);
    return time.toString(format);
}


void PlayerWindow::updateTotalTime(int duration)
{
    if (duration)
    {
        QString strTime;
        ui->totalTime->setText(formatDuration(duration));
    }
    else
    {
        ui->totalTime->setText("--:--");
    }
}


void PlayerWindow::updateTimePassed(int position)
{
    if (mediaPlayer->duration())
    {
        QString strTime;
        strTime = formatDuration(position);
        ui->timePassed->setText(strTime);
    }
    else
    {
        ui->timePassed->setText("--:--");
    }
}


void PlayerWindow::onPlayOrPauseSignal()
{
    if (mediaPlayer->state() == QMediaPlayer::State::PlayingState)
    {
        mediaPlayer->pause();
    }
    else if (mediaPlayer->state() == QMediaPlayer::PausedState)
    {
        mediaPlayer->play();
    }
    else // StoppedState
    {
        /* If pressing play button from a stopped state,
         * start playback from the highlighted song, if there is one.
         * Otherwise just start playback from the beginning of library.
        */
        QModelIndex selectedIndex = ui->libraryView->currentIndex();
        if (selectedIndex.isValid())
        {
            setMediaForPlayback(selectedIndex);
        }
        else
        {
            setMediaForPlayback(libraryProxyModel->index(0, 0, QModelIndex()));
        }
    }
}


void PlayerWindow::closeEvent(QCloseEvent *event)
{
    saveSessionState();
    event->accept();
}


void PlayerWindow::saveSessionState()
{
    saveWindowState();
    saveMediaPlayerVolume();
    saveLibraryViewState();
}


void PlayerWindow::saveWindowState()
{
    session.setValue("PlayerWindow/position", pos());
    session.setValue("PlayerWindow/isMaximized", isMaximized());
    session.setValue("PlayerWindow/size", size());
}


void PlayerWindow::saveMediaPlayerVolume()
{
    session.setValue("MediaPlayer/volume", mediaPlayer->volume());
}


void PlayerWindow::saveLibraryViewState()
{
    const int trackIdColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getTrackIdColumn());
    const int titleColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getTitleColumn());
    const int artistColumnWidthwidth = ui->libraryView->columnWidth(libraryProxyModel->getArtistColumn());
    const int albumColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getAlbumColumn());
    const int trackNumColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getTrackNumColumn());
    const int yearColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getYearColumn());
    const int genreColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getGenreColumn());
    const int durationColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getDurationColumn());
    const int locationColumnWidth = ui->libraryView->columnWidth(libraryProxyModel->getLocationColumn());
    session.setValue("LibraryView/trackIdColumnWidth", trackIdColumnWidth);
    session.setValue("LibraryView/titleColumnWidth", titleColumnWidth);
    session.setValue("LibraryView/artistColumnWidth", artistColumnWidthwidth);
    session.setValue("LibraryView/albumColumnWidth", albumColumnWidth);
    session.setValue("LibraryView/trackNumColumnWidth", trackNumColumnWidth);
    session.setValue("LibraryView/yearColumnWidth", yearColumnWidth);
    session.setValue("LibraryView/genreColumnWidth", genreColumnWidth);
    session.setValue("LibraryView/durationColumnWidth", durationColumnWidth);
    session.setValue("LibraryView/locationColumnWidth", locationColumnWidth);

    const int sortedColumn = ui->libraryView->header()->sortIndicatorSection();
    const int sortOrder = ui->libraryView->header()->sortIndicatorOrder();
    session.setValue("LibraryView/sortByColumn", sortedColumn);
    session.setValue("LibraryView/sortOrder", sortOrder);
}
