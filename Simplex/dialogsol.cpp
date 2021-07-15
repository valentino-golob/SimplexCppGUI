#include "dialogsol.h"
#include "ui_dialogsol.h"

DialogSol::DialogSol(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSol)
{
    ui->setupUi(this);
    Qt::WindowFlags flags(Qt::WindowTitleHint);
    this->setWindowFlags(flags);
    this->setWindowTitle("Lösung des LP-Modells");
    this->yMAX = 0;
    this->yMAX = 0;
    this->crossedYAxis = false;
}

DialogSol::~DialogSol()
{
    delete ui;
}

void DialogSol::dispSol(std::vector<std::vector<double> > A, std::vector<double> c, std::vector<double> b, int numberGNB)
{
    //Instanz simplex der Klasse Simple erstellen
    Simplex simplex(A, c, b);

    //rows := Anzahl der UNB; cols := Anzahl der Parameter, numberStruct := Anzahl an Strukturvariablen
    rows = simplex.getRows();
    cols = simplex.getCols();
    numberStruct = simplex.getNumberStruct();
    this->numberGNB = numberGNB;

    //Layout fuer Ausgabe erstellen
    initLayout(simplex);

    //BV, NBV und F setzen
    simplex.setBasePrimParam();
    //Tableau ausgeben
    printTableau(simplex);
    //BV, NBV und F ausgeben
    printBaseSol(simplex);
    //Basisloesung in Chart
    if(numberStruct == 2 && numberGNB == 0)
        baseSolChart(simplex);

    //Abfrage ob der Vektor b nicht negative Elemente auweist
    //--> nicht zulaessige Basisloesung -->dualer Siplex-Algorithmus
    //Wenn kein b nicht negative Elemente aufweist dann liegt bereits eine zulaessige Basisloesung vor
    while (!simplex.isBaseValid())
       {
           //Schritt 1:
           //Wahl der Pivotzeile s, Zeile mit dem kleinsten b_s < 0 als Pivotzeile
           simplex.setPivotRow(simplex.indexPivotRowDual());

           //Schritt 2:
           //Befindet sich in der Pivotzeile s kein a_sj < 0, so besitzt das Problem keine zulaessige Basisloesung --> Abbruch des Verfahrens
           if (!simplex.isRowPivotDoubleValid(simplex.getPivotRow()))
           {
               qDebug() << "Das Problem hat keine zulaessige Basisloesung";
               simplex.setSolutionType(3);
               printSolution(simplex);
               return;
           }

           //Pivotspalte bestimmen
           simplex.setPivotCol(simplex.indexPivotColDual());

           //Schritt 3:
           //Tableautransformation
           simplex.pivotStep();

           //BV, NBV und F setzen
           simplex.setBasePrimParam();
           //print Tableau
           printTableau(simplex);
           //BV, NBV und F ausgeben
           printBaseSol(simplex);
           //Basisloesung in Chart
           if(numberStruct == 2 && numberGNB == 0)
               baseSolChart(simplex);
       }

    //Den primalen Simplex Algorithmus solange wiederholen bis die Basisloesung optimal ist
    while (!simplex.isOptim())
    {
        //Schritt 1:
        //Suche die Spalte t mit dem kleinsten (negativen) Wert in Kapazitaetenvektor c
        //(stehen mehrere Spalten mit einem kleinstem Wert zur Auswahl, so waehlt der
        //Algorithmus std::min_element das Element mit dem kleineren index).
        //Die zugehörige Nichtbasisvariable xt wird neu in die Basis aufgenommen.
        //Die Spalte t ist die Pivospalte
        simplex.setPivotCol(simplex.indexPivotCol());

        //Schritt 2:
        //Sind in der Pivotspalte t alle a_it < 0, so kann für das betrachtete
        //Problem keine optimale Loesung angegeben werden
        // --> Abbruch des Verfahrens.
        if (!simplex.isColPivotValid(simplex.getPivotCol()))
        {
            qDebug() << "Das betrachtete Problem ist unbeschraenkt.";
            simplex.setSolutionType(2);
            printSolution(simplex);
            return;
        }
        //Pivotzeile bestimmen
        simplex.setPivotRow(simplex.indexPivotRow());

        //Schritt 3:
        //Berechnung der neuen Basisloesung, des neuen Simplex-Tableaus
        simplex.pivotStep();

        //BV, NBV und F setzen
        simplex.setBasePrimParam();
        //print Tableau
        printTableau(simplex);
        //BV, NBV und F ausgeben
        printBaseSol(simplex);
        //Basisloesung in Chart
        if(numberStruct == 2 && numberGNB == 0)
            baseSolChart(simplex);
    }

    //Loesung ausgeben
    printSolution(simplex);
    //Zielfungtionsgerade im Chart abbilden
    if(numberStruct == 2 && numberGNB == 0)
        chartZielfunktion(simplex);
}

