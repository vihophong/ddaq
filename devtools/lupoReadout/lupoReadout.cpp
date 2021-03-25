#include "lupoReadout.h"
#include "ui_luporeadout.h"
#include "QFileDialog"
long get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

lupoReadout::lupoReadout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::lupoReadout)
{
    ui->setupUi(this);


    lupo=new lupoLib(0x111000);
    triggerCounter=0;
    saveFile=false;

    ti=0;


    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    acsii_flag=true;
}

lupoReadout::~lupoReadout()
{
    delete ui;
}

void lupoReadout::on_pushButton_clicked()
{
    connect(this, SIGNAL(endOfReadOutCycle()), this, SLOT(readoutCycle()),Qt::QueuedConnection);
    ui->label->setText("Aquiring data...");
    lupo->clearFIFO();
    //lupo->resetTimeStamp();
    lupo->clearTriggerCounter();

    ti=0;
    deadtimeC=0;
    triggerCounter=0;
    receivedTrigger=0;
    PrevRateTime = get_time();
    output="";
    //lupo->sendPulse(1);
    emit endOfReadOutCycle();
}
void lupoReadout::readoutCycle()
{

    CurrentTime = get_time();
    ElapsedTime = CurrentTime - PrevRateTime; // milliseconds
    if (ElapsedTime > 1000) {
      ti++;
      ui->lcdNumber->display(ti);
      PrevRateTime = CurrentTime;
      output=QString("Trigger Counter = %1\n").arg(triggerCounter);      
      output+=QString("Received Trigger = %1\n").arg(receivedTrigger);
      output+=QString("Dead Events = %1\n").arg(deadtimeC);
      output+=QString("Current timestamp = %1\n").arg(timestamp);
      /*
      QString s;
      s.sprintf("Current timestamp = 0x%llX", timestamp);
      output+=s;
      */
      ui->textEdit->setText(output);
    }

    unsigned int prev_TC;
    prev_TC=triggerCounter;
    triggerCounter=lupo->readTriggerCounter();
    if (triggerCounter>prev_TC){
        //unsigned long timeStampLupo=((unsigned long)lupo->readTimeStampMSB32())<<32|((unsigned long)lupo->readTimeStampLSB32());
        unsigned int BinHeader[3];
        size_t s;
        if ((triggerCounter-prev_TC)>1){
            for (unsigned int i=prev_TC+1;i<triggerCounter;i++){
                BinHeader[0] =i;//Number of sample
                BinHeader[1] = lupo->readTimeStampMSB32();
                BinHeader[2] = lupo->readTimeStampLSB32();
                //Write header and check for consistency                
                //For share memory segment
                shmp->trgCnt=triggerCounter;
                shmp->timestampMSB=BinHeader[1];
                shmp->timestampLSB=BinHeader[2];
                //

                if (saveFile==true&&acsii_flag==false){
                    s=fwrite(BinHeader,sizeof(unsigned int),3,fTSLupo);
                    fflush(fTSLupo);
                    if(s!=3){
                        output += QString("Error writing header, return %1\n").arg(s);
                        ui->textEdit->setText(output);
                    }
                }
                if (saveFile==true&&acsii_flag==true){
                    long long coarseTS=((unsigned long)BinHeader[1])<<32|((unsigned long)BinHeader[2]);
                    fprintf(fTSLupo,"%i %i %lli\n",triggerCounter,i,coarseTS);
                }

            }
            deadtimeC+=triggerCounter-prev_TC-1;
        }
        //

        BinHeader[0] =triggerCounter;//Number of sample
        BinHeader[1] = lupo->readTimeStampMSB32();
        BinHeader[2] = lupo->readTimeStampLSB32();
        //For share memory segment
        shmp->trgCnt=triggerCounter;
        shmp->timestampMSB=BinHeader[1];
        shmp->timestampLSB=BinHeader[2];
        timestamp=(long long) BinHeader[1]<<32 | BinHeader[2];

        //Write header and check for consistency
        if (saveFile==true&&acsii_flag==false){
            s=fwrite(BinHeader,sizeof(unsigned int),3,fTSLupo);
            fflush(fTSLupo);
            if(s!=3){
                output += QString("Error writing header, return %1\n").arg(s);
                ui->textEdit->setText(output);
            }
        }
        if (saveFile==true&&acsii_flag==true){
            long long coarseTS=((unsigned long)BinHeader[1])<<32|((unsigned long)BinHeader[2]);
            fprintf(fTSLupo,"%i %i %lli\n",triggerCounter,triggerCounter,coarseTS);
        }


        //fprintf(fTSLupo,"%lu %lu\n",triggerCounter,timeStampLupo); //1us
        lupo->clearFIFO();//1 us
        //lupo->sendPulse(1);
        receivedTrigger++;
    }
    emit endOfReadOutCycle();
}

