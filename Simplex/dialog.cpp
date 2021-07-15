#include "dialog.h"
#include "ui_dialog.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags(Qt::WindowTitleHint);
    this->setWindowFlags(flags);
    this->setWindowTitle("LP-Modell definieren");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::inputSetupGUI(int numberParam, int numberUNB, int numberGNB)
{
    this->numberParam = numberParam;
    this->numberUNB   = numberUNB;
    this->numberGNB   = numberGNB;

    // Fehlerabfrage beim Aufruf
    if(numberParam < 2)
    {
        QMessageBox msgBox;
        QPushButton *connectButton = msgBox.addButton(tr("Beenden"), QMessageBox::DestructiveRole);
        msgBox.setWindowTitle("Fehlerhafte Eingabe");
        msgBox.setText("Bitte geben Sie mindestens 2 zu optimierende Parameter ein!");
        msgBox.setInformativeText("Das Programm wird beendet.");
        msgBox.exec();
        if(msgBox.clickedButton() == connectButton)
            QApplication::quit();
    }
    else if(numberGNB == 0 && numberUNB == 0)
    {
        QMessageBox msgBox;
        QPushButton *connectButton = msgBox.addButton(tr("Beenden"), QMessageBox::DestructiveRole);
        msgBox.setWindowTitle("Fehlerhafte Eingabe");
        msgBox.setText("Bitte geben Sie mindestens eine Nebenbedingung an!");
        msgBox.setInformativeText("Das Programm wird beendet.");
        msgBox.exec();
        if(msgBox.clickedButton() == connectButton)
            QApplication::quit();
    }

    QPushButton *button = new QPushButton("Übernehmen",this);
    connect(button, &QPushButton::clicked,[=](){
        initSimplexData();
    });
    createHorizontalGroupBox();
    createGridGroupBox();

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(horizontalGroupBox);
    mainLayout->addWidget(gridGroupBox);
    mainLayout->addWidget(button);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(mainLayout);
}

void Dialog::createHorizontalGroupBox()
{
    horizontalGroupBox = new QGroupBox(tr("Zu maximierende Zielfunktion"));
    QHBoxLayout *layout = new QHBoxLayout;

    //dynamische Ausgabe: max F(x1,..., xn) = ....
    std::string stringMaxF = "max F(";
    for(int i = 1; i <= numberParam; ++i)
    {
        if(i < numberParam)
            stringMaxF += "x" + std::to_string(i) + ",";
        else
            stringMaxF += "x" + std::to_string(i) + ") = ";
    }
    QLabel * labelMaxF = new QLabel(QString::fromStdString(stringMaxF), this);
    layout->addWidget(labelMaxF);

    std::string xparam{""};
    for(int i = 0; i < numberParam; ++i)
    {
        fieldLineParam.append(new QLineEdit(this));
        layout->addWidget(fieldLineParam.at(i));

        xparam = "x" + std::to_string(i+1);
        fieldLabelParam.append(new QLabel(this));
        fieldLabelParam.at(i)->setText(QString::fromStdString(xparam));
        layout->addWidget(fieldLabelParam.at(i));

        QLabel* plusoperator = new QLabel("+", this);
        if(i < (numberParam-1))
        {
            layout->addWidget(plusoperator);
        }
    }
    horizontalGroupBox->setLayout(layout);
}

