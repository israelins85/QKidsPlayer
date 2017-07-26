#include "playercontroller.h"
#include "ui_playercontroller.h"

PlayerController::PlayerController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerController)
{
    ui->setupUi(this);
//    setVisible(false);
//    setWindowFlags(Qt::FramelessWindowHint);
//    setAttribute(Qt::WA_TranslucentBackground);
}

PlayerController::~PlayerController()
{
    delete ui;
}
