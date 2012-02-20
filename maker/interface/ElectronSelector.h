// Select electrons with loose selection
//
// Created by Samvel Khalatyan, Oct 10, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ELECTRON_SELECTOR
#define BSM_ELECTRON_SELECTOR

#include <vector>

#include "bsm_input_maker/maker/interface/Selector.h"

namespace pat
{
    class Electron;
}

namespace bsm
{
    class ElectronSelector: public Selector
    {
        public:
            ElectronSelector(const edm::InputTag &electron_tag);

            typedef std::vector<const pat::Electron *> Electrons;
            
            virtual bool init(const edm::Event *);

            const Electrons &electron() const;

        private:
            Electrons _electron;
    };
}

#endif
