// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_INPUT_MAKER
#define BSM_INPUT_MAKER

#include <string>

#include <boost/shared_ptr.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace bsm
{
    class InputMaker: public edm:: EDAnalyzer
    {
        public:
            InputMaker(const edm::ParameterSet &);
            virtual ~InputMaker();

        private:
            virtual void beginJob();
            virtual void analyze(const edm::Event &, const edm::EventSetup &);
            virtual void endJob();
    };
}

#endif
