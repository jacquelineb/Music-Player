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
    restorePlayerSettings();
}


void PlayerWindow::restorePlayerSettings()
{
    /* Restore volume, last playlist, etc. */
    mediaPlayer->setVolume(ui->controls->getVolume());
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
}


void PlayerWindow::setUpConnections()
{
    connect(ui->playlistView, &QTreeView::doubleClicked, this, &PlayerWindow::setMediaForPlayback);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onMediaPlayerStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updatePlaylistTreeViewSelection);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::updatePlaybackState);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
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


void PlayerWindow::updatePlaylistTreeViewSelection()
{
    // Automatically highlight the currently playing track in the playlist view.
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    ui->playlistView->setCurrentIndex(proxyIndexOfCurrMedia);
}


void PlayerWindow::setNextMediaForPlayback()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    QModelIndex proxyIndexOfNextMedia = proxyIndexOfCurrMedia.siblingAtRow(proxyIndexOfCurrMedia.row()+1);
    setMediaForPlayback(proxyIndexOfNextMedia);
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
    librarySourceModel->insertRecord(librarySourceModel->rowCount(), trackRecord);

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

        qDebug() << "Location: " << location;
        qDebug() << "Title: " << title;
        qDebug() << "Artist: " << artist;
        qDebug() << "Album: " << album;
        qDebug() << "Track Number: " << trackNum;
        qDebug() << "Year: " << year;
        qDebug() << "Genre: " << genre;
        qDebug() << "Duration: " << duration;

        insertToTrackTable(title, artist, album, trackNum, year, genre, duration, location);
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
}


void PlayerWindow::closeEvent(QCloseEvent *event)
{
    saveWindowState();
    ui->controls->close();
    event->accept();
}
