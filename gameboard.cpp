//
// gameboard.cpp
//
//  Created by Victoria Wen on 10/20/15.
//  Copyright (c) 2015 Victoria Wen. All rights reserved.
//

/** @file gameboard.cpp
@brief implements the GameBoard class
@author Victoria Wen
@date October 20, 2015
*/

#include "gameboard.h"
#include "ui_gameboard.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QKeyEvent>
#include <random>
#include <chrono>
#include <QTimer>
#include <QMessageBox>
#include <QSpacerItem>
#include <QTime>

// obtain a seed from system clock
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
// setting seed for random number generator
std::default_random_engine generator (seed);

/**
 * @brief GameBoard constructor
 * @param parent is the parent of the widget
 * @param num_roads is how many roads are on the board
 * @param car_speed is speed of car
 */

GameBoard::GameBoard(QWidget* parent, size_t num_roads, size_t car_speed) :
    QWidget(parent),
    ui(new Ui::GameBoard)
{
    ui->setupUi(this);

    setFixedSize(700,600);

    end_game = false;
    if_hit = false;
    num_blood = 0;
    this->num_roads = num_roads;
    num_lines = num_roads + 2;
    this->car_speed = car_speed;

    // This is overall layout
    QVBoxLayout* overall_layout = new QVBoxLayout;
    this->setLayout(overall_layout);

    // Defining puppy_image
    QString fileName(":/Images/Pomsky_Run.jpg");
    puppy_image = new QPixmap(fileName);

    // Making top bar and adding it to the overall layout
    Top = new QWidget;
    QHBoxLayout* top_bar = new QHBoxLayout(Top);
    overall_layout->addWidget(Top,0,Qt::AlignHCenter);
    lives_remaining = 3;
    lives = new QLabel*[lives_remaining];

    // Set the labels to have the puppy image, add to the top bar layout.
    for(size_t i=0;i<lives_remaining;i++) {
        lives[i] = new QLabel;
        lives[i]->setPixmap(*puppy_image);
        lives[i]->setMinimumSize(65,65);
        lives[i]->setMaximumSize(65,65);
        lives[i]->setScaledContents(true);
        top_bar->addWidget(lives[i]);
    }

    QSpacerItem* horizontal_space = new QSpacerItem(20,20);
    top_bar->addSpacerItem(horizontal_space);

    // initiates blood
    blood = new QPixmap(":/Images/Blood.png");

    // Set the level
    QLabel* level_text = new QLabel("Level: ");
    level_value = new QLabel(QString::number(1)); // start with level 1.
    level_value->setMinimumSize(100, 100);
    level_value->setMaximumSize(100, 100);
    top_bar->addWidget(level_text);
    top_bar->addWidget(level_value);

    // This code creates the board
    Board = new QWidget;
    Board->setFixedSize(500,400);
    Board->setContentsMargins(10,10,10,10);
    QGridLayout* SquareGrid = new QGridLayout(Board);
    overall_layout->addWidget(Board,0,Qt::AlignCenter);
    this->setLayout(overall_layout);
    SquareGrid->setGeometry(QRect());
    SquareGrid->setSpacing(0);

    labels = new QLabel*[num_lines * num_lines];

    // Create each label for the board
    for(size_t i=0;i < (num_lines) ;i++) {
        for(size_t j=0;j < (num_lines);j++) {

            // Create label and set properties.
            labels[i*num_lines+j] = new QLabel;
            labels[i*num_lines+j]->setMinimumSize(400 / num_lines,400 / num_lines);
            labels[i*num_lines+j]->setMaximumSize(400 / num_lines,400 / num_lines);
            if(j == 0 || j == num_lines - 1) {
                labels[i*num_lines+j]->setStyleSheet("QLabel { background-color : white; }");
            } else {
                labels[i*num_lines+j]->setStyleSheet("QLabel { background-color : black; }");
            }

            labels[i*num_lines+j]->setAlignment(Qt::AlignCenter);

            // Add label to the layout
            SquareGrid -> addWidget(labels[i*num_lines+j] ,i,j);

        }
    }


    // Initialize the puppy at the top left corner, coordinate (0,0).
    puppy_position = new QPoint(0,0);
    labels[0]->setPixmap(*puppy_image);
    labels[0]->setScaledContents(true);

    // Initialize the bone to bottom right corner
    QString fileNameBone(":/Images/bone.gif");
    bone_image = new QPixmap(fileNameBone);
    labels[num_lines*num_lines - 1]->setPixmap(*bone_image);
    labels[num_lines*num_lines - 1]->setScaledContents(true);
    bone_position = new QPoint(num_lines - 1, num_lines - 1);


    // Initialize car in random places of each road
     std::uniform_int_distribution<int> distribution(0,num_lines - 2);
     QString fileNameCar(":/Images/Car.png");
     car_image = new QPixmap(fileNameCar);

     num_cars = num_roads;
     car_positions = new QPoint[num_cars];


    for(size_t j = 1; j < num_lines - 1; j++) {
        size_t desired_row_for_car = distribution(generator);
        car_positions[j-1].setX(j);
        car_positions[j-1].setY(desired_row_for_car);
    }

    // Setting timer for car
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCars()));
    timer->start(car_speed);

    // Initialize powerup
    QString fileNamePower(":/Images/TennisBall.png");
    powerup = new QPixmap(fileNamePower);
    int nx = distribution(generator);
    int ny = distribution(generator);

    while (nx == 0 || nx == num_lines-1) {
        nx = distribution(generator);

    }

    // Setting timer for powerup
    powerup_position = new QPoint(nx,ny);
    QTimer* ptimer = new QTimer(this);
    connect(ptimer, SIGNAL(timeout()), this, SLOT(update_powerup()));
    ptimer->start(5000);


    // Making the quit button
    QPushButton* quit = new QPushButton("New Game");
    QObject::connect(quit, SIGNAL(clicked()), parent, SLOT(game_over()));


    overall_layout->addWidget(quit);
}

