#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cstdlib"
#include "ctime"
#include "QVector"
#include "windows.h"

QVector<double> array_x(400), array_sort(400), array_range(20), array_frequency_sum(20), array_frequency(20);
double p_level, slowdown;
int itr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Pseudogradient estimation of quantiles and threshold");
    bar_chart_1 = new QCPBars(ui->widget->xAxis, ui->widget->yAxis);
    bar_chart_2 = new QCPBars(ui->widget_2->xAxis, ui->widget_2->yAxis);
    p_level=ui->doubleSpinBox->value();
    itr=ui->spinBox->value();
    slowdown=ui->doubleSpinBox_2->value();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    srand(time(0));
    double sum, temp;
        for (int j = 0; j < array_x.size(); j++)
        {
            sum = 0;
            for (int k = 0; k < 12; k++)
            {
                temp = rand() % 100;
                sum = sum + temp / 100.0;
             }
             array_x[j] = sum - 6.0;
             array_sort[j]=array_x[j];
        }
    std::sort(array_sort.begin(),array_sort.end());

    for(int i=0;i<array_range.size();i++)
    {
         array_range[i]=0;
         array_frequency_sum[i]=0;
         array_frequency[i]=0;
    }

     double increment=(abs(array_sort[0])+array_sort[array_sort.size()-1])/(array_range.size()/2);
     array_range[0]=array_sort[0];
     for(int i=1;i<array_range.size();i++)
     {
         if(i%2!=0) array_range[i]=array_range[i-1]+increment;
         if(i%2==0) array_range[i]=array_range[i-1];
     }

     for(int i=0;i<array_range.size();i+=2)
     {
         int temp=0;
         for(int j=0;j<array_x.size();j++)
         {
             if((array_sort[j]>=array_range[i]) && (array_sort[j]<= array_range[i+1])){
                 temp++;
             }
             array_frequency[i]=temp;
             array_frequency[i+1]=temp;
         }
     }


     for(int i=0;i<array_frequency.size();i++)
     {
         array_frequency[i]=array_frequency[i]/array_x.size();
     }

     array_frequency_sum[0]=array_frequency_sum[1]=array_frequency[0];
     for(int i=2;i<array_frequency_sum.size();i+=2)
     {
         array_frequency_sum[i]=array_frequency_sum[i-1]+array_frequency[i];
         array_frequency_sum[i+1]= array_frequency_sum[i];
     }

     QVector<double> x(10), y(10), y2(10);
     int n=0;
     for(int i=0;i<10;i++)
     {
         x[i]=array_range[n];
         y[i]=array_frequency[n];
         y2[i]=array_frequency_sum[n];
         n+=2;
     }

    ui->widget->clearGraphs();
    ui->widget->xAxis->setLabel("Frequency histogram");
    ui->widget->xAxis->setRange(array_range[0],array_range[array_range.size()-1]);
    ui->widget->yAxis->setRange(-(*std::max_element(array_frequency.begin(),array_frequency.end())/5), *std::max_element(array_frequency.begin(),array_frequency.end())*2);
    bar_chart_1->data()->clear();
    bar_chart_1->setData(x,y);
    bar_chart_1->setWidth(increment);
    bar_chart_1->setBrush(QColor(135, 206, 235,150));
    bar_chart_1->setPen(QPen(QColor(135, 206, 235,0)));
    ui->widget->replot();

    ui->widget_2->clearGraphs();
    ui->widget_2->xAxis->setLabel("Distribution function");
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
     p_level=ui->doubleSpinBox->value();
     QVector<double> x(2),y(2);
     y[0]=-0.1;
     y[1]=1.1;

     for(int i=0;i<array_frequency_sum.size();i+=2)
     {
         if(p_level>=array_frequency_sum[i]) x[0]=x[1]=(array_range[i]+array_range[i+1])/2.0;
         else if(p_level==array_frequency_sum[i]) x[0]=x[1]=(array_range[i]+array_range[i-1])/2.0;
         else if(p_level<array_frequency_sum[0]) x[0]=x[1]=array_range[0];
     }

     ui->label->setText("");
     ui->label->setText("");
     ui->label->setText("Quantile value: " + QString::number(x[0]));
     ui->label->adjustSize();
     ui->widget->addGraph();
     ui->widget->graph(0)->setData(x,y);
     ui->widget->graph(0)->setPen(QPen(QColor(220, 20, 60)));
     ui->widget->replot();
     ui->widget_2->addGraph();
     ui->widget_2->graph(0)->setData(x,y);
     ui->widget_2->graph(0)->setPen(QPen(QColor(220, 20, 60)));
     ui->widget_2->replot();
}

void MainWindow::on_pushButton_3_clicked()
{
    slowdown=ui->doubleSpinBox_2->value();
    QVector<double> x(2),y(2);
    y[0]=-0.1;
    y[1]=1.1;
    double alfa=array_x[0];
    QVector<double> nu(itr);
    double value_nu=0.5;

    for(int i=0;i<itr;i++)
    {
        nu[i]=value_nu;
        value_nu-=0.5/itr;
    }

    int k=1;
    for(int i=0;i<itr;i++)
    {
        if(k==array_x.size()) k=0;
        if(array_x[k]>=alfa) alfa+=nu[i]*p_level;
        if(array_x[k]<alfa) alfa-=nu[i]*(1-p_level);
        k++;
        x[0]=x[1]=array_x[i];
        ui->widget->addGraph();
        ui->widget->graph(1)->setData(x,y);
        ui->widget->graph(1)->setPen(QPen(QColor(0, 0, 0)));
        ui->widget->replot();
        ui->widget_2->addGraph();
        ui->widget_2->graph(1)->setData(x,y);
        ui->widget_2->graph(1)->setPen(QPen(QColor(0, 0, 0)));
        ui->widget_2->replot();

        x[0]=x[1]=alfa;
        QPen pen;
        pen.setColor(QColor(220, 20, 60));
        pen.setStyle(Qt::DashLine);
        ui->widget->addGraph();
        ui->widget->graph(2)->setData(x,y);
        ui->widget->graph(2)->setPen(pen);
        ui->widget->replot();
        ui->widget_2->addGraph();
        ui->widget_2->graph(2)->setData(x,y);
        ui->widget_2->graph(2)->setPen(pen);
        ui->widget_2->replot();
        Sleep(slowdown*1000);
    }
}
