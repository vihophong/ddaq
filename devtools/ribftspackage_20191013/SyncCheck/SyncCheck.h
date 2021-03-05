#include <TQObject.h>
#include <TGStatusBar.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TGButton.h>

#include <RQ_OBJECT.h>
class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;
class MyMainFrame {
    
    RQ_OBJECT("MyMainFrame")
    
private:
    TGMainFrame          *fMain;
    TRootEmbeddedCanvas  *fEcan;
    TGStatusBar          *fStatusBar;
    TTimer               *timer;
    TTimer               *timer2;
    TGCheckButton        *PlotUpdate;
    TH2F                  *h2;
    TH2F                  *h2partial;
    TH1F                  *h1X;
    TH1F                  *h1Y;
public:

    MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
    virtual ~MyMainFrame();
    void DoExit();
    void DoDraw();
    void Reset();
    void AutoUpdate();
    void TimerDone();
    void ClearDone();
    void SelectPixel(Int_t,Int_t,Int_t,TObject*);
    void SetStatusText(const char *txt, Int_t pi);
    void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);	

};

