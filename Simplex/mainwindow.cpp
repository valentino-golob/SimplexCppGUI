#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include<QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Lineare Optimierung");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_OpenEingabe_clicked()
{

    int nParam = ui->l_NumberParam->text().toInt();
    int nUNB   = ui->l_NumberUNB->text().toInt();
    int nGNB   = ui->l_NumberGNB->text().toInt();
    hide();
    Dialog inputData;
    inputData.setModal(true); //modal approach
    inputData.inputSetupGUI(nParam, nUNB, nGNB);
    inputData.exec();
}
