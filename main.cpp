//
// main.cpp
//
//  Created by Victoria Wen on 10/20/15.
//  Copyright (c) 2015 Victoria Wen. All rights reserved.
//

/** @file main.cpp
@brief produces the application
@author Victoria Wen
@date October 20, 2015
*/

#include "mainwindow.h"
#include <QApplication>

/**
 * @brief main function to produce application
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 * @return passes control of program to Qt's event handler
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Puppy Run");
    w.show();

    return a.exec();
}
