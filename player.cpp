#include "player.h"
#include "ui_player.h"

#include <QDebug>
#include <QCloseEvent> // why does it still work if i comment this out
//#include <QFileInfo>
//#include <QMediaMetaData>
//#include <QSqlQuery>
//#include <QSqlError> // maybe delete this later
//#include <QMediaPlaylist>
//#include <QSqlRecord>
//#include <QSqlField>
//#include <QSqlIndex> //

#include "libraryplaylistmodel.h" //

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    initializeLibraryModels();
    initializeLibraryTreeView();
    initializeMediaPlayer();
    setUpConnections();
}

Player::~Player()
{
    delete ui;
}

void Player::initializeLibraryModels()
{
    librarySourceModel = new QSqlRelationalTableModel(this);
    librarySourceModel->setTable("Track");
    const int artistIdColumn = 2;
    librarySourceModel->setRelation(artistIdColumn, QSqlRelation("Artist", "id", "name"));
    librarySourceModel->select();

    libraryProxyModel = new LibraryPlaylistModel(this);
    libraryProxyModel->setSourceModel(librarySourceModel);
    libraryProxyModel->setHeaderData(libraryProxyModel->trackIdColumn(), Qt::Horizontal, tr("Track ID"));
    libraryProxyModel->setHeaderData(libraryProxyModel->titleColumn(), Qt::Horizontal, tr("Title"));
    libraryProxyModel->setHeaderData(libraryProxyModel->artistColumn(), Qt::Horizontal, tr("Artist"));
    libraryProxyModel->setHeaderData(libraryProxyModel->albumColumn(), Qt::Horizontal, tr("Album"));
    libraryProxyModel->setHeaderData(libraryProxyModel->trackNumColumn(), Qt::Horizontal, tr("Track Number"));
    libraryProxyModel->setHeaderData(libraryProxyModel->yearColumn(), Qt::Horizontal, tr("Year"));
    libraryProxyModel->setHeaderData(libraryProxyModel->genreColumn(), Qt::Horizontal, tr("Genre"));
    libraryProxyModel->setHeaderData(libraryProxyModel->durationColumn(), Qt::Horizontal, tr("Duration"));
    libraryProxyModel->setHeaderData(libraryProxyModel->locationColumn(), Qt::Horizontal, tr("Location"));
    libraryProxyModel->setDynamicSortFilter(true);
}

void Player::initializeLibraryTreeView()
{
    ui->playlistView->setModel(libraryProxyModel);
    ui->playlistView->sortByColumn(libraryProxyModel->artistColumn(), Qt::SortOrder::AscendingOrder);

    ui->playlistView->setColumnHidden(libraryProxyModel->trackIdColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->trackNumColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->yearColumn(), true);
    ui->playlistView->setColumnHidden(libraryProxyModel->locationColumn(), true);
}

void Player::initializeMediaPlayer()
{
    mediaPlayer = new QMediaPlayer(this);
    restorePlayerSettings();
}

void Player::restorePlayerSettings()
{
    /* Restore volume, last playlist, etc. */
    mediaPlayer->setVolume(ui->controls->getVolume());
}

void Player::setUpConnections()
{
    connect(ui->playlistView, &QTreeView::doubleClicked, this, &Player::setMediaForPlayback);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &Player::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &Player::updatePlaylistTreeViewSelection); // Check difference between QMediaPlayer's mediChanged and currentMediaChanged signals
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setPlayButtonLabel); // maybe just rename setPlayButtonLabel to setControlState
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &Player::playOrPauseMedia);
    connect(ui->controls, &PlayerControls::nextClicked, this, &Player::setNextMediaForPlayback);
    connect(ui->controls, &PlayerControls::prevClicked, this, &Player::setPreviousMediaForPlayback);
}

void Player::setMediaForPlayback(const QModelIndex &selectedProxyIndex)
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

void Player::onStatusChanged(QMediaPlayer::MediaStatus status)
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

void Player::playLoadedMedia()
{
    mediaPlayer->play();
}

void Player::setNextMediaForPlayback()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    QModelIndex proxyIndexOfNextMedia = proxyIndexOfCurrMedia.siblingAtRow(proxyIndexOfCurrMedia.row()+1);
    setMediaForPlayback(proxyIndexOfNextMedia);
}

void Player::setPreviousMediaForPlayback()
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

void Player::updateCurrTrackLabel()
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

void Player::updatePlaylistTreeViewSelection()
{
    // Automatically highlight the currently playing track in the playlist view.
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    ui->playlistView->setCurrentIndex(proxyIndexOfCurrMedia);
}

void Player::playOrPauseMedia()
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

void Player::savePlayerSettings()
{
    /* Save which playlist was last opened */
}

void Player::closeEvent(QCloseEvent *event)
{
    savePlayerSettings();
    ui->controls->close();
    event->accept();
}

//
void Player::updatePlaylistView()
{
    librarySourceModel->select();
}

