#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QWidget>

namespace Ui {
    class PlayerController;
}

class PlayerController : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerController(QWidget *parent = 0);
    ~PlayerController();

private:
    Ui::PlayerController *ui;
};

#endif // PLAYERCONTROLLER_H
