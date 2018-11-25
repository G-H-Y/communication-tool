#include "progressbarview.h"
#include "ui_progressbarview.h"

PROGRESSBARVIEW::PROGRESSBARVIEW(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PROGRESSBARVIEW)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

PROGRESSBARVIEW::~PROGRESSBARVIEW()
{
    delete ui;
}

void PROGRESSBARVIEW::SetProgress(int progress){
    ui->progressBar->setValue(progress);
}

void PROGRESSBARVIEW::SetText(const QString &editText){
    ui->label->setText(editText);
}

