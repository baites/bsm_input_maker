// Input Reader example
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/filesystem.hpp>

#include <TCanvas.h>
#include <TH1F.h>
#include <TRint.h>

#include "interface/Event.pb.h"
#include "interface/Reader.h"

using bsm::Event;
using bsm::Jet;
using bsm::LorentzVector;
using bsm::Muon;
using bsm::PrimaryVertex;
using bsm::Vector;
using bsm::Reader;

using std::cerr;
using std::cout;
using std::endl;

namespace fs = boost::filesystem;

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

        boost::shared_ptr<Reader> reader(new Reader(fs::path(argv[1])));
        cout << "Start reading events: " << reader->eventsStored() << endl;

        for(boost::shared_ptr<Event> event(new Event());
                reader->read(*event);
                )
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

            event->Clear();
        }

        boost::shared_ptr<TRint> app(new TRint("app", &argc, argv));

        boost::shared_ptr<TCanvas> canvas(new TCanvas("canvas", "Canvas", 800, 640));
        canvas->Divide(2, 2);

        canvas->cd(1);
        h_npv->Draw();

        canvas->cd(2);
        h_pv_x->Draw();

        canvas->cd(3);
        h_pv_y->Draw();

        canvas->cd(4);
        h_pv_z->Draw();

        cout << "Events read: " << reader->eventsRead() << endl;

        app->Run();
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
catch(...)
{
    cerr << "Unknown error" << endl;

    google::protobuf::ShutdownProtobufLibrary();
}