/**
 * @brief GameBoard destructor
 */
GameBoard::~GameBoard()
{
    delete puppy_position;
    delete [] car_positions;
    delete bone_position;
    delete ui;
}

/**
 * @brief moves the puppy when a key pressed, specifically the up, down, right, left arrows
 * @param event is the event that occurs
 */
void GameBoard::keyPressEvent(QKeyEvent *event){
    if(!end_game) {
        int x = puppy_position->rx();
        int y = puppy_position->ry();

        switch (event->key()) {
        case Qt::Key_Left:
            if(puppy_position->rx() != 0)
                updatePuppy(x,y,x-1,y);
            break;
        case Qt::Key_Right:
            if(puppy_position->rx() != num_lines - 1)
                updatePuppy(x,y,x+1,y);
            break;
        case Qt::Key_Up:
            if(puppy_position->ry() != 0)
                updatePuppy(x,y,x,y-1);
            break;
        case Qt::Key_Down:
            if(puppy_position->ry() != num_lines - 1)
                updatePuppy(x,y,x,y+1);
            break;

        default:
            QWidget::keyPressEvent(event);
        }
        checkPuppy();
        QCoreApplication::processEvents();
        repaint();
    }
    return;
}


/**
 * @brief paints everything
 * @param e is the event that occurs
 */

void GameBoard::paintEvent(QPaintEvent *e) {
    if(!end_game) {

        // paint bone
        int bx = bone_position->rx();
        int by = bone_position->ry();

        labels[by*num_lines+bx]->setPixmap(*bone_image);
        labels[by*num_lines+bx]->setScaledContents(true);


        // paint powerup
        int px = powerup_position->rx();
        int py = powerup_position->ry();
        labels[py*num_lines+px]->setPixmap(*powerup);
        labels[py*num_lines+px]->setScaledContents(true);


        // paint puppy
        int x = puppy_position->rx();
        int y = puppy_position->ry();

        labels[y*num_lines+x]->setPixmap(*puppy_image);
        labels[y*num_lines+x]->setScaledContents(true);


        // paint cars
        for(size_t i=0;i<num_cars;i++) {
            int px = car_positions[i].rx();
            int py = car_positions[i].ry();

            if(px >= 1 && py >= 0 && px < (int)(num_lines - 1) && py < (int)num_lines) {
                labels[py*num_lines+px]->setPixmap(*car_image);
                labels[py*num_lines+px]->setScaledContents(true);
            }
        }


        // react if hit
        if (if_hit && num_blood < 20) {

            int hit_x = hit_pos->rx();
            int hit_y = hit_pos->ry();
            labels[hit_y*num_lines+hit_x]->setPixmap(*blood);
            labels[hit_y*num_lines+hit_x]->setScaledContents(true);
            num_blood++;
        }
        if (if_hit && num_blood >= 20) {
            num_blood = 0;
            if_hit = false;
        }

    }
}

/**
 * @brief show the event
 * @param e is the event that occurs
 */

