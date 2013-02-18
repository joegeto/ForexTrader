#include "mainwindow.h"
#include "ui_mainwindow.h"



#include "bid.h"

#include <QDebug>
#include <QDate>


double mp (double low,double high);
double sma(QVector<bid*> vec,int ind,int n);
double ao(QVector<bid*> vec,int ind);

mainwindow::mainwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwindow)
{
    ui->setupUi(this);





    bid a("2000.01.17,10:00,1.0122,1.0128,1.0114,1.0123,352");

    QDate date(2012,10,10);

    qDebug()<<date.toString("yyyy.MM.dd");

    for (int i=0;i<10;i++){
        date=date.addDays(-1);
        //qDebug()<<date.toString("yyyy.MM.dd");
    }

}

double mp (double low,double high){
    return (low+high)/2;
}
double sma(QVector<bid*> vec,int ind,int n){
    double sum=0;
    for (int i=ind-n;i<ind;i++){
        sum+=mp(vec[i]->low(),vec[i]->high());
    }
    return sum/n;
}
double ao(QVector<bid*> vec,int ind){
    return sma(vec,ind,34)-sma(vec,ind,5);
}


mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::on_select_file_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), ".", "");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << tr("Open file error.");
        return;
    }
    ui->select_file_label->setText("Selected file: "+fileName);
    while(!file.atEnd()){
        vector.append(new bid(file.readLine()));
    }

    file.close();

    mas_ao=(double**)calloc(vector.size(),sizeof(double*));
    for(int i=0;i<vector.size();i++){
        mas_ao[i]=(double*)calloc(4,sizeof(double));
    }


}


int mainwindow::find_date(QDate date){
    /* TODO: fix search if selected date not in vector */

        int t=vector.count()/2;
        int f=t;
        QDate _date;
        while(vector[f]->date() != date && !(t==2 && vector[f]->date() > date)) {
            t=t/2+1;
            if(vector[f]->date()<date)
                f+=t;
            else if(vector[f]->date()>date)
                f-=t;
            _date=vector[f]->date();
        }
        return f;
}

int mainwindow::find_first_date(QDate date) {
    int f=find_date(date);
    while(vector[f]->date() == date)
        f--;
    return ++f;
}
int mainwindow::find_last_date(QDate date) {
    int f=find_date(date);
    while(vector[f]->date() == date)
        f++;
    return --f;
}

void mainwindow::on_start_learn_clicked()
{

    /*//2000.02.12
    QDate dt(2006,12,23);
    qDebug()<<"Correct id:"<<43344<<"find date id:"<<find_date(dt)<<
              "find first date id"<<find_first_date(dt)<<"find last date id"<<find_last_date(dt);//*/

    int date_open=find_first_date(ui->trainFrom->date()),date_close=find_last_date(ui->trainTo->date());

    //нахождение всех необходимы коефициетвов осцилляторов для обучения
    for (int i=date_open;i<=date_close;i++){
        mas_ao[i][0]=ao(vector,i);
        mas_ao[i][1]=ao(vector,i-7);
        mas_ao[i][2]=ao(vector,i-14);
        mas_ao[i][3]=ao(vector,i-21);
    }

   double profit_max=0,w1_max=0,w2_max=0,w3_max=0,w4_max=0;
   //обучение(подбор весовых коефициентов, при которых профит максимальный)
   bool open=true,close=false;
   for (int i1=1;i1<=100;i1++){
       ui->progressBar->setValue(i1);
       for (int i2=1;i2<=100;i2++){
           for (int i3=1;i3<=100;i3++){
               for (int i4=1;i4<=100;i4++){
                   //c=0;
                   float profit=0;
                   open=true;
                   close=false;
                   for (int i=date_open;i<=date_close;i++){
                       double d=mas_ao[i][0]*(i1-100)+mas_ao[i][1]*(i2-100)+mas_ao[i][2]*(i3-100)+mas_ao[i][3]*(i4-100);
                       if (open){
                          if (d>0){
                             profit=profit-vector[i]->open();
                             open=false;
                             close=true;
                          }
                       }
                       if (close){
                          if (d<0){
                             profit=profit+vector[i]->close();
                             close=false;
                             open=true;
                          }
                       }
                       if ((close)&&(i==date_close)){
                           profit=profit+vector[i]->close();
                           close=false;
                           open=true;
                       }

                       if (profit>profit_max){
                          profit_max=profit;
                          w1_max=i1;
                          w2_max=i2;
                          w3_max=i3;
                          w4_max=i4;
                          ui->trainProfit->setText(QString::number(profit_max));
                          ui->trainW1->setText(QString::number(w1_max));
                          ui->trainW2->setText(QString::number(w2_max));
                          ui->trainW3->setText(QString::number(w3_max));
                          ui->trainW4->setText(QString::number(w4_max));
                       }
                   }
               }
           }
       }
   }//*/
   ui->trainProfit->setText(QString::number(profit_max));
   ui->trainW1->setText(QString::number(w1_max));
   ui->trainW2->setText(QString::number(w2_max));
   ui->trainW3->setText(QString::number(w3_max));
   ui->trainW4->setText(QString::number(w4_max));
}

