#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <variant>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

class Simplex {
private:
    int rows;														//Anzahl Zeilen
    int cols;														//Anzahl Spalten
    int pivotRow;													//Pivotzeile
    int pivotCol;												    //Pivotspalte
    int numberStruct;                                               //Anzahl an Strukturvariablen
    int solutionType;                                               //Art der Loesung: 1=Loesung ist optimal, 2=Problem ist unbeschraenkt, 3=Problem hat keine zulaessige Basisloesung
    double F;														//Zielfunktionswert
    bool optim;														//Kennzeichnet ob die Basisloesung optimal ist
    vector<vector<double>> A;										//Koeffizientenmatrix
    vector<double> c;												//Zielfunktionskoeffizienten
    vector<double> c_init;											//Zielfunktionskoeffizienten Vektor bleibt ueber alle Berechnungen unveraendert
    vector<double> b;												//Kapazitaetenvektor
    vector<vector<std::variant<std::string, double>>> basePrimParam;//Vector mit BV, NBV, und ihren Werten

public:
    Simplex(vector<vector<double>> A, vector<double> c, vector<double> b); // Konstruktor
    ~Simplex() {};	//Destruktor
    //Getter Meberfunktionen
    double getA(int i, int j);		    //Element der Matrix A  (
    double getb(int i);                 //Element des Vektors b (Kapazitaetenvector)
    double getc(int j);                 //Element des Vektors c (Zielfunktionskoeffizient)
    double getcInit(int j);             //Element des Vektors c_init (Zielfunktion)
    double getF();                      //maximaler Funktionswert
    int getRows();                      //Anzahl der Zeilen
    int getCols();                      //Anzhal der Spalten
    int getPivotRow();                  //Pivotzeile
    int getPivotCol();                  //Pivotspalte
    int getNumberStruct();              //Anzahl der Strukturvariablen
    int getSolutionType();              //Art der Loesung
    vector<vector<std::variant<std::string, double>>> getBasePrimParam(); //Vector mit BV, NBV, und ihren Werten

    //Setter Memberfunktionen
    void setPivotRow(int r);
    void setPivotCol(int c);
    void setSolutionType(int s);

    //Print
    void printTableau();							//Koeffizientenmatrix ausgeben
    void printBaseSol();						//Basisloesung und Wert der Zielfunktion ausgeben

    //Hilfsfunktionen
    bool isOptim();							   	//Pruefen ob Basisloesung optimal
    bool isColPivotValid(int t);				//ist eines der Elemente a_it der Pivotspalte t groeßer 0 --> a_it > 0
    int indexPivotRow();						//Index Pivotzeile s, primaler Simplex
    int indexPivotCol();						//Index Pivotspalte t, primaler Simplex
    void pivotStep();				         	//Brechnung der neuen Basisloesung
    vector<vector<std::variant<std::string, double>>> setPrimParam(); //Initialisiert 2D-Vektor basePrimParam mit Informationen ueber die Variablen
    void setBasePrimParam();					//Basisvariablen und Nichtbasisvariablen berechen und Werte zuordnen --> basePrimParam, und F berechen
    vector<double> init_c();					//initialisiere den Zielfunktionskoeffizienten Vektor
    double evalF();						     	//Bestimme den Zielfuntionswert
    bool isBaseValid();							//Abfrage ob die Basisloesung zulaessig ist
    int indexPivotRowDual();					//Index Pivotzeile s, dualer Simplex
    int indexPivotColDual();					//Index Pivotspalte t, dualer Simplex
    bool isRowPivotDoubleValid(int s);		    //ist eines der Elemente a_sj der Pivotzeile s groeßer 0 --> a_sj > 0



};
