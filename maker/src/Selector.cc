// CMSSW objects quality selectors
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#include <cmath>

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "bsm_input_maker/maker/interface/Selector.h"

bool bsm::selector::isGoodPrimaryVertex(const reco::Vertex &vertex,
                                        const bool &is_real_data)
{
    return !vertex.isFake()
        && 4 <= vertex.ndof()
        && (is_real_data ? 24 : 15) >= fabs(vertex.z())
        && 2 >= fabs(vertex.position().Rho());
}
