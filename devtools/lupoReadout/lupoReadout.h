#ifndef LUPOREADOUT_H
#define LUPOREADOUT_H

#include <QMainWindow>
#include "lupoLib.h"
#include "stdio.h"
#include "caenvmeA32.h"
#include "QFileDialog"
//For share memory
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define SHMKEY 95

namespace Ui {
class lupoReadout;
}

typedef struct SHM_DGTZ_S{
  uint32_t pid;
  uint32_t timestampMSB;
  uint32_t timestampLSB;
  uint32_t trgCnt;  
  int ana_flag;
} SHM_DGTZ;

class lupoReadout : public QMainWindow
{
    Q_OBJECT

public:
    explicit lupoReadout(QWidget *parent = 0);
    ~lupoReadout();
    SHM_DGTZ* shmp;
     int shmid;

private:
    Ui::lupoReadout *ui;


private slots:
    void readoutCycle();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    //void on_pushButton_7_clicked();

    void on_checkBox_toggled(bool checked);

    void on_checkBox_2_clicked();

    void on_checkBox_2_toggled(bool checked);

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

signals:
    void endOfReadOutCycle();

private:
    int BHandle;
    lupoLib* lupo;
    unsigned int triggerCounter;
    unsigned int receivedTrigger;
    long long timestamp;
    FILE* fTSLupo;
    u_int64_t CurrentTime, PrevRateTime, ElapsedTime;
    QString output;
    unsigned int deadtimeC;
    QString saveDirectory;
    QString saveFileName;
    bool saveFile;
    bool acsii_flag;

    int ti;

    QFileDialog dialog;
};

#endif // LUPOREADOUT_H
