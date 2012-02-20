// Select electrons with loose selection
//
// Created by Samvel Khalatyan, Oct 10, 2011
// Copyright 2011, All rights reserved

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "bsm_input_maker/maker/interface/ElectronSelector.h"

using namespace bsm;
using namespace edm;
using namespace pat;

ElectronSelector::ElectronSelector(const edm::InputTag &electron_tag):
    Selector(electron_tag)
{
}

bool ElectronSelector::init(const edm::Event *event)
{
    _electron.clear();

    // Apply loose electron id and save pointers in event
    //
    Handle<ElectronCollection> electrons;
    event->getByLabel(tag(), electrons);

    bool result = false;
    if (!electrons.isValid())
    {
        LogWarning("ElectronSelector")
            << "failed to extract electrons. Check Input Tag: "
            << tag();
    }
    else
    {
        for(ElectronCollection::const_iterator electron = electrons->begin();
                electrons->end() != electron;
                ++electron)
        {
            if (30 < electron->pt()
                    && 2.5 > fabs(electron->eta()))
            {
                _electron.push_back(&*electron);
            }
        }

        result = true;
    }

    return result;
}

const ElectronSelector::Electrons &ElectronSelector::electron() const
{
    return _electron;
}
