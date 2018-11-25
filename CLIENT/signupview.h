#ifndef SIGNUPVIEW_H
#define SIGNUPVIEW_H

#include <QDialog>
#include "mainwindow.h"
namespace Ui {
class SIGNUPVIEW;
}

class SIGNUPVIEW : public QDialog
{
    Q_OBJECT

public:
    explicit SIGNUPVIEW(MainWindow *parent = 0);
    ~SIGNUPVIEW();

private slots:
    void on_signUp_clicked();

private:
    Ui::SIGNUPVIEW *ui;
    MainWindow *mainWin;
};

#endif // SIGNUPVIEW_H
