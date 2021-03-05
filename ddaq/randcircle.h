#ifndef RANDCIRCLE_H
#define RANDCIRCLE_H
#include "TRandom.h"
#include "TMath.h"
#include "TCanvas.h"
#include "Riostream.h"
#include "TH2.h"
#include "TH2F.h"
#include "TEllipse.h"
#include "TColor.h"
#include "TROOT.h"
void genRndCircle(Double_t &x,Double_t &y,Double_t a,Double_t b,Double_t xpos,Double_t ypos,Double_t R);
void genRndRec(Double_t &x,Double_t &y,Double_t a,Double_t b);
void drawCircleHist(TH2D* his,Double_t xpos,Double_t ypos,Double_t R,Int_t npoint,Double_t weight);
void randCircle();
#endif // RANDCIRCLE_H
