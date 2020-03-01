#include "playerwindow.h"
#include "ui_playerwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QMediaMetaData>
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

    connect(ui->actionAddToLibrary, &QAction::triggered, this, &PlayerWindow::onAddToLibraryActionTriggered);
    // =======
    // Change this later. Use taglib. Then combine the code into onAddToLibraryActionTriggered.
    mediaToBeAdded = new QMediaPlayer(this);
    connect(mediaToBeAdded, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onAddMediaStatusChanged);
    // =======
}


PlayerWindow::~PlayerWindow()
{
    delete ui;
}


void PlayerWindow::initializeMediaPlayer()
{
    mediaPlayer = new QMediaPlayer(this);

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
    ui->playlistView->setModel(libraryProxyModel);
    ui->playlistView->sortByColumn(libraryProxyModel->getArtistColumn(), Qt::SortOrder::AscendingOrder);
    ui->playlistView->setColumnHidden(libraryProxyModel->getTrackIdColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->getLocationColumn(), true);

    // Restore column widths.
    const int DEFAULT_COLUMN_WIDTH = 100;
    const int trackIdColumnWidth = settings.value("PlaylistView/trackIdColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int titleColumnWidth = settings.value("PlaylistView/titleColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int artistColumnWidth = settings.value("PlaylistView/artistColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int albumColumnWidth = settings.value("PlaylistView/albumColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int trackNumColumnWidth = settings.value("PlaylistView/trackNumColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int yearColumnWidth = settings.value("PlaylistView/yearColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int genreColumnWidth = settings.value("PlaylistView/genreColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int durationColumnWidth = settings.value("PlaylistView/durationColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();
    const int locationColumnWidth = settings.value("PlaylistView/locationColumnWidth", DEFAULT_COLUMN_WIDTH).toInt();

    ui->playlistView->setColumnWidth(libraryProxyModel->getTrackIdColumn(), trackIdColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getTitleColumn(), titleColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getArtistColumn(), artistColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getAlbumColumn(), albumColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getTrackNumColumn(), trackNumColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getYearColumn(), yearColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getGenreColumn(), genreColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getDurationColumn(), durationColumnWidth);
    ui->playlistView->setColumnWidth(libraryProxyModel->getLocationColumn(), locationColumnWidth);
}


void PlayerWindow::setUpConnections()
{
    //connect(ui->playlistView, &QTreeView::doubleClicked, this, &PlayerWindow::setMediaForPlayback);
    connect(ui->playlistView, &QTreeView::activated, this, &PlayerWindow::setMediaForPlayback);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onMediaPlayerStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updatePlaylistTreeViewSelection);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::updatePlaybackState);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &PlayerWindow::playOrPauseMedia);
    connect(ui->controls, &PlayerControls::nextClicked, this, &PlayerWindow::setNextMediaForPlayback);
    connect(ui->controls, &PlayerControls::prevClicked, this, &PlayerWindow::setPreviousMediaForPlayback);
}

void PlayerWindow::setMediaForPlayback(const QModelIndex &selectedProxyIndex)
{
    if (selectedProxyIndex.isValid())
    {
        // Keep track of the index of the current media.
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
        qDebug() << "Playing media";
        playLoadedMedia();
    }
    else if (status == QMediaPlayer::EndOfMedia)
    {
        setNextMediaForPlayback();
    }
    else if (status == QMediaPlayer::InvalidMedia)
    {
        qDebug() << "Line 132: " << status;
        // Notify user that the media was invalid and could not be played. Give them option to remove song.
        // An example of when this status might occur is if a media file couldn't be located, eg., if I deleted an added song file from disk.
    }
    else
    {
        qDebug() << "Line 140: " << status;
    }
}


void PlayerWindow::playLoadedMedia()
{
    mediaPlayer->play();
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
    ui->playlistView->setCurrentIndex(proxyIndexOfCurrMedia);
}


void PlayerWindow::playOrPauseMedia()
{
    if (mediaPlayer->state() == QMediaPlayer::State::PlayingState)
    {
        mediaPlayer->pause();
    }
    else // Pause or Stopped State
    {
        if (mediaPlayer->state() == QMediaPlayer::State::StoppedState)
        {
            /* If pressing play button from a stopped state,
             * start playback from the first highlighted song if there is one.
             * Otherwise just start playback from the beginning of playlist.
            */
            QModelIndex selectedIndex = ui->playlistView->currentIndex();
            if (selectedIndex.isValid())
            {
                setMediaForPlayback(selectedIndex);
            }
            else
            {
                setMediaForPlayback(libraryProxyModel->index(0, 0, QModelIndex()));
            }
        }
        mediaPlayer->play();
    }
}


void PlayerWindow::onAddToLibraryActionTriggered()
{
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Add file to library");
    mediaToBeAdded->setMedia(filename);
}

// ===========================================================
void PlayerWindow::insertToTrackTable(const QString &title,
                                      const QString &artist,
                                      const QString &album,
                                      int trackNum,
                                      int year,
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


void PlayerWindow::onAddMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia)
    {
        // Can only get the metadata once the media has loaded
        QString location = mediaToBeAdded->currentMedia().canonicalUrl().toLocalFile();
        QString title = mediaToBeAdded->metaData(QMediaMetaData::Title).toString();
        if (title.isEmpty())
        {
            title = QFileInfo(location).completeBaseName();
        }
        QString artist = mediaToBeAdded->metaData(QMediaMetaData::Author).toString();
        QString album = mediaToBeAdded->metaData(QMediaMetaData::AlbumTitle).toString();
        int trackNum = mediaToBeAdded->metaData(QMediaMetaData::TrackNumber).toInt();
        int year = mediaToBeAdded->metaData(QMediaMetaData::Year).toInt();
        QString genre = mediaToBeAdded->metaData(QMediaMetaData::Genre).toString();
        int duration = mediaToBeAdded->metaData(QMediaMetaData::Duration).toInt();

        insertToTrackTable(title, artist, album, trackNum, year, genre, duration, location);
        // make insertToTrackTable() a bool. If false, notify the user that the track couldn't be added to the library.
        // One reason it might have failed is if the song already exists in the library.
    }
}
// ===========================================================


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


