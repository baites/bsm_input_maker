// Utility methods
//
// Created by Samvel Khalatyan, Jun 01, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_UTILITY
#define BSM_IO_UTILITY

#include <iosfwd>

namespace bsm
{
    class LorentzVector;
    class Vector;

    std::ostream &operator <<(std::ostream &, const LorentzVector &);
    std::ostream &operator <<(std::ostream &, const Vector &);
}

#endif
