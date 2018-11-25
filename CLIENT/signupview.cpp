#include "signupview.h"
#include "ui_signupview.h"
#include <QMessageBox>

SIGNUPVIEW::SIGNUPVIEW(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::SIGNUPVIEW)
{
    ui->setupUi(this);
    this->mainWin=parent;
}

SIGNUPVIEW::~SIGNUPVIEW()
{
    delete ui;
}


void SIGNUPVIEW::on_signUp_clicked()
{
    if(ui->rePsd->text()!=ui->passWord->text()){
        QMessageBox::information(this,"HINT","Please enter the correct password",QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    mainWin->SignUp(ui->userName->text(),ui->passWord->text(),ui->rePsd->text());
}
