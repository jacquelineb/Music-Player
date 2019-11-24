#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    connect(ui->playButton, &QAbstractButton::clicked, this, &PlayerControls::clickPlay);
}

PlayerControls::~PlayerControls()
{
    delete ui;
}

void PlayerControls::clickPlay()
{
    ui->playButton->setText("you clicked play");
}
