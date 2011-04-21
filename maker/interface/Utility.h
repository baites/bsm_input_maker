// Convert CMSSW format into input BSM
//
// Created by Samvel Khalatyan, Apr 21, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_UTILITY
#define BSM_UTILITY

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Point3D.h"

namespace bsm
{
    class LorentzVector;
    class Vector;

    namespace utility
    {
        void set(LorentzVector *bsm_p4, const math::XYZTLorentzVector *cms_p4);
        void set(Vector *bsm_v, const math::XYZPoint *cms_v);
    }
}

#endif
