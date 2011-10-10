// CMSSW objects quality selectors
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SELECTOR
#define BSM_SELECTOR

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace reco
{
    class Vertex;
}

namespace bsm
{
    namespace selector
    {
        bool isGoodPrimaryVertex(const reco::Vertex &vertex,
                                 const bool &is_real_data = false);
    }

    class Selector
    {
        public:
            Selector(const edm::InputTag &tag);
            virtual ~Selector();

            virtual bool init(const edm::Event *) = 0;

            const edm::InputTag &tag() const;

        private:
            edm::InputTag _tag;
    };

}

#endif
