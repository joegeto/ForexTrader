#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>

#include "bid.h"

namespace Ui {
class mainwindow;
}

class mainwindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit mainwindow(QWidget *parent = 0);
    ~mainwindow();
    
private slots:
    int find_date(QDate);
    int find_first_date(QDate);
    int find_last_date(QDate);

    void on_select_file_clicked();

    void on_start_learn_clicked();

    void on_start_trade_clicked();

    void on_start_all_clicked();


private:
    Ui::mainwindow *ui;
    QVector<bid*> vector;
    double** mas_ao;

};

#endif // MAINWINDOW_H
