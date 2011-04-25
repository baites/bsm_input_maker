// CMSSW objects quality selectors
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SELECTOR
#define BSM_SELECTOR

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
}

#endif
