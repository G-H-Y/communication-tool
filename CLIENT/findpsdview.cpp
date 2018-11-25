#include "findpsdview.h"
#include "ui_findpsdview.h"

FINDPSDVIEW::FINDPSDVIEW(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::FINDPSDVIEW)
{
    ui->setupUi(this);
    this->mainWin=parent;
}

FINDPSDVIEW::~FINDPSDVIEW()
{
    delete ui;
}

void FINDPSDVIEW::on_findPsdBtn_clicked()
{
    mainWin->FindPsd(ui->userName->text(),ui->answer->text());
}
