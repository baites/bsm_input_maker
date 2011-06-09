// Define Physics objects algebra
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#include <cmath>
#include <cfloat>

#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/Algebra.h"

using bsm::LorentzVector;
using bsm::Vector;

const double bsm::pi = 3.14159265358979323846;

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
    double energy = v1.e() - v2.e();
    if (0 >= energy)
        v1.set_e(0);
    else
        v1.set_e(energy);

    v1.set_px(v1.px() - v2.px());
    v1.set_py(v1.py() - v2.py());
    v1.set_pz(v1.pz() - v2.pz());

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

double bsm::operator *(const LorentzVector &v1, const LorentzVector &v2)
{
    return v1.e() * v2.e()
        - v1.px() * v2.px()
        - v1.py() * v2.py()
        - v1.pz() * v2.pz();
}

double bsm::dphi(const LorentzVector &v1, const LorentzVector &v2)
{
    double result = phi(v1) - phi(v2);

    while(result >= pi)
        result -= pi;

    while(result < -pi)
        result += pi;

    return result;
}

double bsm::dr(const LorentzVector &v1, const LorentzVector &v2)
{
    double delta_phi = dphi(v1, v2);
    double eta1 = eta(v1);
    double eta2 = eta(v2);

    if (eta1 == -eta2
            && DBL_MAX == fabs(eta1))
        return DBL_MAX;

    double delta_eta = eta1 - eta2;

    return sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
}

double bsm::et(const LorentzVector &v)
{
    double p = momentum(v);

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

double bsm::eta(const LorentzVector &v)
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
        double cosine_theta = v.pz() / momentum(v);

        return -0.5 * log( ( 1 - cosine_theta) / (1 + cosine_theta) );
    }
}

double bsm::mass(const LorentzVector &v)
{
    double magnitude = v * v;

    return (0 >= magnitude)
        ? 0
        : sqrt(magnitude);
}

double bsm::momentum(const LorentzVector &v)
{
    return sqrt(v.px() * v.px()
            + v.py() * v.py()
            + v.pz() * v.pz());
}

double bsm::phi(const LorentzVector &v)
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

double bsm::pt(const LorentzVector &v)
{
    return sqrt(v.px() * v.px() + v.py() * v.py());
}
