#include "Simplex.h"

//Kostruktor mit Elementinitilaisierungsliste
Simplex::Simplex(vector<vector<double>> A, vector<double> c, vector<double> b) :A(A), c(c), b(b)//Konstruktor Elementinitialisierungsliste
{
    this->rows     = b.size();
    this->cols     = c.size();
    this->optim    = false;
    this->pivotRow = 0;
    this->pivotCol = 0;
    this->c_init   = init_c();
    this->solutionType = 1;

    basePrimParam = setPrimParam();
    setBasePrimParam();
}

//--------------------Getter Meberfunktionen-----------------------//
double Simplex::getA(int i, int j){ return A[i][j]; }

double Simplex::getb(int i){ return b[i]; }

double Simplex::getc(int j){ return c[j]; }

double Simplex::getcInit(int j){ return c_init[j]; }

double Simplex::getF(){ return F; }

int Simplex::getRows(){ return rows; }

int Simplex::getCols(){ return cols; }

int Simplex::getPivotRow(){ return pivotRow; }

int Simplex::getPivotCol(){ return pivotCol; }

int Simplex::getNumberStruct(){ return numberStruct; }

int Simplex::getSolutionType(){ return solutionType; }

vector<vector<std::variant<std::string, double>>> Simplex::getBasePrimParam(){ return basePrimParam; }

//--------------------Setter Memberfunktionen----------------------//

void Simplex::setPivotRow(int r){ this->pivotRow = r; }

void Simplex::setPivotCol(int c){ this->pivotCol = c; }

void Simplex::setSolutionType(int s){ this->solutionType = s; }

//----------------------Ausgabe------------------------------------//
//Tableau ausgeben
void Simplex::printTableau()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            cout << std::setw(7) << std::setprecision(3);
            cout << A[i][j] << " ";
        }
        cout << std::setw(7) << std::setprecision(3);
        cout << b[i];
        cout << endl;
    }
    for (int j = 0; j < cols; ++j)
    {
        cout << std::setw(7) << std::setprecision(3);
        cout << c[j] << " ";
    }

    cout << endl << endl;
}

//Basisloesung und Wert der Zielfunktion ausgeben
void Simplex::printBaseSol()
{
    if (isOptim())
        cout << "Optimale Basisloesung:" << endl;
    else if (isBaseValid())
        cout << "Zulaessige Basisloesung: " << endl;
    else
        cout << "Unzulaessige Basisloesung:" << endl;
    for (int j = 0; j < cols; j++)
    {
        if (std::get<std::string>(basePrimParam[j][2]) == "BV")
        {
            cout << std::get<std::string>(basePrimParam[j][0]) << ": ";
            cout << std::setprecision(3);
            cout << std::get<double>(basePrimParam[j][3]) << ", ";
        }
    }
    for (int j = 0; j < cols; j++)
    {
        if (std::get<std::string>(basePrimParam[j][2]) == "NBV")
        {
            cout << std::get<std::string>(basePrimParam[j][0]) << ": ";
            cout << std::setprecision(3);
            cout << std::get<double>(basePrimParam[j][3]) << ", ";
        }
    }
    {
        printf("F: %.3f", F);
        cout << endl << endl << endl;
    }
}

//-------------------------Hilfsfunktionen------------------------//
//Pruefen ob die Basisloesung optimal ist
bool Simplex::isOptim()
{
    for (int j = 0; j < cols; ++j)
    {
        if (c[j] < 0)
            return false;
    }
    this->optim = true;

    return true;
}

//ist eines der Elemente a_it der Pivotspalte t groeßer 0 --> a_it > 0
bool Simplex::isColPivotValid(int t)
{
    for (int i = 0; i < rows; ++i)
    {
        if (A[i][t] > 0)
            return true;
    }
    return false;
}

//Pivotzeile bestimmen
int Simplex::indexPivotRow()
{
    double tmp{ 0 }, min{ 0 }, index_min{ 0 }, index_tmp{ 0 };
    bool first{ true };
    for (int i = 0; i < rows; ++i)
    {
        if (A[i][pivotCol] > 0)
        {
            if (first)
            {
                min = (b[i] / A[i][pivotCol]);
                index_min = i;
                first = false;
            }
            tmp = (b[i] / A[i][pivotCol]);
            index_tmp = i;
            index_min = tmp < min ? index_tmp : index_min;
        }
    }
    return index_min;
}

//Pivotspalte bestimmmen
int Simplex::indexPivotCol()
{
    return  std::min_element(c.begin(), c.end()) - c.begin();
}

