// Compare user applied Jet Energy Corrections to PAT
//
// Created by Samvel Khalatyan, Jul 13, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_JET_ENERGY_CORRECTION
#define BSM_JET_ENERGY_CORRECTION

#include <ostream>

#include <boost/shared_ptr.hpp>

#include "DataFormats/Math/interface/LorentzVector.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace edm
{
    class InputTag;
}

namespace bsm
{
    class JetEnergyCorrection: public edm:: EDAnalyzer
    {
        public:
            JetEnergyCorrection(const edm::ParameterSet &);

        private:
            virtual void analyze(const edm::Event &, const edm::EventSetup &);

            edm::InputTag _jets_tag;
            edm::InputTag _rho_tag;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &, const math::XYZTLorentzVector &);
}

#endif
