#ifndef DIALOGSOL_H
#define DIALOGSOL_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QString>
#include <QDebug>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>

#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QValueAxis>
#include <QScatterSeries>
#include <QColor>
#include <QPen>

#include <Simplex.h>
#include <vector>
#include <string>
#include <variant>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <utility>
#include <cstdlib>
#include <cmath>

#include "callout.h"

namespace Ui {
class DialogSol;
}

class DialogSol : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSol(QWidget *parent = nullptr);
    ~DialogSol();
    void dispSol(std::vector<std::vector<double>> A, std::vector<double> c, std::vector<double> vec_b, int numberGNB);    //Darstellung der Loesung des Simplex
    void printTableau(Simplex &simplex);      //Tableau ausgeben
    void printBaseSol(Simplex &simplex);     //Basisloesungen ausgeben
    void printSolution(Simplex &simplex);                    //Loesung ausgeben. eventl. auch ob unzulaessige Basisloesung oder ob das Problem unbeschraenkt ist
    void initLayout(Simplex simplex);        //Layout fuer die Ausgabe erstellen
    void createHorizontalGroupBoxText(bool visual);     //Layout fuer Texeditor
    void createHorizontalGroupBoxGraph();    //Layout fuer Graph
    void initNonEqualConstraintLines(Simplex simplex);       //Nebenbedingungsgeraden einzeichen
    void baseSolChart(Simplex simplex);                      //Basisloesung in Chart einzeichnen
    void chartZielfunktion(Simplex &simplex);                //Zielfunktionsgerade in den Cahrt einzeichnen

private:
    Ui::DialogSol *ui;
    std::vector<std::vector<double>> vec_A;  //Vektor der Koeffizeientenmatrix
    std::vector<double> vec_c;               //Vektor der Zielfunktionskoeffizienten
    std::vector<double> vec_b;               //Kapazitaetenvektor
    int rows;                                //Anzahl an Zeilen
    int cols;                                //Anzahl an Spalten
    int numberStruct;                        //Anzahl an Strukturvariablen
    int numberGNB;                           //Anzahl der Gleichungsnebenbedingungen
    QGroupBox *horizontalGroupBoxText;
    QGroupBox *horizontalGroupBoxGraph;
    QPushButton* buttonEnd;                 //Desktopanwendung beenden
    QTextEdit* textEdit_Sol;                //Textedit Box
    QChart* chartSol;                       //Chart fuer visuelle Ausgabe in 2D
    QChartView* chartViewSol;               //Chartview fuer visuelle Ausgebe in 2D
    QValueAxis *axisX;
    QValueAxis *axisY;
    Callout *m_tooltip;
    double xMAX;
    double yMAX;
    bool crossedYAxis;

};

#endif // DIALOGSOL_H
