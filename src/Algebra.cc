// Define Physics objects algebra
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/Algebra.h"

using bsm::LorentzVector;
using bsm::Vector;

LorentzVector &bsm::operator +=(LorentzVector &v1, const LorentzVector &v2)
{
    v1.set_e(v1.e() + v2.e());
    v1.set_px(v1.px() + v2.px());
    v1.set_py(v1.py() + v2.py());
    v1.set_pz(v1.pz() + v2.pz());

    return v1;
}

LorentzVector &bsm::operator -=(LorentzVector &v1, const LorentzVector &v2)
{
    v1.set_e(v1.e() - v2.e());
    v1.set_px(v1.px() - v2.px());
    v1.set_py(v1.py() - v2.py());
    v1.set_pz(v1.pz() - v2.pz());

    return v1;
}

Vector &bsm::operator +=(Vector &v1, const Vector &v2)
{
    v1.set_x(v1.x() + v2.x());
    v1.set_y(v1.y() + v2.y());
    v1.set_z(v1.z() + v2.z());

    return v1;
}

Vector &bsm::operator -=(Vector &v1, const Vector &v2)
{
    v1.set_x(v1.x() - v2.x());
    v1.set_y(v1.y() - v2.y());
    v1.set_z(v1.z() - v2.z());

    return v1;
}