void GameBoard::showEvent(QShowEvent *e) {

    this->activateWindow();
    this->setFocus();
    QWidget::showEvent(e);
}

/**
 * @brief updates the puppy's position
 * @param px is the old x coordinate
 * @param py is the old y coordinate
 * @param nx is the new x coordinate
 * @param ny is the new y coordinate
 */

void GameBoard::updatePuppy(int px, int py, int nx, int ny) {
    QLabel *label = labels[py*num_lines+px];
    label->clear();

    puppy_position->setX(nx);
    puppy_position->setY(ny);

}

/**
 * @brief updates car's position
 */

void GameBoard::updateCars() {
    if(!end_game) {
         for(size_t i = 0; i < num_cars; i++) {
            int px = car_positions[i].rx();
            int py = car_positions[i].ry();
            int ny = py - 1;
            if(py > 0) {
                car_positions[i].setY(ny);
                labels[py*num_lines + px]->clear();
            } else {
                ny = num_lines - 1;
                labels[py*num_lines + px]->clear();
                car_positions[i].setY(ny);
            }
         }
       }



    QCoreApplication::processEvents();
    repaint();

    checkPuppy();
}

/**
  * @brief changes powerup's location
  */
void GameBoard::update_powerup() {
    if(!end_game) {
            std::uniform_int_distribution<int> distribution(0,num_roads);
            int pup_x = puppy_position->rx();
            int pup_y = puppy_position->ry();
            int bone_x = bone_position->rx();
            int bone_y = bone_position->ry();
            int px = powerup_position->rx();
            int py = powerup_position->ry();
            labels[py*num_lines + px]->clear();
            labels[py*num_lines + px]->clear();
            int nx = distribution(generator);
            int ny = distribution(generator);

            while (((nx == pup_x && ny == pup_y) || (nx == bone_x && ny == bone_y)) || (nx == 0 || nx == num_lines-1)) {
                nx = distribution(generator);
                ny = distribution(generator);
            }
            powerup_position->setX(nx);
            powerup_position->setY(ny);

        }

    QCoreApplication::processEvents();
    repaint();

}



/**
 * @brief checks if puppy is hit
 */

void GameBoard::checkPuppy() {
    if(!end_game) {
    // For every car, see if there is a puppy in the same spot
        for(size_t i = 0; i < num_cars; i++) {
            if(car_positions[i] == *puppy_position) {
                if(lives_remaining > 0) {
                    // Lose a life
                    lives_remaining--;
                    lives[lives_remaining]->clear();
                    hit_pos = new QPoint;
                    hit_pos->rx() = puppy_position->rx();
                    hit_pos->ry() = puppy_position->ry();
                    if_hit = true;
                    num_blood = 1;
                    puppy_position->rx() = 0;
                    puppy_position->ry() = 0;

                    QCoreApplication::processEvents();
                    repaint();
                    break;
                } else if(!end_game) {
                    end_game = true;

                    // print game over and level reached
                    QString end_level = level_value->text();
                    QString display = "Game over! You reached level " + end_level + "! Press ok and then new game to start again.";
                    QByteArray ba = display.toLatin1();
                    const char* c_str = ba.data();
                    QMessageBox::information(
                        this,
                        tr("Puppy Run"),
                        tr(c_str) );
                    puppy_position->rx() = 0;
                    puppy_position->ry() = 0;
                }
            }
        }
     // Checking if we got the bone
        if(*bone_position == *puppy_position && !end_game) {
            end_game = true;
            QMessageBox::information(
                this,
                tr("Puppy Run"),
                tr("You win! Press ok to go to the next level.") );
            puppy_position->rx() = 0;
            puppy_position->ry() = 0;
            bone_position->rx() = num_lines - 1;
            bone_position->ry() = num_lines - 1;
            end_game = false;

            // updates level
            int level_num = (level_value->text()).toInt();
            level_num++;
            level_value->setText(QString::number(level_num));

        }
     // checking if we got a power up
        if(*powerup_position == *puppy_position && !end_game && lives_remaining < 3) {

            ++lives_remaining;
            lives[lives_remaining-1]->setPixmap(*puppy_image);
            lives[lives_remaining-1]->setMinimumSize(65,65);
            lives[lives_remaining-1]->setMaximumSize(65,65);
            lives[lives_remaining-1]->setScaledContents(true);

            update_powerup();

        }
    }
}

/**
 * @brief sets end_game to true
 */
void GameBoard::endGame() {
    end_game = true;
}
