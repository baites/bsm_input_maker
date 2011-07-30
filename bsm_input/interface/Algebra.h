// Define Physics objects algebra
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_ALGEBRA
#define BSM_IO_ALGEBRA

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    extern const float pi;

    // Lorentz Vector algebra
    //
    LorentzVector &operator +=(LorentzVector &, const LorentzVector &);
    LorentzVector &operator -=(LorentzVector &, const LorentzVector &);
    LorentzVector &operator *=(LorentzVector &, const float &);

    LorentzVector operator +(const LorentzVector &, const LorentzVector &);
    LorentzVector operator -(const LorentzVector &, const LorentzVector &);

    bool operator ==(const LorentzVector &, const LorentzVector &);
    bool operator !=(const LorentzVector &, const LorentzVector &);

    float operator *(const LorentzVector &, const LorentzVector &);

    float dphi(const LorentzVector &, const LorentzVector &);
    float dr(const LorentzVector &, const LorentzVector &);

    float et(const LorentzVector &);
    float eta(const LorentzVector &);
    float mass(const LorentzVector &);
    float momentum(const LorentzVector &);
    float phi(const LorentzVector &);
    float pt(const LorentzVector &);
    float ptrel(const LorentzVector &v1, const LorentzVector &v2);
    float ptrel2(const LorentzVector &v1, const LorentzVector &v2);



    // Vector algebra
    //
    Vector toVector(const LorentzVector &);

    float magnitude(const Vector &);

    Vector &operator +=(Vector &, const Vector &);
    Vector &operator -=(Vector &, const Vector &);

    Vector operator +(const Vector &, const Vector &);
    Vector operator -(const Vector &, const Vector &);

    bool operator ==(const Vector &, const Vector &);
    bool operator !=(const Vector &, const Vector &);

        // Dot multiplication
        //
    float operator *(const Vector &, const Vector &);

        // Cross product
        //
    Vector cross(const Vector &, const Vector &);



    // Miscellaneous
    //
    bool operator ==(const Event_Extra &, const Event_Extra &);
}

#endif
