// Define Physics objects algebra
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#include <cmath>
#include <cfloat>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/Algebra.h"

using bsm::LorentzVector;
using bsm::Vector;

const float bsm::pi = 3.14159265358979323846;

// Lorentz Vector algebra
//
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
    float energy = v1.e() - v2.e();
    if (0 >= energy)
        v1.set_e(0);
    else
        v1.set_e(energy);

    v1.set_px(v1.px() - v2.px());
    v1.set_py(v1.py() - v2.py());
    v1.set_pz(v1.pz() - v2.pz());

    return v1;
}

LorentzVector &bsm::operator *=(LorentzVector &v1, const float &jec)
{
    v1.set_e(v1.e() * jec);
    v1.set_px(v1.px() * jec);
    v1.set_py(v1.py() * jec);
    v1.set_pz(v1.pz() * jec);

    return v1;
}

LorentzVector bsm::operator +(const LorentzVector &v1, const LorentzVector &v2)
{
    LorentzVector v;

    v += v1;
    v += v2;

    return v;
}

LorentzVector bsm::operator -(const LorentzVector &v1, const LorentzVector &v2)
{
    LorentzVector v;

    v += v1;
    v -= v2;

    return v;
}

bool bsm::operator ==(const LorentzVector &v1, const LorentzVector &v2)
{
    return v1.e() == v2.e()
        && v1.px() == v2.px()
        && v1.py() == v2.py()
        && v1.pz() == v2.pz();
}

bool bsm::operator !=(const LorentzVector &v1, const LorentzVector &v2)
{
    return !(v1 == v2);
}

float bsm::operator *(const LorentzVector &v1, const LorentzVector &v2)
{
    return v1.e() * v2.e()
        - v1.px() * v2.px()
        - v1.py() * v2.py()
        - v1.pz() * v2.pz();
}

float bsm::dphi(const LorentzVector &v1, const LorentzVector &v2)
{
    float result = phi(v1) - phi(v2);

    while(result >= pi)
        result -= pi;

    while(result < -pi)
        result += pi;

    return result;
}

float bsm::dr(const LorentzVector &v1, const LorentzVector &v2)
{
    float delta_phi = dphi(v1, v2);
    float eta1 = eta(v1);
    float eta2 = eta(v2);

    if (eta1 == -eta2
            && DBL_MAX == fabs(eta1))
        return DBL_MAX;

    float delta_eta = eta1 - eta2;

    return sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
}

float bsm::et(const LorentzVector &v)
{
    float p = momentum(v);

    if (0 == p
            || 0 == v.pz())
    {
        return 0;
    }
    else
    {
        return v.e() * pt(v) / p;
    }
}

float bsm::eta(const LorentzVector &v)
{
    if (0 == v.px()
            && 0 == v.py())
    {
        // zero pT
        //
        if (0 <= v.pz())
            return DBL_MAX;
        else
            return -DBL_MAX;
    }
    else
    {
        float cosine_theta = v.pz() / momentum(v);

        return -0.5 * log( ( 1 - cosine_theta) / (1 + cosine_theta) );
    }
}

float bsm::mass(const LorentzVector &v)
{
    float magnitude = v * v;

    return (0 >= magnitude)
        ? 0
        : sqrt(magnitude);
}

float bsm::momentum(const LorentzVector &v)
{
    return sqrt(v.px() * v.px()
            + v.py() * v.py()
            + v.pz() * v.pz());
}

float bsm::phi(const LorentzVector &v)
{
    if (v.px())
        return atan2(v.py(), v.px());

    if (0 == v.py())
        return 0;
    else if (0 < v.py())
        return pi / 2;
    else
        return -pi / 2;
}

float bsm::pt(const LorentzVector &v)
{
    return sqrt(v.px() * v.px() + v.py() * v.py());
}

float bsm::ptrel(const LorentzVector &v1, const LorentzVector &v2)
{
    Vector pt1 = toVector(v1);
    Vector pt2 = toVector(v2);

    const float pt1_momentum = momentum(v1);
    const float pt2_momentum = momentum(v2);
    const float pt1_dot_pt2 = pt1 * pt2;

    return pt2_momentum
        ? sqrt((pt1_momentum * pt1_momentum)
                - (pt1_dot_pt2 * pt1_dot_pt2) / (pt2_momentum * pt2_momentum))
        : 0;
}



// Vector algebra
//
Vector bsm::toVector(const LorentzVector &v)
{
    Vector result;

    result.set_x(v.px());
    result.set_y(v.py());
    result.set_z(v.pz());

    return result;
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

Vector bsm::operator +(const Vector &v1, const Vector &v2)
{
    Vector v;

    v += v1;
    v += v2;

    return v;
}

Vector bsm::operator -(const Vector &v1, const Vector &v2)
{
    Vector v;

    v += v1;
    v -= v2;

    return v;
}

bool bsm::operator ==(const Vector &v1, const Vector &v2)
{
    return v1.x() == v2.x()
        && v1.y() == v2.y()
        && v1.z() == v2.z();
}

bool bsm::operator !=(const Vector &v1, const Vector &v2)
{
    return !(v1 == v2);
}

float bsm::operator *(const Vector &v1, const Vector &v2)
{
    return v1.x() * v2.x()
        + v1.y() * v2.y()
        + v1.z() * v2.z();
}



// Miscellaneous
//
bool bsm::operator ==(const Event_Extra &e1, const Event_Extra &e2)
{
    return e1.run() == e2.run()
        && e1.lumi() == e2.lumi()
        && e1.id() == e2.id();
}
