// Convert CMSSW format into input BSM
//
// Created by Samvel Khalatyan, Apr 21, 2011
// Copyright 2011, All rights reserved

#include "bsm_input_maker/bsm_input/interface/Physics.pb.h"

#include "bsm_input_maker/maker/interface/Utility.h"

void bsm::utility::set(LorentzVector *bsm_p4, const math::XYZTLorentzVector *cms_p4)
{
    bsm_p4->set_e(cms_p4->energy());
    bsm_p4->set_px(cms_p4->px());
    bsm_p4->set_py(cms_p4->py());
    bsm_p4->set_pz(cms_p4->pz());
}

void bsm::utility::set(Vector *bsm_v, const math::XYZPoint *cms_v)
{
    bsm_v->set_x(cms_v->x());
    bsm_v->set_y(cms_v->y());
    bsm_v->set_z(cms_v->z());
}
