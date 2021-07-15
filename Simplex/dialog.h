#ifndef DIALOG_H
#define DIALOG_H

#include "Simplex.h"
#include "dialogsol.h"
#include <iostream>
#include <vector>
#include <variant>
#include <string>

#include <QDialog>
#include <cstdlib>
#include <string>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QString>
#include <QLayout>
#include <QGroupBox>
#include <QVector>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void inputSetupGUI(int numberParam, int numberUNB, int numberGNB);
    void createHorizontalGroupBox();
    void createGridGroupBox();
    void createButton();
    void initSimplexData();

private:
    Ui::Dialog *ui;
    QGroupBox *horizontalGroupBox;
    QGroupBox *gridGroupBox;
    int numberParam;
    int numberUNB;
    int numberGNB;
    QList<QLabel*> fieldLabelParam;
    QList<QLineEdit*> fieldLineParam;
    QList<QLabel*> fieldLabelUNB;
    QList<QLineEdit*> fieldLineUNB;
    QList<QLabel*> fieldLabelGNB;
    QList<QLineEdit*> fieldLineGNB;
    QList<QLineEdit*> fieldLineB;
    QList<QLineEdit*> fieldLineBGNB;
    QDialogButtonBox *buttonBox;

};

#endif // DIALOG_H
