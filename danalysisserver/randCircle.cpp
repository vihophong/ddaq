#include "randCircle.h"
#include "TLatex.h"
void genRndCircle(Double_t &x,Double_t &y,Double_t a,Double_t b,Double_t xpos,Double_t ypos,Double_t R){
    if (b<a){
        Double_t temp=a;
        a=b;
        b=temp;
    }
    x=xpos+b*R*TMath::Cos(2*TMath::Pi()*a/b);
    y=ypos+b*R*TMath::Sin(2*TMath::Pi()*a/b);
}
void genRndRec(Double_t &x,Double_t &y,Double_t a,Double_t b){
    x=-50+a*100;
    y=-50+b*100;
}
void drawCircleHist(TH2D* his,Double_t xpos,Double_t ypos,Double_t R,Int_t npoint,Double_t weight){
    Double_t dphi=2*TMath::Pi()/(Double_t)npoint;
    for (Int_t i=0;i<npoint;i++){
        Double_t x,y;
        x=xpos+R*TMath::Cos(dphi*i);
        y=ypos+R*TMath::Sin(dphi*i);
        his->Fill(x,y,weight);
    }
}

void randCircle()
{
    TCanvas* c1=new TCanvas("c1","c1",900,900);
    c1->cd();

    TH2D* h2=new TH2D("h2","h2",1000,-450,450,1000,-450,450);
    h2->GetXaxis()->SetRangeUser(-450,450);
    h2->GetYaxis()->SetRangeUser(-450,450);
    h2->Draw();
    std::ifstream inf("briken_mapping.txt",std::ios::in);
    Double_t xpos[172];
    Double_t ypos[172];
    Double_t diameter[172];
    Int_t ch;
    while (inf.good()){
        inf>>ch;
        inf>>xpos[ch]>>ypos[ch]>>diameter[ch];
        cout<<ch<<"-"<<xpos[ch]<<endl;
    }

    TEllipse* holes[172];
    TLatex* holest[172];
    for (Int_t i=0;i<172;i++){
        holes[i]=new TEllipse(xpos[i],ypos[i],diameter[i]/2);
        holest[i]=new TLatex(xpos[i],ypos[i],Form("%d",i));
        holest[i]->SetTextSize(0.02);
        holes[i]->SetLineColor(2);
        holes[i]->SetLineWidth(2);
        holes[i]->Draw("same");
        holest[i]->Draw("same");
        //drawCircleHist(h2,xpos[i],ypos[i],diameter[i]/2,200,1000);
    }

    /*
    TRandom rr;
    for (Int_t i=0;i<100;i++){
        Double_t a,b;
        a=rr.Rndm();
        b=rr.Rndm();
        Double_t x,y;
        genRndCircle(x,y,a,b,xpos[100],ypos[100],diameter[100]/2);
        h2->Fill(x,y);
        a=rr.Rndm();
        b=rr.Rndm();
        genRndRec(x,y,a,b);
        h2->Fill(x,y);
    }
    h2->Draw("colz same");
    */
}
