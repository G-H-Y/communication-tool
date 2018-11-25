#ifndef SIGNINVIEW_H
#define SIGNINVIEW_H

#include <QDialog>
#include "mainwindow.h"
namespace Ui {
class SIGNINVIEW;
}

class SIGNINVIEW : public QDialog
{
    Q_OBJECT

public:
    explicit SIGNINVIEW(MainWindow *parent=0);
    ~SIGNINVIEW();
private slots:
    void on_SignInBtn_clicked();

private:
    Ui::SIGNINVIEW *ui;
    MainWindow *mainWin;
};

#endif // SIGNINVIEW_H
