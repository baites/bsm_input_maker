// Define Physics objects algebra
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_ALGEBRA
#define BSM_IO_ALGEBRA

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    extern const double pi;

    LorentzVector &operator +=(LorentzVector &, const LorentzVector &);
    LorentzVector &operator -=(LorentzVector &, const LorentzVector &);
    LorentzVector &operator *=(LorentzVector &, const float &);

    LorentzVector operator +(const LorentzVector &, const LorentzVector &);
    LorentzVector operator -(const LorentzVector &, const LorentzVector &);

    bool operator ==(const LorentzVector &, const LorentzVector &);
    bool operator !=(const LorentzVector &, const LorentzVector &);

    Vector &operator +=(Vector &, const Vector &);
    Vector &operator -=(Vector &, const Vector &);

    Vector operator +(const Vector &, const Vector &);
    Vector operator -(const Vector &, const Vector &);

    bool operator ==(const Vector &, const Vector &);
    bool operator !=(const Vector &, const Vector &);

    double operator *(const LorentzVector &, const LorentzVector &);

    double dphi(const LorentzVector &, const LorentzVector &);
    double dr(const LorentzVector &, const LorentzVector &);

    double et(const LorentzVector &);
    double eta(const LorentzVector &);
    double mass(const LorentzVector &);
    double momentum(const LorentzVector &);
    double phi(const LorentzVector &);
    double pt(const LorentzVector &);

    bool operator ==(const Event_Extra &, const Event_Extra &);
}

#endif