//--------------------------Hilfsfunktionen----------------------------------//
//Ausgabe Tableau
void DialogSol::printTableau(Simplex &simplex)
{
    std::stringstream ss;
    for (int i = 0; i < rows; ++i)
    {
        ss.str("");
        for (int j = 0; j < cols; ++j)
        {
            ss << std::setw(12) << std::setprecision(3) << std::right  << simplex.getA(i,j);
        }
        ss << std::setw(12) << std::right << std::setprecision(3) << simplex.getb(i);
        textEdit_Sol->append(QString::fromStdString(ss.str()));
    }

    ss.str("");
    for (int j = 0; j < cols; ++j)
    {
        ss << std::setw(12) << std::right << std::setprecision(3) << simplex.getc(j);
    }
    ss << std::setw(12) << std::right << std::setprecision(3) << simplex.getF();

    textEdit_Sol->append(QString::fromStdString(ss.str()));
}

//Basisloesungen und den Wert der Zielfunktion ausgeben
void DialogSol::printBaseSol(Simplex &simplex)
{
    vector<vector<std::variant<std::string, double>>> baseParam = simplex.getBasePrimParam();
    std::stringstream ss;
    if (simplex.isOptim() && simplex.isBaseValid())
           ss << "Optimale Basislösung: ";
       else if (simplex.isBaseValid())
           ss << "Zulässige Basislösung: ";
       else
           ss << "Unzulässige Basislösung: ";

       for (int j = 0; j < cols; j++)
       {
           if (std::get<std::string>(baseParam[j][2]) == "BV")
           {
               ss <<  std::get<std::string>(baseParam[j][0]);
               ss << ": ";
               ss << std::setprecision(3) << std::get<double>(baseParam[j][3]);
               ss << ", ";
           }
       }
       for (int j = 0; j < cols; j++)
       {
           if (std::get<std::string>(baseParam[j][2]) == "NBV")
           {
               ss << std::get<std::string>(baseParam[j][0]);
               ss << ": ";
               ss << std::setprecision(3) << std::get<double>(baseParam[j][3]);
               ss << ", ";
           }
       }
       ss << "F: ";
       ss << std::setprecision(3) << simplex.getF();
       std::string result;
       std::getline(ss, result);
       textEdit_Sol->append(QString::fromStdString(result));
       textEdit_Sol->append("\n");
}

void DialogSol::printSolution(Simplex &simplex)
{
    vector<vector<std::variant<std::string, double>>> baseParam = simplex.getBasePrimParam();
    std::stringstream ss;
    std::string result;
    //Art des Problems abfragen
    switch (simplex.getSolutionType())
    {
    case 1:; //optimale Loesung des Problems
        for(int i = 0; i < numberStruct; ++i)
        {
            if(i == 0)
                ss << "Optimale Lösung: ";
            ss << "x" << std::to_string(i+1) << " = " << std::setprecision(3) << std::get<double>(baseParam[i][3]) << ", ";
        }
        ss << "F = " << simplex.getF();
        break;
    case 2:; //Das Problem ist unbeschraenkt
        ss << "Das betrachtete Problem ist unbeschränkt.";
        break;
    case 3: //Das Problem besitzt keine zulässige Basisloesung
        ss << "Das betrachtete Problem besitzt keine zulässige Basislösung.";
        break;
    }
    std::getline(ss, result);
    textEdit_Sol->append(QString::fromStdString(result));
}

void DialogSol::initLayout(Simplex simplex)
{
    bool visual{false};
    buttonEnd = new QPushButton("Programm beenden",this);
    connect(buttonEnd, &QPushButton::clicked,[=](){
        QCoreApplication::quit();
    });

    //Layout mit oder ohne 2D Graph initialisieren
    if(numberStruct == 2 && numberGNB == 0)
    {
        visual = true;
        createHorizontalGroupBoxText(visual);
        createHorizontalGroupBoxGraph();
        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addWidget(horizontalGroupBoxText);
        mainLayout->addWidget(horizontalGroupBoxGraph);
        mainLayout->addWidget(buttonEnd);

        setLayout(mainLayout);

        //Nebenbedingungsgeraden einzeichnen
        initNonEqualConstraintLines(simplex);
    }
    else
    {
        createHorizontalGroupBoxText(visual);
        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addWidget(horizontalGroupBoxText);
        mainLayout->addWidget(buttonEnd);

        setLayout(mainLayout);
    }
}

void DialogSol::createHorizontalGroupBoxText(bool visual)
{
    horizontalGroupBoxText = new QGroupBox(tr("Ausgabe"));
    QHBoxLayout *layout = new QHBoxLayout;
    textEdit_Sol = new QTextEdit(this);
    if(visual)
        textEdit_Sol->setMinimumHeight(300);
    layout->addWidget(textEdit_Sol);
    horizontalGroupBoxText->setLayout(layout);
}

void DialogSol::createHorizontalGroupBoxGraph()
{
    horizontalGroupBoxGraph = new QGroupBox(tr("2D-Plot"));
    QHBoxLayout *layout = new QHBoxLayout;
    chartSol = new QChart();
    chartViewSol = new QChartView(chartSol);
    chartViewSol->setRenderHint(QPainter::Antialiasing);
    chartViewSol->setMinimumSize(400,400);
    layout->addWidget(chartViewSol);
    horizontalGroupBoxGraph->setLayout(layout);
}

