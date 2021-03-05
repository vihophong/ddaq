#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "TQtWidget.h"
#include"TH1.h"
#include"TH2.h"
#include"TGraph.h"
#include"TCanvas.h"
#include <TSystem.h>
#include <TStyle.h>
#include "daq.h"
#include "dpp.h"

#define online_evt_wf 10
#define online_evt 10

namespace Ui {
class MainWindow;
}
class daq;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    daq* daq_t;
private slots:
    void on_pushButton_clicked();
    void readoutCycle();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void endOfReadOutCycle();

private:
    Ui::MainWindow *ui;
    bool isConnected;
    uint32_t record_length;
    TH1F *h5;
    TF1 *fa1;
    TF1 *fa2;
    TGraph* grwf;
    TH1F *h5_ch2;
    TF1 *fa1_ch2;
    TF1 *fa2_ch2;
    TGraph* grwf_ch2;
    TH1F *htemp;

    Double_t adcLong;

    int evtn;
    int evtn_ch2;
    int fail_bit_flag;


};

#endif // MAINWINDOW_H
