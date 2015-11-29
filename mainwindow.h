//
// mainwindow.h
//
//  Created by Victoria Wen on 10/20/15.
//  Copyright (c) 2015 Victoria Wen. All rights reserved.
//

/** @file mainwindow.h
@brief declares the MainWindow class
@author Victoria Wen
@date October 20, 2015
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gameboard.h"

#include <QMainWindow>
#include <QStackedWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void instructions();

    void easy_game_begin();
    void medium_game_begin();
    void hard_game_begin();

    void game_over();


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    GameBoard* board;
    QStackedWidget* stack;
};

#endif // MAINWINDOW_H