void mainwindow::on_start_trade_clicked()
{
    int date_open=find_first_date(ui->tradeFrom->date()),date_close=find_last_date(ui->tradeTo->date());
    for (int i=date_open;i<=date_close;i++){
        mas_ao[i][0]=ao(vector,i);
        mas_ao[i][1]=ao(vector,i-7);
        mas_ao[i][2]=ao(vector,i-14);
        mas_ao[i][3]=ao(vector,i-21);
        //printf("a1= %f a2= %f a3= %f a4= %f i= %d\n",mas_ao[i][0],mas_ao[i][1],mas_ao[i][2],mas_ao[i][3],i);
    }
   //Торговля
   FILE* output_t=fopen("output_t.txt","w+");
   double profit=0,d=0;
   bool open=true,close=false;
   int c=0;
   fprintf(output_t,
           "==========================\nCoefficients: w1=%d w2=%d w3=%d w4=%d\n===========================\n",
           ui->tradeW1->text().toInt(),ui->tradeW2->text().toInt(),ui->tradeW3->text().toInt(),ui->tradeW4->text().toInt());
   for (int i=date_open;i<=date_close;i++){
                       d=mas_ao[i][0]*(100-ui->tradeW1->text().toInt())+mas_ao[i][1]*(100-ui->tradeW2->text().toInt())+
                               mas_ao[i][2]*(100-ui->tradeW3->text().toInt())+mas_ao[i][3]*(100-ui->tradeW4->text().toInt());
                       if (open==true){
                          if (d>0){
                             c++;
                             profit=profit-vector[i]->open();
                             open=false;
                             close=true;
                             fprintf(output_t,
                                     "--------------------------------------------\nDeal id= %d:\nOpen number in a file of quotations %d, date: %s %s, open price - %f profit= %f\n",
                                     c,i,vector[i]->date().toString("yyyy.MM.dd").toAscii().data(),vector[i]->time().toString("hh:mm").toAscii().data(),vector[i]->open(),profit);
                          }
                       }
                       if (close==true){
                          if (d<0){
                             profit=profit+vector[i]->close();
                             close=false;
                             open=true;
                             fprintf(output_t,
                                     "Close number in a file of quotations %d %s %s close price= %f profit= %f\n",
                                     i,vector[i]->date().toString("yyyy.MM.dd").toAscii().data(),vector[i]->time().toString("hh:mm").toAscii().data(),vector[i]->close(),profit);
                          }
                       }
                       if ((close==true)&&(i==date_close)){
                           profit=profit+vector[i]->close();
                           close=false;
                           open=true;
                           fprintf(output_t,
                                   "Close number in a file of quotations %d %s %s close price= %f profit= %f\n",
                                   i,vector[i]->date().toString("yyyy.MM.dd").toAscii().data(),vector[i]->time().toString("hh:mm").toAscii().data(),vector[i]->close(),profit);

                       }
   }
   fclose(output_t);
   ui->tradeProfit->setText(QString::number(profit));
}