void DialogSol::initNonEqualConstraintLines(Simplex simplex)
{
    chartSol->legend()->hide();
    axisY = new QValueAxis();
    axisX = new QValueAxis();

    axisX->setTitleText("x1");
    axisY->setTitleText("x2");
    chartSol->addAxis(axisY, Qt::AlignLeft);
    chartSol->addAxis(axisX, Qt::AlignBottom);

    double x1{0}, x2{0}, b{0}, dt{0};
    double xMaxTmp{0}, yMaxTmp{0}, xMax{0}, yMax{0};
    bool breakWhile{false};
    for(int i = 0; i < rows; ++i)
    {
        breakWhile = false;
        xMax = 0;
        yMax = 0;
        dt = 0;
        x2 = simplex.getA(i,1);
        x1 = simplex.getA(i,0)/x2;
        b  = simplex.getb(i)/x2;

        if(x2 != 0)
        {
            QLineSeries *series = new QLineSeries();

            do
            {
                x2 = b - dt*x1;
                series->append(dt, x2);
                dt += 0.1;
                xMax = dt > xMax ? dt : xMax;
                yMax = x2 > yMax ? x2 : yMax;
                if(x2 < 0 && b >= 0)
                {
                    yMaxTmp = yMax > yMaxTmp ? yMax : yMaxTmp;
                    xMaxTmp = xMax > xMaxTmp ? xMax : xMaxTmp;
                    crossedYAxis = true;
                    breakWhile = true;
                }
                else if(dt > 10000 && crossedYAxis)
                {
                    xMax = xMaxTmp;
                    yMax = yMaxTmp;
                    breakWhile = true;
                }
                else if(dt > 10000)
                {
                    xMax = 10*x1;
                    yMax = 10*x1;
                    breakWhile = true;
                }
            }while(!breakWhile);

            chartSol->addSeries(series);
            series->attachAxis(axisX);
            series->attachAxis(axisY);

            std::string s;
            s = std::to_string(i+1) + ". NBG";
            chartSol->legend()->markers(series)[0]->setLabel(QString::fromStdString(s));

        }

    }
    xMAX = xMax;
    yMAX = yMax;
    axisX->setRange(0,xMax+2);
    axisY->setRange(0,yMax+2);
}

void DialogSol::baseSolChart(Simplex simplex)
{
    vector<vector<std::variant<std::string, double>>> baseParam = simplex.getBasePrimParam();
    QScatterSeries *series = new QScatterSeries();
    series->setMarkerSize(15);
    series->setColor(QColor(43,45,66,200));
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    double x1 = std::get<double>(baseParam[0][3]);
    double x2 = std::get<double>(baseParam[1][3]);

    series->append(x1,x2);
    chartSol->addSeries(series);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartSol->legend()->markers(series)[0]->setVisible(false);

    // Callout Funktion
    static int counter = 0;
    std::stringstream ss;
    std::string s;
    ss << ++counter << ". x1: " << std::setprecision(3) << x1 << ", x2: " <<  std::setprecision(3) << x2;
    std::getline(ss, s);

    QPointF point(x1,x2);
    m_tooltip = new Callout(chartSol);
    m_tooltip->setText(QString::fromStdString(s));

    m_tooltip->setAnchor(point);
    m_tooltip->updateGeometry();
    m_tooltip->show();

    // Legende anzeigen
    chartSol->legend()->setVisible(true);
    chartSol->legend()->setAlignment(Qt::AlignBottom);
}


void DialogSol::chartZielfunktion(Simplex &simplex)
{

    double dt{0}, xMax{0}, yMax{0};
    bool breakWhile{false};
    QLineSeries *series = new QLineSeries();

    double x2 = simplex.getcInit(1);
    double x1 = simplex.getcInit(0)/x2;
    double b = simplex.getF()/x2;

    do
    {
        x2 = b - dt*x1;
        series->append(dt, x2);
        dt += 0.1;
        xMax = dt > xMax ? dt : xMax;
        yMax = x2 > yMax ? x2 : yMax;
        if(x2 < 0 && b >= 0)
        {
            yMAX = yMax > yMAX ? yMax : yMAX;
            xMAX = xMax > xMAX ? xMax : xMAX;
            breakWhile = true;
        }
        else if(dt > 10000 && crossedYAxis)
        {
            breakWhile = true;
        }
        else if(dt > 10000)
        {
            xMAX = 10*x1;
            yMAX = 10*x1;
            breakWhile = true;
        }
    }while(!breakWhile);

    chartSol->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QPen pen = series->pen();
    pen.setStyle(Qt::DashDotLine);
    pen.setBrush(Qt::red);
    series->setPen(pen);

    chartSol->legend()->markers(series)[0]->setLabel(QString::fromStdString("Zielfunktion"));
    axisX->setRange(0,xMAX+2);
    axisY->setRange(0,yMAX+2);

}

