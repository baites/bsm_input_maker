// Select muons with loose selection
//
// Created by Samvel Khalatyan, Oct 10, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_MUON_SELECTOR
#define BSM_MUON_SELECTOR

#include <vector>

#include "bsm_input_maker/maker/interface/Selector.h"

namespace pat
{
    class Muon;
}

namespace bsm
{
    class MuonSelector: public Selector
    {
        public:
            MuonSelector(const edm::InputTag &muon_tag,
                    const edm::InputTag &primary_vertex_tag);

            typedef std::vector<const pat::Muon *> Muons;
            
            virtual bool init(const edm::Event *);

            const Muons &muon() const;

            const edm::InputTag &primaryVertexTag() const;

        private:
            Muons _muon;

            edm::InputTag _primary_vertex_tag;
    };
}

#endif
