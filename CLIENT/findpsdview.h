#ifndef FINDPSDVIEW_H
#define FINDPSDVIEW_H

#include <QDialog>
#include "mainwindow.h"
namespace Ui {
class FINDPSDVIEW;
}

class FINDPSDVIEW : public QDialog
{
    Q_OBJECT

public:
    explicit FINDPSDVIEW(MainWindow *parent = 0);
    ~FINDPSDVIEW();

private slots:
    void on_findPsdBtn_clicked();

private:
    Ui::FINDPSDVIEW *ui;
    MainWindow *mainWin;
};

#endif // FINDPSDVIEW_H
