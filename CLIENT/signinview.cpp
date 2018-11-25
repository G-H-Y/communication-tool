#include "signinview.h"
#include "ui_signinview.h"

SIGNINVIEW::~SIGNINVIEW()
{
    delete ui;
}

SIGNINVIEW::SIGNINVIEW(MainWindow *parent):
    QDialog(parent),
    ui(new Ui::SIGNINVIEW)
{
    ui->setupUi(this);
    this->mainWin=parent;
    connect(parent,SIGNAL(SignInSucceed()),this,SLOT(close()));
}


void SIGNINVIEW::on_SignInBtn_clicked()
{
    mainWin->SignIn(ui->userName->text(),ui->passWord->text());
}
