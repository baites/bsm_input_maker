// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <string>

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "bsm_input_maker/input/interface/Event.pb.h"
#include "bsm_input_maker/input/interface/Writer.h"

#include "bsm_input_maker/maker/interface/InputMaker.h"

using std::string;

using edm::Handle;
using edm::InputTag;
using edm::LogInfo;
using edm::LogWarning;
using edm::ParameterSet;

using bsm::InputMaker;

InputMaker::InputMaker(const ParameterSet &config)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    _writer.reset(new Writer(config.getParameter<string>("fileName")));
    _event.reset(new Event());

    _jets_tag = config.getParameter<string>("jets");
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
    }
}

DEFINE_FWK_MODULE(InputMaker);
