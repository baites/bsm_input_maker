// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "bsm_input_maker/maker/interface/InputMaker.h"

using edm::Handle;
using edm::InputTag;
using edm::LogInfo;
using edm::LogWarning;
using edm::ParameterSet;

using bsm::InputMaker;

InputMaker::InputMaker(const ParameterSet &config)
{
}

InputMaker::~InputMaker()
{
}



// Privates
//
void InputMaker::beginJob()
{
}

void InputMaker::analyze(const edm::Event &event,
                        const edm::EventSetup &setup)
{
}

void InputMaker::endJob()
{
}

DEFINE_FWK_MODULE(InputMaker);
