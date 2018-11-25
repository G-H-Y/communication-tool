#pragma once
#ifndef LINKSERVERVIEW_H
#define LINKSERVERVIEW_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class LINKSERVERVIEW;
}

class LINKSERVERVIEW : public QDialog
{
    Q_OBJECT

public:
    explicit LINKSERVERVIEW(MainWindow *parent = 0);
    ~LINKSERVERVIEW();

private slots:
    void on_linkServer_clicked();

private:
    Ui::LINKSERVERVIEW *ui;
    MainWindow *mainWin;
};

#endif // LINKSERVERVIEW_H