//Neue Basisloesung berechnen --> Transformation des Tableaus
void Simplex::pivotStep()
{

    //Dividiere die Pivotzeile s durch das Pivotelement a_st
    double a_st = A[pivotRow][pivotCol];
    for (int j = 0; j < cols; ++j)
    {
        A[pivotRow][j] = (A[pivotRow][j] / a_st);
    }
    b[pivotRow] = (b[pivotRow] / a_st);

    //Multipliziere für alle Zeilen i (außer s) die neue
    //Pivotzeile mit -a_it und addiere sie zur jeweiligen Zeile i hinzu.
    double a_it{ 0 };
    for (int i = 0; i < rows; ++i)
    {
        if (i != pivotRow)
        {
            a_it = A[i][pivotCol];
            for (int j = 0; j < cols; ++j)
            {
                A[i][j] = A[i][j] - a_it * A[pivotRow][j];
            }
            b[i] = b[i] - a_it * b[pivotRow];
        }
    }
    double c_col = c[pivotCol];
    for (int j = 0; j < cols; ++j)
        c[j] = c[j] - c_col * A[pivotRow][j];
}

//Initialisiert 2D-Vektor basePrimParam mit Informationen ueber die Variablen
vector<vector<std::variant<std::string, double>>> Simplex::setPrimParam()
{
    vector<vector<std::variant<std::string, double>>> tmp(cols, vector<std::variant<std::string, double>>(4, 0));

    int counter{0};
    for (int i = 0; i < cols; ++i)
    {
        //Variablennamen definieren
        tmp[i][0] = "x" + std::to_string(i + 1);

        //Strukturvariable oder Schlupfvariable
        if (c_init[i] != 0)
        {
            tmp[i][1] = "structure variable";
            counter++;
        }
        else
            tmp[i][1] = "slack variable";
    }
    this->numberStruct = counter;
    return tmp;
}

//Basisvariablen und Nichtbasisvariablen berechnen und Werte zuordnen, F berechnen. Die Basis des Tabeaus besitzt die Gestalt einer Einheitsmatrix
void Simplex::setBasePrimParam()
{
    for (int j = 0; j < cols; ++j)
    {
        //bei diesem Spezialfall besitzt unser Tableau eine kanonische Basis
        int counter_0{ 0 }, counter_1{ 0 }, index_1{ 0 };
        for (int i = 0; i < rows; ++i)
        {
            if (A[i][j] == 0)
                ++counter_0;
            else if (A[i][j] == 1)
            {
                ++counter_1;
                index_1 = i;
            }
        }
        if (counter_1 == 1 && counter_0 == (rows - 1))
            basePrimParam[j][2] = "BV";
        else
            basePrimParam[j][2] = "NBV";

        if (std::get<std::string>(basePrimParam[j][2]) == "BV")
            basePrimParam[j][3] = b[index_1];
        else
            basePrimParam[j][3] = 0;
    }
    F = evalF();
}

//Initialisiert c_init und wechselt macht einen Vorzeichenwechsel fuer Vektor c
vector<double> Simplex::init_c()
{
    vector<double> tmp(cols, 0);
    int counter{ 0 };
    for (std::vector<double>::iterator it = c.begin(); it != c.end(); ++it)
    {
        if (*it != 0)
        {
            tmp[counter] = *it;
            *it *= -1;
            ++counter;
        }
    }

    return tmp;
}

//Bestimme den Zielfunktionswert
double Simplex::evalF()
{
    double tmp{ 0 };

    for (int i = 0; i < cols; ++i)
    {
        if (std::get<std::string>(basePrimParam[i][1]) == "structure variable")
        {
            tmp = tmp + (std::get<double>(basePrimParam[i][3])*c_init[i]);
        }
    }

    return tmp;
}

//Abfrage ob die Basisloesung zulaessig ist
bool Simplex::isBaseValid()
{
    for (int i = 0; i < rows; ++i)
    {
        if (b[i] < 0)
            return false;
    }
    return true;
}

//(dualer Simplex) Wahl der Pivotzeile s, Zeile mit dem kleinsten b_s < 0 als Pivotzeile
int Simplex::indexPivotRowDual()
{
    return  std::min_element(b.begin(), b.end()) - b.begin();
}

bool Simplex::isRowPivotDoubleValid(int s)
{
    for (int j = 0; j < cols; ++j)
    {
        if (A[s][j] < 0)
            return true;
    }
    return false;
}

//Bestimmen der Pivotspalte t
int Simplex::indexPivotColDual()
{
    double tmp{ 0 }, max{ 0 }, index_max{ 0 }, index_tmp{ 0 };
    bool first{ true };
    for (int j = 0; j < cols; ++j)
    {
        if (A[pivotRow][j] < 0)
        {
            if (first)
            {
                max = (c[j] / A[pivotRow][j]);
                index_max = j;
                first = false;
            }
            tmp = (c[j] / A[pivotRow][j]);
            index_tmp = j;
            index_max = tmp > max ? index_tmp : index_max;
        }
    }
    return index_max;
}

