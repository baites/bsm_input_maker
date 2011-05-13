// Input Writer example
//
// Write messages into specified output file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/lexical_cast.hpp>

#include <TRandom3.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Writer.h"

using bsm::Event;
using bsm::Input;
using bsm::Jet;
using bsm::LorentzVector;
using bsm::Muon;
using bsm::PrimaryVertex;
using bsm::Vector;
using bsm::Writer;

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, const char *argv[])
try
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (2 > argc)
    {
        cerr << "Usage: " << argv[0] << " [events] output.pb" << endl;

        google::protobuf::ShutdownProtobufLibrary();

        return 1;
    }

    {
        int events = 100;
        boost::shared_ptr<Writer> writer;

        if (3 == argc)
        {
            events = boost::lexical_cast<uint32_t>(argv[1]);
            writer.reset(new Writer(argv[2], 10000));
        }
        else
            writer.reset(new Writer(argv[1]));

        writer->input()->set_type(Input::DATA);

        boost::shared_ptr<Event> event(new Event());
        boost::shared_ptr<TRandom> randomizer(new TRandom3());
        randomizer->SetSeed(time(0));

        cout << "Generate: " << events << " events" << endl;

        for(int i = 0; events > i; ++i)
        {
            for(int pv = 0, pvs = randomizer->Poisson(2); pvs > pv; ++pv)
            {
                PrimaryVertex *pv = event->add_primary_vertices();

                Vector *vertex = pv->mutable_vertex();

                vertex->set_x(randomizer->Gaus(0, .3));
                vertex->set_y(randomizer->Gaus(0, .5));
                vertex->set_z(randomizer->Gaus(0, .7));
            }

            for(int j = 0, jets = randomizer->Poisson(5); jets > j; ++j)
            {
                Jet *jet = event->add_jets();
                LorentzVector *p4 = jet->mutable_physics_object()->mutable_p4();

                p4->set_e(randomizer->Gaus(172, 10));
                p4->set_px(randomizer->Gaus(100, 15));
                p4->set_py(randomizer->Gaus(100, 20));
                p4->set_pz(randomizer->Gaus(100, 25));

                Vector *vertex = jet->mutable_physics_object()->mutable_vertex();

                vertex->set_x(randomizer->Gaus(0, 5));
                vertex->set_y(randomizer->Gaus(0, 4));
                vertex->set_z(randomizer->Gaus(0, 3));
            }

            for(int m = 0, muons = randomizer->Poisson(10); muons > m; ++m)
            {
                Muon *muon = event->add_muons();
                LorentzVector *p4 = muon->mutable_physics_object()->mutable_p4();

                p4->set_e(randomizer->Gaus(.105, 0.005));
                p4->set_px(randomizer->Gaus(10, 3));
                p4->set_py(randomizer->Gaus(10, 2));
                p4->set_pz(randomizer->Gaus(10, 1));

                Vector *vertex = muon->mutable_physics_object()->mutable_vertex();

                vertex->set_x(randomizer->Gaus(1, 2));
                vertex->set_y(randomizer->Gaus(1, 3));
                vertex->set_z(randomizer->Gaus(1, 4));
            }

            writer->write(*event);

            event->Clear();
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
