// Utility methods
//
// Created by Samvel Khalatyan, Jun 01, 2011
// Copyright 2011, All rights reserved

#include <ostream>

#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/Utility.h"

std::ostream &bsm::operator <<(std::ostream &out, const LorentzVector &p4)
{
    out << "e: " << p4.e()
        << " px: " << p4.px()
        << " py: " << p4.py()
        << " pz: " << p4.pz();

    return out;
}

std::ostream &bsm::operator <<(std::ostream &out, const Vector &v)
{
    out << "x: " << v.x()
        << " y: " << v.y()
        << " z: " << v.z();

    return out;
}
