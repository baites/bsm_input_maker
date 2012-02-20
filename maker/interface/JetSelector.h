// Select jets with loose selection
//
// Created by Samvel Khalatyan, Oct 10, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_JET_SELECTOR
#define BSM_JET_SELECTOR

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_input_maker/maker/interface/Selector.h"
#include "bsm_input_maker/maker/interface/ElectronSelector.h"
#include "bsm_input_maker/maker/interface/MuonSelector.h"

class FactorizedJetCorrector;

namespace pat
{
    class Jet;
}

namespace bsm
{
    class JetSelector: public Selector
    {
        public:
            typedef std::vector<std::string> JECFiles;

            typedef std::vector<const pat::Jet *> Jets;
            typedef ElectronSelector::Electrons Electrons;
            typedef MuonSelector::Muons Muons;
            
            JetSelector(const edm::InputTag &jet_tag,
                    const edm::InputTag &primary_vertex_tag,
                    const edm::InputTag &rho_tag,
                    const JECFiles &);

            virtual bool init(const edm::Event *,
                    const Electrons &,
                    const Muons &);

            const Jets &jet() const;

            const edm::InputTag &primaryVertexTag() const;
            const edm::InputTag &rhoTag() const;

        private:
            Jets _jet;

            edm::InputTag _primary_vertex_tag;
            edm::InputTag _rho_tag;

            boost::shared_ptr<FactorizedJetCorrector> _jec;
    };
}

#endif
