#include "todgroupbox.h"
#include "ui_todgroupbox.h"

TodGroupBox::TodGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::TodGroupBox)
{
    ui->setupUi(this);
}

TodGroupBox::~TodGroupBox()
{
    delete ui;
}
