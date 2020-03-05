#include "playerwindow.h"
#include "ui_playerwindow.h"

#include "trackdurationdelegate.h"

#include <taglib/fileref.h>

#include <QFileDialog>
#include <QSqlError>
#include <QSqlRecord>


PlayerWindow::PlayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerWindow)
{
    ui->setupUi(this);
    restoreWindowState();
    initializeMediaPlayer();
    initializeLibraryModels();
    initializeLibraryTreeView();
    setUpConnections();
}


PlayerWindow::~PlayerWindow()
{
    delete ui;
}


void PlayerWindow::initializeMediaPlayer()
{
    mediaPlayer = new QMediaPlayer(this);
    restoreMediaPlayerVolume();
}

void PlayerWindow::restoreMediaPlayerVolume()
{
    const int DEFAULT_VOLUME = 100; // This should match DEFAULT_VOLUME in PlayerControls::restoreVolumeSliderState()
    int volume = settings.value("MediaPlayer/volume", DEFAULT_VOLUME).toInt();
    mediaPlayer->setVolume(volume);
}


void PlayerWindow::initializeLibraryModels()
{
    librarySourceModel = new QSqlTableModel(this);
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

    libraryProxyModel = new LibraryModel(this);
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


void PlayerWindow::initializeLibraryTreeView()
{
    ui->libraryView->setModel(libraryProxyModel);
    ui->libraryView->setItemDelegateForColumn(libraryProxyModel->getDurationColumn(), new TrackDurationDelegate);
    ui->libraryView->setCurrentIndex(QModelIndex());
    restoreLibraryViewState();
}


void PlayerWindow::restoreLibraryViewState()
{
    // Restore the column widths
    const int DEFAULT_COLUMN_WIDTH = ui->libraryView->header()->defaultSectionSize();
    const int trackIdColumnWidth = settings.value("LibraryView/trackIdColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int titleColumnWidth = settings.value("LibraryView/titleColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int artistColumnWidth = settings.value("LibraryView/artistColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int albumColumnWidth = settings.value("LibraryView/albumColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int trackNumColumnWidth = settings.value("LibraryView/trackNumColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int yearColumnWidth = settings.value("LibraryView/yearColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int genreColumnWidth = settings.value("LibraryView/genreColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int durationColumnWidth = settings.value("LibraryView/durationColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int locationColumnWidth = settings.value("LibraryView/locationColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
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
    if (settings.value("LibraryView/sortOrder").toInt() == 0)
    {
        sortOrder = Qt::SortOrder::AscendingOrder;
    }
    else
    {
        sortOrder = Qt::SortOrder::DescendingOrder;
    }
    const int sortColumn = settings.value("LibraryView/sortByColumn").toInt();
    ui->libraryView->sortByColumn(sortColumn, sortOrder);
}


void PlayerWindow::setUpConnections()
{
    connect(ui->actionAddToLibrary, &QAction::triggered, this, &PlayerWindow::onAddToLibraryActionTriggered);

    connect(ui->libraryView, &QTreeView::activated, this, &PlayerWindow::setMediaForPlayback);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onMediaPlayerStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updatePlaylistTreeViewSelection);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::updatePlaybackState);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &PlayerWindow::onPlayOrPauseSignal);
    connect(ui->controls, &PlayerControls::nextClicked, this, &PlayerWindow::setNextMediaForPlayback);
    connect(ui->controls, &PlayerControls::prevClicked, this, &PlayerWindow::setPreviousMediaForPlayback);
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
        //qDebug() << mediaPlayer->isAudioAvailable();
        //qDebug() << mediaPlayer->isVideoAvailable();
        mediaPlayer->play();
    }
    else if (status == QMediaPlayer::EndOfMedia)
    {
        setNextMediaForPlayback();
    }
    else if (status == QMediaPlayer::InvalidMedia)
    {
        qDebug() << "Line 174: " << status;
        // Notify user that the media was invalid and could not be played. Give them option to remove song.
        // An example of when this status might occur is if a media file couldn't be located, eg., if I deleted an added song file from disk.
    }
    else
    {
        qDebug() << "Line 179: " << status;
    }

    //qDebug() << "184: " << mediaPlayer->errorString();
    //qDebug() << mediaPlayer->error();
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
        //mediaPlayer->setPosition(0);
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

        QString currentlyPlayingText = "Now Playing: ";
        if (!currArtist.isEmpty())
        {
            currentlyPlayingText += currArtist + " - ";
        }
        currentlyPlayingText += currTrack;
        ui->currentTrackLabel->setText(currentlyPlayingText);
    }
    else
    {
        ui->currentTrackLabel->clear();
    }
}


void PlayerWindow::updatePlaylistTreeViewSelection()
{
    // Automatically highlight the currently playing track in the playlist view.
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    ui->libraryView->setCurrentIndex(proxyIndexOfCurrMedia);
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
         * start playback from the first highlighted song if there is one.
         * Otherwise just start playback from the beginning of playlist.
        */
        QModelIndex selectedIndex = ui->libraryView->currentIndex();
        if (selectedIndex.isValid())
        {
            setMediaForPlayback(selectedIndex);
        }
        else // Thinking of just removing this branch
        {
            setMediaForPlayback(libraryProxyModel->index(0, 0, QModelIndex()));
        }
    }
}


