// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "bsm_input_maker/input/interface/Event.pb.h"
#include "bsm_input_maker/input/interface/Input.pb.h"
#include "bsm_input_maker/input/interface/Writer.h"
#include "bsm_input_maker/maker/interface/Selector.h"
#include "bsm_input_maker/maker/interface/Utility.h"

#include "bsm_input_maker/maker/interface/InputMaker.h"

using std::string;
using std::vector;

using boost::to_lower;

using edm::Handle;
using edm::InputTag;
using edm::LogInfo;
using edm::LogWarning;
using edm::ParameterSet;

using reco::Vertex;

using bsm::InputMaker;

InputMaker::InputMaker(const ParameterSet &config)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    _writer.reset(new Writer(config.getParameter<string>("fileName")));
    _event.reset(new Event());

    _jets_tag = config.getParameter<string>("jets");
    _electrons_tag = config.getParameter<string>("electrons");
    _muons_tag = config.getParameter<string>("muons");
    _primary_vertices_tag = config.getParameter<string>("primary_vertices");
    _missing_energies = config.getParameter<string>("missing_energies");

    _input_type = config.getParameter<string>("input_type");
    to_lower(_input_type);

    Input::Type type(Input::UNKNOWN);
    if ("data" == _input_type)
        type = Input::DATA;

    else if ("ttbar" == _input_type)
        type = Input::TTBAR;

    else if ("qcd" == _input_type)
        type = Input::QCD;

    else if ("wjets" == _input_type)
        type = Input::WJETS;

    else if ("zjets" == _input_type)
        type = Input::ZJETS;

    else if ("vqq" == _input_type)
        type = Input::VQQ;

    else if ("single_top_t_channel" == _input_type)
        type = Input::SINGLE_TOP_T_CHANNEL;

    else if ("single_top_s_channel" == _input_type)
        type = Input::SINGLE_TOP_S_CHANNEL;

    else if ("single_top_tw_channel" == _input_type)
        type = Input::SINGLE_TOP_TW_CHANNEL;

    else if ("wc" == _input_type)
        type = Input::WC;

    _writer->input()->set_type(type);
}

InputMaker::~InputMaker()
{
    _event.reset();
    _writer.reset();

    google::protobuf::ShutdownProtobufLibrary();
}



// Privates
//
void InputMaker::beginJob()
{
}

void InputMaker::analyze(const edm::Event &event,
                        const edm::EventSetup &setup)
{
    _event->Clear();

    jets(event);
    electrons(event);
    muons(event);
    primaryVertices(event);
    met(event);

    _writer->write(*_event);
}

void InputMaker::endJob()
{
}

void InputMaker::jets(const edm::Event &event)
{
    using pat::JetCollection;

    Handle<JetCollection> jets;
    event.getByLabel(InputTag(_jets_tag), jets);

    if (!jets.isValid())
    {
        LogWarning("InputMaker") << "failed to extract jets";

        return;
    }

    for(JetCollection::const_iterator jet = jets->begin();
            jets->end() != jet;
            ++jet)
    {
        bsm::Jet *pb_jet = _event->add_jets();

        utility::set(pb_jet->mutable_physics_object()->mutable_p4(),
                &jet->p4());
        utility::set(pb_jet->mutable_physics_object()->mutable_vertex(),
            &jet->vertex());

        // Skip the rest if Generator Parton is not found for the jet
        //
        if (!jet->genParton())
            continue;

        const reco::GenParticle *parton = jet->genParton();
        bsm::GenParticle *pb_gen_particle = pb_jet->mutable_gen_parton();

        utility::set(pb_gen_particle->mutable_physics_object()->mutable_p4(),
                &parton->p4());

        utility::set(pb_gen_particle->mutable_physics_object()->mutable_vertex(),
                &parton->vertex());

        pb_gen_particle->set_id(parton->pdgId());
        pb_gen_particle->set_status(parton->status());

    }
}

void InputMaker::electrons(const edm::Event &event)
{
    using pat::ElectronCollection;

    Handle<ElectronCollection> electrons;
    event.getByLabel(InputTag(_electrons_tag), electrons);

    if (!electrons.isValid())
    {
        LogWarning("InputMaker") << "failed to extract electrons";

        return;
    }

    for(ElectronCollection::const_iterator electron = electrons->begin();
            electrons->end() != electron;
            ++electron)
    {
        bsm::Electron *pb_electron = _event->add_electrons();

        utility::set(pb_electron->mutable_physics_object()->mutable_p4(),
                &electron->p4());
        utility::set(pb_electron->mutable_physics_object()->mutable_vertex(),
                &electron->vertex());
    }
}

void InputMaker::muons(const edm::Event &event)
{
    using pat::MuonCollection;

    Handle<MuonCollection> muons;
    event.getByLabel(InputTag(_muons_tag), muons);

    if (!muons.isValid())
    {
        LogWarning("InputMaker") << "failed to extract muons";

        return;
    }

    for(MuonCollection::const_iterator muon = muons->begin();
            muons->end() != muon;
            ++muon)
    {
        bsm::Muon *pb_muon = _event->add_muons();

        utility::set(pb_muon->mutable_physics_object()->mutable_p4(),
                &muon->p4());
        utility::set(pb_muon->mutable_physics_object()->mutable_vertex(),
                &muon->vertex());
    }
}

void InputMaker::primaryVertices(const edm::Event &event)
{
    typedef vector<Vertex> PVCollection;

    Handle<PVCollection> primary_vertices;
    event.getByLabel(InputTag(_primary_vertices_tag), primary_vertices);

    if (!primary_vertices.isValid())
    {
        LogWarning("InputMaker") << "failed to extract primary_vertices";

        return;
    }

    for(PVCollection::const_iterator vertex = primary_vertices->begin();
            primary_vertices->end() != vertex;
            ++vertex)
    {
        if (!selector::isGoodPrimaryVertex(*vertex, event.isRealData()))
            continue;

        bsm::PrimaryVertex *pb_vertex = _event->add_primary_vertices();

        utility::set(pb_vertex->mutable_vertex(), &vertex->position());
    }
}

void InputMaker::met(const edm::Event &event)
{
    using pat::METCollection;

    Handle<METCollection> mets;
    event.getByLabel(InputTag(_missing_energies), mets);

    if (!mets.isValid())
    {
        LogWarning("InputMaker") << "failed to extract mets";

        return;
    }

    if (!mets->size())
    {
        LogWarning("InputMaker") << "empty mets";

        return;
    }

    bsm::MissingEnergy *pb_met = _event->mutable_missing_energy();

    utility::set(pb_met->mutable_physics_object()->mutable_p4(),
            &mets->begin()->p4());
    utility::set(pb_met->mutable_physics_object()->mutable_vertex(),
            &mets->begin()->vertex());
}

DEFINE_FWK_MODULE(InputMaker);
