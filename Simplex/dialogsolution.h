#ifndef DIALOGSOLUTION_H
#define DIALOGSOLUTION_H

#include <QDialog>

namespace Ui {
class DialogSolution;
}

class DialogSolution : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSolution(QWidget *parent = nullptr);
    ~DialogSolution();

private:
    Ui::DialogSolution *ui;
};

#endif // DIALOGSOLUTION_H
