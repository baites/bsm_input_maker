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

    LorentzVector operator +(const LorentzVector &, const LorentzVector &);
    LorentzVector operator -(const LorentzVector &, const LorentzVector &);

    Vector &operator +=(Vector &, const Vector &);
    Vector &operator -=(Vector &, const Vector &);

    Vector operator +(const Vector &, const Vector &);
    Vector operator -(const Vector &, const Vector &);

    double operator *(const LorentzVector &, const LorentzVector &);

    double et(const LorentzVector &);
    double eta(const LorentzVector &);
    double mass(const LorentzVector &);
    double momentum(const LorentzVector &);
    double pt(const LorentzVector &);
}

#endif
