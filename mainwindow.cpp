//
// mainwindow.cpp
//
//  Created by Victoria Wen on 10/20/15.
//  Copyright (c) 2015 Victoria Wen. All rights reserved.
//

/** @file mainwindow.cpp
@brief implements the MainWindow class
@author Victoria Wen
@date October 20, 2015
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameboard.h"
#include <QMessageBox>
/**
 * @brief MainWindow constructor
 * @param parent is the parent of the MainWindow
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

/**
  *@brief shows instructions
  */
void MainWindow::instructions() {
    QMessageBox::information(
        this,
        tr("Instructions"),
        tr("Use the arrow keys to move the puppy to the bone.<br />Don't get hit by a car, or you'll lose a life!<br /> If you've lost a life, getting a tennis ball will earn you back a life.") );
}

/**
 * @brief starts the easy game
 */
void MainWindow::easy_game_begin() {
    board = new GameBoard(this, 4, 300);
    this->setCentralWidget(board);
}

/**
 * @brief starts the medium game
 */
void MainWindow::medium_game_begin() {
    board = new GameBoard(this, 6, 200);
    this->setCentralWidget(board);
}

/**
 * @brief starts the hard game
 */
void MainWindow::hard_game_begin() {
    board = new GameBoard(this, 8, 100);
    this->setCentralWidget(board);
}

/**
 * @brief ends the game
 */
void MainWindow::game_over() {
    this->board->endGame();

    // Take the current board game out of the central widget of MainWindow
    QWidget* wid = this->centralWidget();
    wid->setParent(nullptr);

    // Reset the MainWindow with the initial startup screen
    ui->setupUi(this);
    this->setWindowTitle("Puppy Run");


}

/**
 * @brief MainWindow destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}
