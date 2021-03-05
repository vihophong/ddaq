#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isConnected=false;

    daq_t=new daq();
    if ((daq_t->shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1){
        daq_t->shmp = (struct SHM_DGTZ_S *)shmat(daq_t->shmid, 0, 0);
        ui->statusBar->showMessage(tr("Connected to the shared memory key # %1").arg(SHMKEY));
    }else{
        ui->statusBar->showMessage(tr("Not Connected!"));
    }
    record_length=daq_t->shmp->recordLength[0];// record length for board 0
    daq_t->shmp->ana_status=1;


    ui->widget->GetCanvas()->Divide(1,2);
    ui->widget->GetCanvas()->cd(1);
    h5 = new TH1F("h5","h5",400,100,50000);
    h5->Draw();
    ui->widget->GetCanvas()->cd(1)->Update();

    ui->widget->GetCanvas()->cd(2);
    h5_ch2 = new TH1F("h5_ch2","h5_ch2",400,100,50000);
    h5_ch2->Draw();
    ui->widget->GetCanvas()->cd(2)->Update();
    //ui->widget->Refresh();
    //draw wf window


    ui->widget_2->GetCanvas()->Divide(1,2);

    ui->widget_2->GetCanvas()->cd(1);
    //shortgate
    fa1 = new TF1("fa1",Form("(x>%d)*(x<%d)*2000",daq_t->shmp->preTrg[0][0]-daq_t->shmp->preGate[0][0],daq_t->shmp->preTrg[0][0]-daq_t->shmp->preGate[0][0]+daq_t->shmp->shortGate[0][0]),0,record_length);
    fa1->GetYaxis()->SetRangeUser(-1000,pow(2,14));
    fa1->SetLineWidth(0.5);
    //longgate
    fa2 = new TF1("fa2",Form("(x>%d)*(x<%d)*5000",daq_t->shmp->preTrg[0][0]-daq_t->shmp->preGate[0][0],daq_t->shmp->preTrg[0][0]-daq_t->shmp->preGate[0][0]+daq_t->shmp->longGate[0][0]),0,record_length);
    fa2->GetYaxis()->SetRangeUser(-1000,pow(2,16));
    fa2->SetLineWidth(0.5);
    fa2->SetLineColor(3);

    //waveform
    Double_t x[record_length];
    Double_t y[record_length];
    for (uint32_t i=0;i<record_length;i++){
        x[i]=(Double_t)i;
        y[i]=0;
    }
    grwf=new TGraph(record_length,x,y);
    //htemp = new TH1F("ht","ht",400,100,50000);
    //htemp->Draw();
    fa1->Draw();
    fa2->Draw("same");
    grwf->Draw("same");
    ui->widget_2->GetCanvas()->cd(1)->Update();

    ui->widget_2->GetCanvas()->cd(2);
    //waveform
    Double_t x2[record_length];
    Double_t y2[record_length];
    for (uint32_t i=0;i<record_length;i++){
        x2[i]=(Double_t)i;
        y2[i]=0;

    }
    grwf_ch2=new TGraph(record_length,x2,y2);
    //shortgate
    fa1_ch2 = new TF1("fa1_ch2",Form("(x>%d)*(x<%d)*2000",daq_t->shmp->preTrg[0][0]-daq_t->shmp->preGate[0][1],daq_t->shmp->preTrg[0][1]-daq_t->shmp->preGate[0][1]+daq_t->shmp->shortGate[0][1]),0,record_length);
    fa1_ch2->GetYaxis()->SetRangeUser(-1000,pow(2,14));
    fa1_ch2->SetLineWidth(0.5);
    //longgate
    fa2_ch2 = new TF1("fa2_ch2",Form("(x>%d)*(x<%d)*5000",daq_t->shmp->preTrg[0][0]-daq_t->shmp->preGate[0][1],daq_t->shmp->preTrg[0][1]-daq_t->shmp->preGate[0][1]+daq_t->shmp->longGate[0][1]),0,record_length);
    fa2_ch2->GetYaxis()->SetRangeUser(-1000,pow(2,16));
    fa2_ch2->SetLineWidth(0.5);
    fa2_ch2->SetLineColor(3);

    fa1_ch2->Draw();
    fa2_ch2->Draw("same");
    grwf_ch2->Draw("same");
    ui->widget_2->GetCanvas()->cd(2)->Update();
    //ui->widget_2->Refresh();

    evtn=0;
    evtn_ch2=0;
    fail_bit_flag=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    connect(this, SIGNAL(endOfReadOutCycle()), this, SLOT(readoutCycle()),Qt::QueuedConnection);
    evtn=0;
    evtn_ch2=0;
    fail_bit_flag=0;
    ui->statusBar->showMessage(tr("Started Monitoring!"),5000);
    emit endOfReadOutCycle();
}
void MainWindow::readoutCycle()
{
    if (daq_t->shmp->ana_flag==1){
        if (daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[0]==0&&daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[1]==0){ //select channel,board
            adcLong=(Double_t) daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[6];
            h5->Fill(adcLong);


            if (evtn%online_evt_wf==0&&evtn>0) {
                for(uint32_t i=0;i<record_length;i++){
                    Double_t ypoint=(Double_t)daq_t->shmp->buffer[daq_t->shmp->block_no].wfdata[i];
                    if (ypoint>pow(2,14)) fail_bit_flag=1;//check fail bit
                    grwf->SetPoint(i,(Double_t)i,ypoint);
                }
                if (fail_bit_flag==0){
                    ui->widget_2->GetCanvas()->cd(1);
                    grwf->Draw();
                    ui->widget_2->GetCanvas()->cd(1)->Update();
                    ui->widget_2->Refresh();
                }
                fail_bit_flag=0;
            }
            if (evtn%online_evt==0&&evtn>0) {
                ui->widget->GetCanvas()->cd(1);
                h5->Draw("hist");
                ui->widget->GetCanvas()->cd(1)->Update();
                //ui->widget->Refresh();
                printf("CH1- Number of event=%i - online event=%i\n",(uint32_t) daq_t->shmp->TrgCnt[0][0],evtn);
                //printf("%i\n",daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[10]);
            }
            evtn++;
            daq_t->shmp->ana_flag=0;
        }
        else if (daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[0]==0&&daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[1]==1){ //select channel,board
            adcLong=(Double_t) daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[6];
            h5_ch2->Fill(adcLong);

            if (evtn_ch2%online_evt_wf==0&&evtn_ch2>0) {
                for(uint32_t i=0;i<record_length;i++){
                    Double_t ypoint=(Double_t)daq_t->shmp->buffer[daq_t->shmp->block_no].wfdata[i];
                    if (ypoint>pow(2,14)) fail_bit_flag=1;//check fail bit
                    grwf_ch2->SetPoint(i,(Double_t)i,ypoint);
                }
                if (fail_bit_flag==0){
                    ui->widget_2->GetCanvas()->cd(2);
                    grwf_ch2->Draw();
                    ui->widget_2->GetCanvas()->cd(2)->Update();
                    ui->widget_2->Refresh();
                }
                fail_bit_flag=0;
            }
            if (evtn_ch2%online_evt==0&&evtn_ch2>0) {
                ui->widget->GetCanvas()->cd(2);
                h5_ch2->Draw("hist");
                ui->widget->GetCanvas()->cd(2)->Update();
                printf("CH2- Number of event=%i - online event=%i\n",(uint32_t) daq_t->shmp->TrgCnt[0][1],evtn_ch2);
                //printf("%i\n",daq_t->shmp->buffer[daq_t->shmp->block_no].dgtzdata[10]);
            }
            evtn_ch2++;
            daq_t->shmp->ana_flag=0;
        }

    }


    emit endOfReadOutCycle(); //Emit end of readout cycle signal
}

void MainWindow::on_pushButton_2_clicked()
{
    disconnect(this, SLOT(readoutCycle()));
    ui->statusBar->showMessage(tr("Stopped Monitoring!"),5000);
}

void MainWindow::on_pushButton_3_clicked()
{
    h5->Reset();
    h5_ch2->Reset();
}
