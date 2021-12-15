#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cstdlib"
#include "ctime"
#include "QVector"
#include "windows.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Pseudogradient estimation of quantiles and threshold");
    bar_chart_1 = new QCPBars(ui->widget->xAxis, ui->widget->yAxis);
    bar_chart_2 = new QCPBars(ui->widget_2->xAxis, ui->widget_2->yAxis);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<double> array_x(20), array_sort(20), array_range(20), array_frequency_sum(20), array_frequency(20);
double quantile = 0;

void MainWindow::on_pushButton_clicked()
{
   srand(time(0));
   for(int i=0;i<array_x.size();i++)
   {
       array_x[i]=0.01*(rand()%1000);
       array_sort[i]=array_x[i];
   }
   std::sort(array_sort.begin(),array_sort.end());

   for(int i=0;i<array_frequency.size();i++)
   {
        array_range[i]=array_frequency_sum[i]=array_frequency[i]=0;
   }

    double increment=(array_sort[0]+array_sort[array_sort.size()-1])/(array_range.size()/2.0);
    array_range[0]=array_sort[0];
    for(int i=1;i<array_range.size();i++)
    {
        if(i%2!=0) array_range[i]=array_range[i-1]+increment;
        if(i%2==0) array_range[i]+=array_range[i-1];
    }

    int n=0;
    for(int i=0;i<array_range.size();i+=2)
    {
        double temp=1.0;
        for(int j=0;j<array_sort.size();j++)
        {
            if((array_sort[j]>=array_range[i]) && (array_sort[j]<= array_range[i+1])) array_frequency[n]=temp++;
        }
        if(n!=20){
            n+=2;
            array_frequency[n-1]=array_frequency[n-2];
        }
    }

    for(int i=0;i<array_frequency.size();i++)
    {
        array_frequency[i]=array_frequency[i]/array_frequency.size();
    }

    array_frequency_sum[0]=array_frequency_sum[1]=array_frequency[0];
    for(int i=2;i<array_frequency_sum.size();i+=2)
    {
        array_frequency_sum[i]=array_frequency_sum[i+1]=array_frequency_sum[i-1]+array_frequency[i];
    }

    QVector<double> x((array_range.size()/2.0)), y((array_range.size()/2.0)), y2((array_range.size()/2.0));
    n=0;
    for(int i=0;i<x.size();i++)
    {
        x[i]=array_range[n];
        y[i]=array_frequency[n];
        y2[i]=array_frequency_sum[n];
        n+=2;
    }

    ui->label->setText("");
    ui->widget->xAxis->setLabel("Frequency histogram");
    ui->widget->clearGraphs();
    ui->widget->xAxis->setRange(array_range[0],array_range[array_range.size()-1]);

    ui->widget->yAxis->setRange(-0.1,1.1);
    bar_chart_1->data()->clear();
    bar_chart_1->setData(x,y);
    bar_chart_1->setWidth(increment);
    bar_chart_1->setBrush(QColor(135, 206, 235,150));
    bar_chart_1->setPen(QPen(QColor(135, 206, 235,0)));
    ui->widget->replot();

    ui->widget_2->xAxis->setLabel("Distribution function");
    ui->widget_2->clearGraphs();
    ui->widget_2->xAxis->setRange(array_range[0],array_range[array_range.size()-1]);
    ui->widget_2->yAxis->setRange(-0.1,1.1);
    bar_chart_2->data()->clear();
    bar_chart_2->setData(x,y2);
    bar_chart_2->setWidth(increment);
    bar_chart_2->setBrush(QColor(135, 206, 235,150));
    bar_chart_2->setPen(QPen(QColor(135, 206, 235,0)));
    ui->widget_2->replot();
}

void MainWindow::on_pushButton_2_clicked()
{
    QVector<double> arrX(2),arrY(2);
    arrY[0]=-0.1;
    arrY[1]=1.1;
    quantile=ui->doubleSpinBox->value();

    for(int i=0;i<array_frequency_sum.size();i+=2)
    {
        if(quantile>=array_frequency_sum[i]) arrX[0]=arrX[1]=(array_range[i]+array_range[i+1])/2.0;
        if(quantile==array_frequency_sum[i]) arrX[0]=arrX[1]=(array_range[i]+array_range[i-1])/2.0;
    }

    ui->label->setText("");
    ui->label->setText("Quantile value: " + QString::number(arrX[0]));
    ui->label->adjustSize();
    ui->widget->addGraph();
    ui->widget->graph(0)->setData(arrX,arrY);
    ui->widget->graph(0)->setPen(QPen(QColor(220, 20, 60)));
    ui->widget->replot();

    ui->widget_2->addGraph();
    ui->widget_2->graph(0)->setData(arrX,arrY);
    ui->widget_2->graph(0)->setPen(QPen(QColor(220, 20, 60)));
    ui->widget_2->replot();
}

void MainWindow::on_pushButton_3_clicked()
{
    double alfa=0;
    QVector<double> arrX(2),arrY(2);
    arrY[0]=-0.1;
    arrY[1]=1.1;
    double t=ui->doubleSpinBox_2->value();
    int itr=ui->spinBox->value();
    int n=0;
    QVector<double> nu(itr);

    double value_nu=(array_sort[0]+array_sort[array_sort.size()-1])/2.0;
    while (value_nu>0)
    {
        value_nu-=(((array_sort[0]+array_sort[array_sort.size()-1])/2.0)/itr);
        if(n==itr) break;
        nu[n]=value_nu;
        n++;
    }

    n=0;
    for(int i=0;i<itr;i++)
    {
        if(n==(array_x.size())) n=0;
        if(array_x[n]>=alfa) alfa+=nu[i]*quantile;
        if(array_x[n]<alfa) alfa-=nu[i]*(1-quantile);

        arrX[0]=arrX[1]=array_x[n];
        n++;
        ui->widget->addGraph();
        ui->widget->graph(1)->setData(arrX,arrY);
        ui->widget->graph(1)->setPen(QPen(QColor(0, 0, 0)));
        ui->widget->replot();
        ui->widget_2->addGraph();
        ui->widget_2->graph(1)->setData(arrX,arrY);
        ui->widget_2->graph(1)->setPen(QPen(QColor(0, 0, 0)));
        ui->widget_2->replot();

        arrX[0]=arrX[1]=alfa;
        QPen pen;
        pen.setColor(QColor(220, 20, 60));
        pen.setStyle(Qt::DashLine);
        ui->widget->addGraph();
        ui->widget->graph(2)->setData(arrX,arrY);
        ui->widget->graph(2)->setPen(pen);
        ui->widget->replot();
        ui->widget_2->addGraph();
        ui->widget_2->graph(2)->setData(arrX,arrY);
        ui->widget_2->graph(2)->setPen(pen);
        ui->widget_2->replot();

        Sleep(t*1000);
    }
}
