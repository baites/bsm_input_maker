// Input Reader example
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <vector>

#include <TCanvas.h>
#include <TH1F.h>
#include <TRint.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Reader.h"

using bsm::Event;
using bsm::Input;
using bsm::Jet;
using bsm::LorentzVector;
using bsm::Muon;
using bsm::PrimaryVertex;
using bsm::Vector;
using bsm::Reader;

using std::cerr;
using std::cout;
using std::endl;
using std::vector;

int main(int argc, char *argv[])
try
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (2 > argc)
    {
        cerr << "Usage: " << argv[0] << " input.pb" << endl;

        google::protobuf::ShutdownProtobufLibrary();

        return 1;
    }

    {
        boost::shared_ptr<TH1> h_npv(new TH1F("npv", "N_{PV}", 10, 0, 10));
        h_npv->SetDirectory(0);
        h_npv->GetXaxis()->SetTitle("N_{PV}");

        boost::shared_ptr<TH1> h_pv_x(new TH1F("pv_x", "Primary Vertex: X", 100, -5, 5));
        h_pv_x->SetDirectory(0);
        h_pv_x->GetXaxis()->SetTitle("X_{PV} [cm]");

        boost::shared_ptr<TH1> h_pv_y(new TH1F("pv_y", "Primary Vertex: Y", 100, -5, 5));
        h_pv_y->SetDirectory(0);
        h_pv_y->GetXaxis()->SetTitle("Y_{PV} [cm]");

        boost::shared_ptr<TH1> h_pv_z(new TH1F("pv_z", "Primary Vertex: Z", 100, -5, 5));
        h_pv_z->SetDirectory(0);
        h_pv_z->GetXaxis()->SetTitle("Z_{PV} [cm]");

        boost::shared_ptr<TH1> h_njet(new TH1F("jets", "N_{jets}", 20, 0, 20));
        h_njet->SetDirectory(0);
        h_njet->GetXaxis()->SetTitle("N_{JET}");

        boost::shared_ptr<TH1> h_jet_x(new TH1F("jet_x", "Jet: X", 200, -10, 10));
        h_jet_x->SetDirectory(0);
        h_jet_x->GetXaxis()->SetTitle("X_{JET} [cm]");

        boost::shared_ptr<TH1> h_jet_y(new TH1F("jet_y", "Jet: Y", 200, -10, 10));
        h_jet_y->SetDirectory(0);
        h_jet_y->GetXaxis()->SetTitle("Y_{JET} [cm]");

        boost::shared_ptr<TH1> h_jet_z(new TH1F("jet_z", "Jet: Z", 200, -10, 10));
        h_jet_z->SetDirectory(0);
        h_jet_z->GetXaxis()->SetTitle("Z_{JET} [cm]");

        boost::shared_ptr<TH1> h_jet_e(new TH1F("jet_e", "Jet: E", 100, 100, 200));
        h_jet_e->SetDirectory(0);
        h_jet_e->GetXaxis()->SetTitle("E_{JET} [GeV]");

        boost::shared_ptr<TH1> h_jet_px(new TH1F("jet_px", "Jet: PX", 100, 0, 200));
        h_jet_px->SetDirectory(0);
        h_jet_px->GetXaxis()->SetTitle("PX_{JET} [GeV/c]");

        boost::shared_ptr<TH1> h_jet_py(new TH1F("jet_py", "Jet: PY", 100, 0, 200));
        h_jet_py->SetDirectory(0);
        h_jet_py->GetXaxis()->SetTitle("PY_{JET} [GeV/c]");

        boost::shared_ptr<TH1> h_jet_pz(new TH1F("jet_pz", "Jet: PZ", 100, 0, 200));
        h_jet_pz->SetDirectory(0);
        h_jet_pz->GetXaxis()->SetTitle("PZ_{JET} [GeV/c]");

        boost::shared_ptr<TH1> h_nmuon(new TH1F("muons", "N_{muons}", 20, 0, 20));
        h_nmuon->SetDirectory(0);
        h_nmuon->GetXaxis()->SetTitle("N_{MUON}");

        boost::shared_ptr<TH1> h_muon_x(new TH1F("muon_x", "Muon: X", 200, -10, 10));
        h_muon_x->SetDirectory(0);
        h_muon_x->GetXaxis()->SetTitle("X_{MUON} [cm]");

        boost::shared_ptr<TH1> h_muon_y(new TH1F("muon_y", "Muon: Y", 200, -10, 10));
        h_muon_y->SetDirectory(0);
        h_muon_y->GetXaxis()->SetTitle("Y_{MUON} [cm]");

        boost::shared_ptr<TH1> h_muon_z(new TH1F("muon_z", "Muon: Z", 200, -10, 10));
        h_muon_z->SetDirectory(0);
        h_muon_z->GetXaxis()->SetTitle("Z_{MUON} [cm]");

        boost::shared_ptr<TH1> h_muon_e(new TH1F("muon_e", "Muon: E", 100, 0.05, 0.15));
        h_muon_e->SetDirectory(0);
        h_muon_e->GetXaxis()->SetTitle("E_{MUON} [GeV]");

        boost::shared_ptr<TH1> h_muon_px(new TH1F("muon_px", "Muon: PX", 50, 0, 50));
        h_muon_px->SetDirectory(0);
        h_muon_px->GetXaxis()->SetTitle("PX_{MUON} [GeV/c]");

        boost::shared_ptr<TH1> h_muon_py(new TH1F("muon_py", "Muon: PY", 50, 0, 50));
        h_muon_py->SetDirectory(0);
        h_muon_py->GetXaxis()->SetTitle("PY_{MUON} [GeV/c]");

        boost::shared_ptr<TH1> h_muon_pz(new TH1F("muon_pz", "Muon: PZ", 50, 0, 50));
        h_muon_pz->SetDirectory(0);
        h_muon_pz->GetXaxis()->SetTitle("PZ_{MUON} [GeV/c]");

        boost::shared_ptr<Reader> reader(new Reader(argv[1]));
        if (reader->input()->has_type())
        {
            switch(reader->input()->type())
            {
                case Input::DATA: cout << " DATA input" << endl;
                                  break;

                default: cout << " Unknown Input" << endl;
                         break;
            }
        }

        cout << "Start reading events: " << reader->input()->events() << endl;

        uint32_t events_read = 0;
        for(boost::shared_ptr<Event> event(new Event());
                reader->read(*event);
                ++events_read)
        {
            h_npv->Fill(event->primary_vertices_size());

            for(int ipv = 0, pvs = event->primary_vertices_size();
                    pvs > ipv;
                    ++ipv)
            {
                const PrimaryVertex &pv = event->primary_vertices(ipv);

                h_pv_x->Fill(pv.vertex().x());
                h_pv_y->Fill(pv.vertex().y());
                h_pv_z->Fill(pv.vertex().z());
            }

            h_njet->Fill(event->jets_size());

            for(int ijet = 0, jets = event->jets_size();
                    jets > ijet;
                    ++ijet)
            {
                const Jet &jet = event->jets(ijet);

                h_jet_x->Fill(jet.physics_object().vertex().x());
                h_jet_y->Fill(jet.physics_object().vertex().y());
                h_jet_z->Fill(jet.physics_object().vertex().z());

                h_jet_e->Fill(jet.physics_object().p4().e());
                h_jet_px->Fill(jet.physics_object().p4().px());
                h_jet_py->Fill(jet.physics_object().p4().py());
                h_jet_pz->Fill(jet.physics_object().p4().pz());
            }

            h_nmuon->Fill(event->muons_size());

            for(int imuon = 0, muons = event->muons_size();
                    muons > imuon;
                    ++imuon)
            {
                const Muon &muon = event->muons(imuon);

                h_muon_x->Fill(muon.physics_object().vertex().x());
                h_muon_y->Fill(muon.physics_object().vertex().y());
                h_muon_z->Fill(muon.physics_object().vertex().z());

                h_muon_e->Fill(muon.physics_object().p4().e());
                h_muon_px->Fill(muon.physics_object().p4().px());
                h_muon_py->Fill(muon.physics_object().p4().py());
                h_muon_pz->Fill(muon.physics_object().p4().pz());
            }

            event->Clear();
        }

        cout << "Events read: " << events_read << endl;

        {
            boost::shared_ptr<TRint> app(new TRint("app", &argc, argv));

            typedef boost::shared_ptr<TCanvas> CanvasPtr;

            vector<CanvasPtr> canvases;
            {
                CanvasPtr canvas(new TCanvas("pv_canvas", "PV Canvas", 800, 640));
                canvases.push_back(canvas);

                canvas->Divide(2, 2);

                canvas->cd(1);
                h_npv->Draw();

                canvas->cd(2);
                h_pv_x->Draw();

                canvas->cd(3);
                h_pv_y->Draw();

                canvas->cd(4);
                h_pv_z->Draw();
            }

            {
                CanvasPtr canvas(new TCanvas("jet_canvas_vertex", "Jet Canvas: Vertices", 800, 640));
                canvases.push_back(canvas);

                canvas->Divide(2, 2);

                canvas->cd(1);
                h_njet->Draw();

                canvas->cd(2);
                h_jet_x->Draw();

                canvas->cd(3);
                h_jet_y->Draw();

                canvas->cd(4);
                h_jet_z->Draw();
            }

            {
                CanvasPtr canvas(new TCanvas("jet_canvas_p4", "Jet Canvas: Lorentz Vector", 800, 640));
                canvases.push_back(canvas);

                canvas->Divide(2, 2);

                canvas->cd(1);
                h_jet_e->Draw();

                canvas->cd(2);
                h_jet_px->Draw();

                canvas->cd(3);
                h_jet_py->Draw();

                canvas->cd(4);
                h_jet_pz->Draw();
            }

            {
                CanvasPtr canvas(new TCanvas("muon_canvas_vertex", "Muon Canvas: Vertices", 800, 640));
                canvases.push_back(canvas);

                canvas->Divide(2, 2);

                canvas->cd(1);
                h_nmuon->Draw();

                canvas->cd(2);
                h_muon_x->Draw();

                canvas->cd(3);
                h_muon_y->Draw();

                canvas->cd(4);
                h_muon_z->Draw();
            }

            {
                CanvasPtr canvas(new TCanvas("muon_canvas_p4", "Muon Canvas: Lorentz Vector", 800, 640));
                canvases.push_back(canvas);

                canvas->Divide(2, 2);

                canvas->cd(1);
                h_muon_e->Draw();

                canvas->cd(2);
                h_muon_px->Draw();

                canvas->cd(3);
                h_muon_py->Draw();

                canvas->cd(4);
                h_muon_pz->Draw();
            }

            app->Run();
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
catch(...)
{
    cerr << "Unknown error" << endl;

    google::protobuf::ShutdownProtobufLibrary();
}
