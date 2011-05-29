// Define Physics objects algebra
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_ALGEBRA
#define BSM_IO_ALGEBRA

namespace bsm
{
    class LorentzVector;
    class Vector;

    LorentzVector &operator +=(LorentzVector &, const LorentzVector &);
    LorentzVector &operator -=(LorentzVector &, const LorentzVector &);

    Vector &operator +=(Vector &, const Vector &);
    Vector &operator -=(Vector &, const Vector &);

    double operator *(const LorentzVector &, const LorentzVector &);

    double mass(const LorentzVector &);
}

#endif
