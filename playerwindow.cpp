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
    librarySourceModel->setTable("Track");
    librarySourceModel->setHeaderData(0, Qt::Horizontal, tr("Track ID"));
    librarySourceModel->setHeaderData(1, Qt::Horizontal, tr("Title"));
    librarySourceModel->setHeaderData(2, Qt::Horizontal, tr("Artist"));
    librarySourceModel->setHeaderData(3, Qt::Horizontal, tr("Album"));
    librarySourceModel->setHeaderData(4, Qt::Horizontal, tr("Track Number"));
    librarySourceModel->setHeaderData(5, Qt::Horizontal, tr("Year"));
    librarySourceModel->setHeaderData(6, Qt::Horizontal, tr("Genre"));
    librarySourceModel->setHeaderData(7, Qt::Horizontal, tr("Duration"));
    librarySourceModel->setHeaderData(8, Qt::Horizontal, tr("Location"));
    librarySourceModel->select();

    libraryProxyModel = new LibraryModel(this);
    libraryProxyModel->setSourceModel(librarySourceModel);
    /*
    libraryProxyModel->setHeaderData(libraryProxyModel->trackIdColumn(), Qt::Horizontal, tr("Track ID"));
    libraryProxyModel->setHeaderData(libraryProxyModel->titleColumn(), Qt::Horizontal, tr("Title"));
    libraryProxyModel->setHeaderData(libraryProxyModel->artistColumn(), Qt::Horizontal, tr("Artist"));
    libraryProxyModel->setHeaderData(libraryProxyModel->albumColumn(), Qt::Horizontal, tr("Album"));
    libraryProxyModel->setHeaderData(libraryProxyModel->trackNumColumn(), Qt::Horizontal, tr("Track Number"));
    libraryProxyModel->setHeaderData(libraryProxyModel->yearColumn(), Qt::Horizontal, tr("Year"));
    libraryProxyModel->setHeaderData(libraryProxyModel->genreColumn(), Qt::Horizontal, tr("Genre"));
    libraryProxyModel->setHeaderData(libraryProxyModel->durationColumn(), Qt::Horizontal, tr("Duration"));
    libraryProxyModel->setHeaderData(libraryProxyModel->locationColumn(), Qt::Horizontal, tr("Location"));
    */
    libraryProxyModel->setDynamicSortFilter(true);
}


void PlayerWindow::initializeLibraryTreeView()
{
    ui->playlistView->setModel(libraryProxyModel);
    ui->playlistView->sortByColumn(libraryProxyModel->artistColumn(), Qt::SortOrder::AscendingOrder);
    ui->playlistView->setColumnHidden(libraryProxyModel->trackIdColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->trackNumColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->yearColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->locationColumn(), true);
}


void PlayerWindow::setUpConnections()
{
    connect(ui->playlistView, &QTreeView::doubleClicked, this, &PlayerWindow::setMediaForPlayback);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onMediaPlayerStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &PlayerWindow::updatePlaylistTreeViewSelection); // Check difference between QMediaPlayer's mediChanged and currentMediaChanged signals
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setPlayButtonLabel); // maybe just rename setPlayButtonLabel to setControlState
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

        QModelIndex trackLocationIndex = selectedProxyIndex.siblingAtColumn(libraryProxyModel->locationColumn());
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
        QModelIndex trackTitleIndex = proxyIndexOfCurrMedia.siblingAtColumn(libraryProxyModel->titleColumn());
        QString currTrack = libraryProxyModel->data(trackTitleIndex).toString();
        QModelIndex trackArtistIndex = proxyIndexOfCurrMedia.siblingAtColumn(libraryProxyModel->artistColumn());
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
    trackRecord.setValue("track_num", trackNum);
    trackRecord.setValue("year", year);
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
        QString artist = mediaToBeAdded->metaData(QMediaMetaData::Author).toString() + "";
        /* The + "" is necessary because if the metadata for the arguement passed to metaData() doesn't exist,
         * then using .toString() on the result of metaData() will result in a NULL string. The + "" will convert
         * this into an emptry string, which is what I need because I want an empty string to be a valid input (and
         * for NULL to be invalid input) for the name field of the Artist table in the media database and NULL input to be invalid.
        */

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