void lupoReadout::on_pushButton_2_clicked()
{
    disconnect(this, SLOT(readoutCycle()));
    ui->label->setText("Stopped");
    output=QString("Trigger Counter = %1\n").arg(triggerCounter);
    output+=QString("Received Trigger = %1\n").arg(receivedTrigger);
    output+=QString("Dead Events = %1\n").arg(deadtimeC);
    ui->textEdit->setText(output);
}

void lupoReadout::on_pushButton_3_clicked()
{
    //also open share memory
    if ((shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1)
    {
         shmp = (struct SHM_DGTZ_S *)shmat(shmid, 0, 0);
         shmp->ana_flag=0;
         shmp->timestampMSB=0;
         shmp->timestampLSB=0;
         shmp->trgCnt=0;
         printf("Sucessfully attached to share memory with shmid=%i, size of %i\n-----------------\n",shmid,(int)sizeof(struct SHM_DGTZ_S));
    }else if ((shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), IPC_CREAT|0600)) != -1)
    {
        shmp = (struct SHM_DGTZ_S *)shmat(shmid, 0, 0);
        printf("Sucessfully created share memory with shmid=%i, size of %i\n-----------------\n",shmid,(int)sizeof(struct SHM_DGTZ_S));
    }
    shmp->pid=getpid();

    lupo->setLupoAddress(ui->lineEdit_2->text().toUInt(0,16));
    lupo->lupoConnect(ui->spinBox_4->value(),ui->spinBox_3->value());
    if (lupo->getConnectStatus()==1) ui->label_2->setText("Connected"); else ui->label_2->setText("Error!");
}

void lupoReadout::on_pushButton_4_clicked()
{
    release_nbbqvio(BHandle);
    ui->label_2->setText("Disconnected");
}

void lupoReadout::on_pushButton_5_clicked()
{
    lupo->sendPulse(ui->spinBox->value());
}

void lupoReadout::on_pushButton_6_clicked()
{
    dialog.exec();
   /*saveFileName= dialog.getSaveFileName(this,
        tr("Save Lupo Time Stamp"), "",
        tr("Binary file (*.nig);;All Files (*)"));*/
   saveDirectory = dialog.selectedFiles()[0];
   ui->lineEdit_3->setText(saveDirectory);
}


void lupoReadout::on_checkBox_toggled(bool checked) //binary
{    
    if (checked==true&&saveFile==false){
        saveFileName=QString("%1/run%2_lupoTS%3").arg(saveDirectory).arg(ui->spinBox_2->value()).arg(".nig");
        fTSLupo=fopen(saveFileName.toUtf8(),"wb");
        saveFile=true;
        acsii_flag=false;
    }else{
        ui->spinBox_2->setValue(ui->spinBox_2->value()+1);
        ui->spinBox_2->update();
        fclose(fTSLupo);
        saveFile=false;
    }
    if (saveFile==true) ui->checkBox->setChecked(false);
}


void lupoReadout::on_checkBox_2_toggled(bool checked) //acsii
{
    if (checked==true&&saveFile==false){
        saveFileName=QString("%1/run%2_lupoTS%3").arg(saveDirectory).arg(ui->spinBox_2->value()).arg(".txt");
        fTSLupo=fopen(saveFileName.toUtf8(),"w");
        saveFile=true;
        acsii_flag=true;
    }else{
        ui->spinBox_2->setValue(ui->spinBox_2->value()+1);
        ui->spinBox_2->update();
        fclose(fTSLupo);
        saveFile=false;
    }
    if (saveFile==true) ui->checkBox->setChecked(false);
}

void lupoReadout::on_pushButton_7_clicked()
{
    lupo->sendPulse(1);
}

void lupoReadout::on_pushButton_8_clicked()
{
    lupo->sendPulse(2);
}

void lupoReadout::on_checkBox_2_clicked()
{

}