void PlayerWindow::saveWindowState()
{
    settings.setValue("PlayerWindow/position", pos());
    settings.setValue("PlayerWindow/isMaximized", isMaximized());
    settings.setValue("PlayerWindow/size", size());

    settings.setValue("MediaPlayer/volume", mediaPlayer->volume());

    // Save the column widths
    int trackIdColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getTrackIdColumn());
    int titleColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getTitleColumn());
    int artistColumnWidthwidth = ui->playlistView->columnWidth(libraryProxyModel->getArtistColumn());
    int albumColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getAlbumColumn());
    int trackNumColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getTrackNumColumn());
    int yearColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getYearColumn());
    int genreColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getGenreColumn());
    int durationColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getDurationColumn());
    int locationColumnWidth = ui->playlistView->columnWidth(libraryProxyModel->getLocationColumn());

    settings.setValue("PlaylistView/trackIdColumnWidth", trackIdColumnWidth);
    settings.setValue("PlaylistView/titleColumnWidth", titleColumnWidth);
    settings.setValue("PlaylistView/artistColumnWidth", artistColumnWidthwidth);
    settings.setValue("PlaylistView/albumColumnWidth", albumColumnWidth);
    settings.setValue("PlaylistView/trackNumColumnWidth", trackNumColumnWidth);
    settings.setValue("PlaylistView/yearColumnWidth", yearColumnWidth);
    settings.setValue("PlaylistView/genreColumnWidth", genreColumnWidth);
    settings.setValue("PlaylistView/durationColumnWidth", durationColumnWidth);
    settings.setValue("PlaylistView/locationColumnWidth", locationColumnWidth);
}


void PlayerWindow::closeEvent(QCloseEvent *event)
{
    saveWindowState();
    event->accept();
}
