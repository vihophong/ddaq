#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TGraph.h"
#include "TQtWidget.h"
//#include "frame.h"
#include "TCanvas.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    connect(this, SIGNAL(endOfReadOutCycle()), this, SLOT(readoutCycle()),Qt::QueuedConnection);
    /*
    TQtWidget *MyWidget= new TQtWidget(0,"MyWidget");
    MyWidget->show();
    MyWidget->GetCanvas()->cd();
    TGraph *mygraph;
    float x[3] = {1,2,3};
    float y[3] = {1.5, 3.0, 4.5};
    mygraph  = new TGraph(3,x,y);
    mygraph->SetMarkerStyle(20);
    mygraph->Draw("AP");
    MyWidget->GetCanvas()->Update();
    */
    ui->widget->show();
    ui->widget->GetCanvas()->Divide(2,1);
    ui->widget->GetCanvas()->cd(1);
    TGraph *mygraph;
    float x[3] = {1,2,3};
    float y[3] = {1.5, 3.0, 4.5};
    mygraph  = new TGraph(3,x,y);
    mygraph->SetMarkerStyle(20);
    mygraph->Draw("AP");
    ui->widget->GetCanvas()->cd(2);
    TGraph *mygraph2;
    float x1[3] = {1,2,3};
    float y1[3] = {1.5, 3.0, 2};
    mygraph2  = new TGraph(3,x1,y1);
    mygraph2->SetMarkerStyle(20);
    mygraph2->Draw("AP");
    ui->widget->GetCanvas()->Update();
    emit endOfReadOutCycle();
}
void MainWindow::readoutCycle()
{

    emit endOfReadOutCycle(); //Emit end of readout cycle signal
}
