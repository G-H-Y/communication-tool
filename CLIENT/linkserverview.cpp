#include "linkserverview.h"
#include "ui_linkserverview.h"

LINKSERVERVIEW::LINKSERVERVIEW(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::LINKSERVERVIEW)
{
    ui->setupUi(this);
    this->mainWin=parent;
}

LINKSERVERVIEW::~LINKSERVERVIEW()
{
    delete ui;
}

void LINKSERVERVIEW::on_linkServer_clicked()
{
    mainWin->ConnectServer(ui->serverIp->text(),ui->serverPort->text());
}
