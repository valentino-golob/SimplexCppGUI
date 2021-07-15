#include "dialogsolution.h"
#include "ui_dialogsolution.h"

DialogSolution::DialogSolution(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSolution)
{
    ui->setupUi(this);
}

DialogSolution::~DialogSolution()
{
    delete ui;
}
