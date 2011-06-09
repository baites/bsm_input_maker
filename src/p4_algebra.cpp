// Test Lorentz Vector algebra
//
// Compare BSM lorentz vector algebra to ROOT TLorentz Vector
//
// Created by Samvel Khalatyan, Jun 09, 2011
// Copyright 2011, All rights reserved

#include <string>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TRandom3.h>
#include <TRint.h>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Physics.pb.h"

using namespace std;
using namespace bsm;

struct Monitor;
struct Delta;

typedef boost::shared_ptr<TLorentzVector> TP4Ptr;
typedef boost::shared_ptr<LorentzVector> P4Ptr;
typedef boost::shared_ptr<TRandom> RandomPtr;
typedef boost::shared_ptr<TH1> H1Ptr;
typedef boost::shared_ptr<Monitor> MonitorPtr;
typedef boost::shared_ptr<Delta> DeltaPtr;
typedef boost::shared_ptr<TCanvas> CanvasPtr;

struct Monitor
{
    Monitor();

    void fill(const P4Ptr &);
    void fill(const TP4Ptr &);

    void draw(const string &name, const string &title);

    H1Ptr _et;
    H1Ptr _eta;
    H1Ptr _mass;
    H1Ptr _momentum;
    H1Ptr _phi;
    H1Ptr _pt;

    CanvasPtr _canvas;
};

Monitor::Monitor()
{
    _et.reset(new TH1D());
    _et->SetDirectory(0);
    _et->Sumw2();
    _et->SetBins(100, 0, 200);

    _eta.reset(new TH1D());
    _eta->SetDirectory(0);
    _eta->Sumw2();
    _eta->SetBins(20, -10, 10);

    _mass.reset(new TH1D());
    _mass->SetDirectory(0);
    _mass->Sumw2();
    _mass->SetBins(100, 0, 200);

    _momentum.reset(new TH1D());
    _momentum->SetDirectory(0);
    _momentum->Sumw2();
    _momentum->SetBins(100, 0, 200);

    _phi.reset(new TH1D());
    _phi->SetDirectory(0);
    _phi->Sumw2();
    _phi->SetBins(70, -7, 7);

    _pt.reset(new TH1D());
    _pt->SetDirectory(0);
    _pt->Sumw2();
    _pt->SetBins(100, 0, 200);
}

void Monitor::fill(const P4Ptr &p)
{
    _et->Fill(et(*p));
    _eta->Fill(eta(*p));
    _mass->Fill(mass(*p));
    _momentum->Fill(momentum(*p));
    _phi->Fill(phi(*p));
    _pt->Fill(pt(*p));
}

void Monitor::fill(const TP4Ptr &p)
{
    _et->Fill(p->Et());
    _eta->Fill(p->Eta());
    _mass->Fill(p->M());
    _momentum->Fill(p->P());
    _phi->Fill(p->Phi());
    _pt->Fill(p->Pt());
}

void Monitor::draw(const string &name, const string &title)
{
    if (_canvas)
        return;

    _canvas.reset(new TCanvas(name.c_str(), title.c_str(), 1024, 480));
    _canvas->Divide(3, 2);

    _canvas->cd(1);
    _et->GetXaxis()->SetTitle("E_{T} [GeV]");
    _et->Draw("hist");

    _canvas->cd(2);
    _eta->GetXaxis()->SetTitle("#eta");
    _eta->Draw("hist");

    _canvas->cd(3);
    _mass->GetXaxis()->SetTitle("Mass [GeV/c^{2}]");
    _mass->Draw("hist");

    _canvas->cd(4);
    _momentum->GetXaxis()->SetTitle("p [GeV/c]");
    _momentum->Draw("hist");

    _canvas->cd(5);
    _phi->GetXaxis()->SetTitle("#phi");
    _phi->Draw("hist");

    _canvas->cd(6);
    _pt->GetXaxis()->SetTitle("p_{T} [GeV/c]");
    _pt->Draw("hist");
}

struct Delta
{
    Delta();

