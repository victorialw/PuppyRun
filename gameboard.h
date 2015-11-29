//
// gameboard.h
//
//  Created by Victoria Wen on 10/20/15.
//  Copyright (c) 2015 Victoria Wen. All rights reserved.
//

/** @file gameboard.h
@brief declares the GameBoard class
@author Victoria Wen
@date October 20, 2015
*/

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT

public slots:
    void updateCars();
    void endGame();
    void update_powerup();

public:
    explicit GameBoard(QWidget* parent = 0, size_t num_roads = 4, size_t car_speed = 100);
    ~GameBoard();

    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);

    void updatePuppy(int px, int py, int nx, int ny);
    void checkPuppy();

private:
    Ui::GameBoard *ui;

    // Top bar variables
    QWidget* Top;
    unsigned int lives_remaining;
    QLabel** lives;
    QLabel* level_value;

    // Board variables
    QWidget* Board;
    size_t num_roads;
    QLabel** labels;
    size_t num_lines;
    bool end_game;

    // Car variables
    QPixmap* car_image;
    QPoint* car_positions;
    size_t car_speed;
    size_t num_cars;

    // Puppy variables
    const QPixmap* puppy_image;
    QPoint* puppy_position;

    // Bone variables
    const QPixmap* bone_image;
    QPoint* bone_position;

    // Power up variables
    QPixmap* powerup;
    QPoint* powerup_position;

    // Blood splatter variables
    bool if_hit;
    QPoint* hit_pos;
    QPixmap* blood;
    size_t num_blood;

};

#endif // GAMEBOARD_H