void mainwindow::on_start_all_clicked()
{
    /*//2000.02.12
    QDate dt(2006,12,23);
    qDebug()<<"Correct id:"<<43344<<"find date id:"<<find_date(dt)<<
              "find first date id"<<find_first_date(dt)<<"find last date id"<<find_last_date(dt);//*/

    int date_open=find_first_date(ui->trainFrom->date()),date_close=find_last_date(ui->trainTo->date());

    //нахождение всех необходимы коефициетвов осцилляторов для обучения
    for (int i=date_open;i<=date_close;i++){
        mas_ao[i][0]=ao(vector,i);
        mas_ao[i][1]=ao(vector,i-7);
        mas_ao[i][2]=ao(vector,i-14);
        mas_ao[i][3]=ao(vector,i-21);
    }

   double profit_max=0,w1_max=0,w2_max=0,w3_max=0,w4_max=0;
   //обучение(подбор весовых коефициентов, при которых профит максимальный)
   bool open=true,close=false;
   for (int i1=1;i1<=100;i1++){
       ui->progressBar->setValue(i1);
       for (int i2=1;i2<=100;i2++){
           for (int i3=1;i3<=100;i3++){
               for (int i4=1;i4<=100;i4++){
                   //c=0;
                   float profit=0;
                   open=true;
                   close=false;
                   for (int i=date_open;i<=date_close;i++){
                       double d=mas_ao[i][0]*(i1-100)+mas_ao[i][1]*(i2-100)+mas_ao[i][2]*(i3-100)+mas_ao[i][3]*(i4-100);
                       if (open){
                          if (d>0){
                             profit=profit-vector[i]->open();
                             open=false;
                             close=true;
                          }
                       }
                       if (close){
                          if (d<0){
                             profit=profit+vector[i]->close();
                             close=false;
                             open=true;
                          }
                       }
                       if ((close)&&(i==date_close)){
                           profit=profit+vector[i]->close();
                           close=false;
                           open=true;
                       }

                       if (profit>profit_max){
                          profit_max=profit;
                          w1_max=i1;
                          w2_max=i2;
                          w3_max=i3;
                          w4_max=i4;
                          ui->trainProfit->setText(QString::number(profit_max));
                          ui->trainW1->setText(QString::number(w1_max));
                          ui->trainW2->setText(QString::number(w2_max));
                          ui->trainW3->setText(QString::number(w3_max));
                          ui->trainW4->setText(QString::number(w4_max));
                       }
                   }
               }
           }
       }
   }//*/
   ui->trainProfit->setText(QString::number(profit_max));
   ui->trainW1->setText(QString::number(w1_max));
   ui->trainW2->setText(QString::number(w2_max));
   ui->trainW3->setText(QString::number(w3_max));
   ui->trainW4->setText(QString::number(w4_max));


   ui->tradeW1->setText(QString::number(w1_max));
   ui->tradeW2->setText(QString::number(w2_max));
   ui->tradeW3->setText(QString::number(w3_max));
   ui->tradeW4->setText(QString::number(w4_max));



   date_open=find_first_date(ui->tradeFrom->date()),date_close=find_last_date(ui->tradeTo->date());
   for (int i=date_open;i<=date_close;i++){
       mas_ao[i][0]=ao(vector,i);
       mas_ao[i][1]=ao(vector,i-7);
       mas_ao[i][2]=ao(vector,i-14);
       mas_ao[i][3]=ao(vector,i-21);
       //printf("a1= %f a2= %f a3= %f a4= %f i= %d\n",mas_ao[i][0],mas_ao[i][1],mas_ao[i][2],mas_ao[i][3],i);
   }
  //Торговля
  FILE* output_t=fopen("output_t.txt","w+");
  double profit=0,d=0;
  open=true,close=false;
  int c=0;
  fprintf(output_t,
          "==========================\nCoefficients: w1=%d w2=%d w3=%d w4=%d\n===========================\n",
          ui->tradeW1->text().toInt(),ui->tradeW2->text().toInt(),ui->tradeW3->text().toInt(),ui->tradeW4->text().toInt());
  for (int i=date_open;i<=date_close;i++){
                      d=mas_ao[i][0]*(100-ui->tradeW1->text().toInt())+mas_ao[i][1]*(100-ui->tradeW2->text().toInt())+
                              mas_ao[i][2]*(100-ui->tradeW3->text().toInt())+mas_ao[i][3]*(100-ui->tradeW4->text().toInt());
                      if (open==true){
                         if (d>0){
                            c++;
                            profit=profit-vector[i]->open();
                            open=false;
                            close=true;
                            fprintf(output_t,
                                    "--------------------------------------------\nDeal id= %d:\nOpen number in a file of quotations %d, date: %s %s, open price - %f profit= %f\n",
                                    c,i,vector[i]->date().toString("yyyy.MM.dd").toAscii().data(),vector[i]->time().toString("hh:mm").toAscii().data(),vector[i]->open(),profit);
                         }
                      }
                      if (close==true){
                         if (d<0){
                            profit=profit+vector[i]->close();
                            close=false;
                            open=true;
                            fprintf(output_t,
                                    "Close number in a file of quotations %d %s %s close price= %f profit= %f\n",
                                    i,vector[i]->date().toString("yyyy.MM.dd").toAscii().data(),vector[i]->time().toString("hh:mm").toAscii().data(),vector[i]->close(),profit);
                         }
                      }
                      if ((close==true)&&(i==date_close)){
                          profit=profit+vector[i]->close();
                          close=false;
                          open=true;
                          fprintf(output_t,
                                  "Close number in a file of quotations %d %s %s close price= %f profit= %f\n",
                                  i,vector[i]->date().toString("yyyy.MM.dd").toAscii().data(),vector[i]->time().toString("hh:mm").toAscii().data(),vector[i]->close(),profit);

                      }
  }
  fclose(output_t);
  ui->tradeProfit->setText(QString::number(profit));

}