void PlayerWindow::onAddToLibraryActionTriggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Add file to library");

    TagLib::FileRef fileRef(filepath.toStdWString().c_str());
    if (!fileRef.isNull())
    {
        QString title = fileRef.tag()->title().toCString(true);
        QString artist = fileRef.tag()->artist().toCString(true);
        QString album = fileRef.tag()->album().toCString(true);
        unsigned int trackNum = fileRef.tag()->track();
        unsigned int year = fileRef.tag()->year();
        QString genre = fileRef.tag()->genre().toCString(true);
        int duration = fileRef.audioProperties()->lengthInMilliseconds();

        insertToTrackTable(title, artist, album, trackNum, year, genre, duration, filepath);
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

    qDebug() << librarySourceModel->insertRecord(librarySourceModel->rowCount(), trackRecord);
    qDebug() << librarySourceModel->lastError();
}


void PlayerWindow::restoreWindowState()
{
    move(settings.value("PlayerWindow/position", pos()).toPoint());
    if (settings.value("PlayerWindow/isMaximized", true).toBool())
    {
        setWindowState(Qt::WindowState::WindowMaximized);
    }
    else
    {
        resize(settings.value("PlayerWindow/size", size()).toSize());
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
    settings.setValue("PlayerWindow/position", pos());
    settings.setValue("PlayerWindow/isMaximized", isMaximized());
    settings.setValue("PlayerWindow/size", size());
}


void PlayerWindow::saveMediaPlayerVolume()
{
    settings.setValue("MediaPlayer/volume", mediaPlayer->volume());
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
    settings.setValue("LibraryView/trackIdColumnWidth", trackIdColumnWidth);
    settings.setValue("LibraryView/titleColumnWidth", titleColumnWidth);
    settings.setValue("LibraryView/artistColumnWidth", artistColumnWidthwidth);
    settings.setValue("LibraryView/albumColumnWidth", albumColumnWidth);
    settings.setValue("LibraryView/trackNumColumnWidth", trackNumColumnWidth);
    settings.setValue("LibraryView/yearColumnWidth", yearColumnWidth);
    settings.setValue("LibraryView/genreColumnWidth", genreColumnWidth);
    settings.setValue("LibraryView/durationColumnWidth", durationColumnWidth);
    settings.setValue("LibraryView/locationColumnWidth", locationColumnWidth);

    const int sortedColumn = ui->libraryView->header()->sortIndicatorSection();
    const int sortOrder = ui->libraryView->header()->sortIndicatorOrder();
    settings.setValue("LibraryView/sortByColumn", sortedColumn);
    settings.setValue("LibraryView/sortOrder", sortOrder);
}