void Dialog::createGridGroupBox()
{
    int distance = 0;
    int tmp_j = 1;
    gridGroupBox = new QGroupBox(tr("Nebenbedingungen"));
    QGridLayout *layout = new QGridLayout;

    int counterUNB{0};
    //Ungleichungsnebenbedingungen
    for(int i = 0; i < numberUNB; ++i)
    {
        counterUNB++;
        tmp_j--;
        distance = 0;
        for(int j = 0; j < numberParam; j++)
        {
            fieldLineUNB.append(new QLineEdit(this));
            //Zeichen x1,..., xn
            fieldLabelUNB.append(new QLabel(this));
            std::string str = "x" + std::to_string(j+1);
            fieldLabelUNB.at(i+tmp_j)->setText(QString::fromStdString(str));

            layout->addWidget(fieldLineUNB.at(i+tmp_j),i,0+distance);
            layout->addWidget(fieldLabelUNB.at(i+tmp_j),i,1+distance);

            //+ Symbol
            QLabel* plusOperator = new QLabel("+", this);
            if(j < (numberParam-1))
            {
               layout->addWidget(plusOperator,i,2+distance);
            }
            distance += 3;
            tmp_j++;
        }
        //Operator Symbol <=
        QLabel* leqOperator = new QLabel("<=", this);
        layout->addWidget(leqOperator, i, distance);

        //Kapazitaetenvektor
        fieldLineB.append(new QLineEdit(this));
        layout->addWidget(fieldLineB.at(i), i, distance + 1);
    }

    tmp_j = 1;
    int counter = counterUNB;
    //Gleichungnebenbedingungen
    for(int i = 0; i < numberGNB; ++i)
    {
        counter++;
        tmp_j--;
        distance = 0;
        for(int j = 0; j < numberParam; j++)
        {
            fieldLineGNB.append(new QLineEdit(this));
            //Zeichen x1,..., xn
            fieldLabelGNB.append(new QLabel(this));
            std::string str = "x" + std::to_string(j+1);
            fieldLabelGNB.at(i+tmp_j)->setText(QString::fromStdString(str));

            layout->addWidget(fieldLineGNB.at(i+tmp_j),counter,0+distance);
            layout->addWidget(fieldLabelGNB.at(i+tmp_j),counter,1+distance);

            //+ Symbol
            QLabel* plusOperator = new QLabel("+", this);
            if(j < (numberParam-1))
            {
               layout->addWidget(plusOperator,counter,2+distance);
            }
            distance += 3;
            tmp_j++;
        }
        //Operator Symbol =
        QLabel* eqOperator = new QLabel("=", this);
        layout->addWidget(eqOperator, counter, distance);

        //Kapazitaetenvektor
        fieldLineBGNB.append(new QLineEdit(this));
        layout->addWidget(fieldLineBGNB.at(i), counter, distance + 1);
    }

gridGroupBox->setLayout(layout);
}

void Dialog::initSimplexData()
{
    int colSizeA = (numberUNB + (2*numberGNB) + numberParam);
    int rowSizeA = numberUNB+(2*numberGNB);

    //Vektor der Koeffizeientenmatrix befuellen
    std::vector<std::vector<double>> vec_A(rowSizeA, std::vector<double>(colSizeA, 0));
    int counter{0}, counter_i{0};
    for (int i = 0; i < numberUNB; ++i)
    {
        counter_i++;
        for (int j = 0; j < colSizeA; ++j)
        {
            if(j < numberParam)
            {
                vec_A[i][j] = fieldLineUNB.at(counter)->text().toDouble();
                counter++;
            }
            else if(j == (numberParam + i))
                vec_A[i][j] = 1;
            else
                vec_A[i][j] = 0;
        }
    }

    counter = 0;
    int tmpCounter{0}, counter2{0};
    bool flag{true};
    for(int i = 0; i < (2*numberGNB); i++)
    {
        counter2++;
        counter = tmpCounter;
        for (int j = 0; j < colSizeA; ++j)
        {
            if(j < numberParam)
            {
                if(flag)
                    vec_A[counter_i][j] = fieldLineGNB.at(counter)->text().toDouble();
                else
                    vec_A[counter_i][j] = (-1)*fieldLineGNB.at(counter)->text().toDouble();
                counter++;
            }
            else if(j == (numberParam + counter_i))
                vec_A[counter_i][j] = 1;
            else
                vec_A[counter_i][j] = 0;
        }
        counter_i++;
        flag = !flag;

        if(counter2 == 2)
        {
            counter2 = 0;
            tmpCounter = i+1;
        }
    }

    //Vektor der Zielfunktionskoeffizienten befuellen
    std::vector<double> vec_c(colSizeA,0);
    for(int j = 0; j < numberParam; ++j)
        vec_c[j] = fieldLineParam.at(j)->text().toDouble();

    //Kapazitaetenvektorbefuellen
    counter_i = 0;
    std::vector<double> vec_b(rowSizeA,0);
    for(int i = 0; i < numberUNB; ++i)
    {
        counter_i++;
        vec_b[i] = fieldLineB.at(i)->text().toDouble();
    }

    flag = true;
    counter2 = 0;
    counter = 0;
    tmpCounter = 0;
    for(int i = 0; i < (2*numberGNB); ++i)
    {
        counter2++;
        if(flag)
            vec_b[counter_i] = fieldLineBGNB.at(counter)->text().toDouble();
        else
            vec_b[counter_i] = (-1)*fieldLineBGNB.at(counter)->text().toDouble();
        if(counter2 == 2)
        {
            counter++;
            counter2 = 0;
        }

        counter_i++;
        flag = !flag;
    }
    hide();
    DialogSol evalData;
    evalData.setModal(true); //modal approach
    evalData.dispSol(vec_A, vec_c, vec_b, numberGNB);
    evalData.exec();

}