    void fill(const P4Ptr &, const P4Ptr &);
    void fill(const TP4Ptr &, const TP4Ptr &);

    void draw(const string &name, const string &title);

    H1Ptr _dphi;
    H1Ptr _dr;

    CanvasPtr _canvas;
};

Delta::Delta()
{
    _dphi.reset(new TH1D());
    _dphi->SetDirectory(0);
    _dphi->Sumw2();
    _dphi->SetBins(100, -10, 10);

    _dr.reset(new TH1D());
    _dr->SetDirectory(0);
    _dr->Sumw2();
    _dr->SetBins(100, 0, 10);
}

void Delta::fill(const P4Ptr &v1, const P4Ptr &v2)
{
    _dphi->Fill(dphi(*v1, *v2));
    _dr->Fill(dr(*v1, *v2));
}

void Delta::fill(const TP4Ptr &v1, const TP4Ptr &v2)
{
    _dphi->Fill(v1->DeltaPhi(*v2));
    _dr->Fill(v1->DeltaR(*v2));
}

void Delta::draw(const string &name, const string &title)
{
    if (_canvas)
        return;

    _canvas.reset(new TCanvas(name.c_str(), title.c_str(), 640, 320));
    _canvas->Divide(2);

    _canvas->cd(1);
    _dphi->GetXaxis()->SetTitle("#Delta #phi");
    _dphi->Draw("hist");

    _canvas->cd(2);
    _dr->GetXaxis()->SetTitle("#Delta R");
    _dr->Draw("hist");
}

void fill(const RandomPtr &, const P4Ptr &, const TP4Ptr &);

int main(int argc, char *argv[])
{
    RandomPtr randomizer(new TRandom3());
    randomizer->SetSeed(time(0));

    P4Ptr v1(new LorentzVector());
    P4Ptr v2(new LorentzVector());
    TP4Ptr tv1(new TLorentzVector());
    TP4Ptr tv2(new TLorentzVector());

    MonitorPtr monitor_v1(new Monitor());
    MonitorPtr monitor_v2(new Monitor());
    MonitorPtr monitor_tv1(new Monitor());
    MonitorPtr monitor_tv2(new Monitor());

    DeltaPtr delta_v(new Delta());
    DeltaPtr delta_tv(new Delta());

    for(uint32_t i = 0; 1000 > i; ++i)
    {
        fill(randomizer, v1, tv1);
        fill(randomizer, v2, tv2);

        monitor_v1->fill(v1);
        monitor_v2->fill(v2);

        monitor_tv1->fill(tv1);
        monitor_tv2->fill(tv2);

        delta_v->fill(v1, v2);
        delta_tv->fill(tv1, tv2);
    }

    int empty_argc = 1;
    char *empty_argv[] = { argv[0] };

    boost::shared_ptr<TRint>
        app(new TRint("app", &empty_argc, empty_argv));

    monitor_v1->draw("monitor_v1", "Lorentz Vector 1");
    monitor_v2->draw("monitor_v2", "Lorentz Vector 2");
    monitor_tv1->draw("monitor_tv1", "TLorentz Vector 1");
    monitor_tv2->draw("monitor_tv2", "TLorentz Vector 2");
    
    delta_v->draw("delta_v", "Delta between Lorentz Vectors");
    delta_tv->draw("delta_tv", "Delta between TLorentz Vectors");

    app->Run();

    return 0;
}

void fill(const RandomPtr &r, const P4Ptr &v, const TP4Ptr &t)
{
    double e = r->Gaus(r->Uniform(50, 150), r->Uniform(2, 10));
    double px = r->Gaus(r->Uniform(20, 80), r->Uniform(2, 10));
    double py = r->Gaus(r->Uniform(20, 80), r->Uniform(2, 10));
    double pz = r->Gaus(r->Uniform(20, 80), r->Uniform(2, 10));

    v->set_e(e);
    v->set_px(px);
    v->set_py(py);
    v->set_pz(pz);

    t->SetPxPyPzE(px, py, pz, e);
}
