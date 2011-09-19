// Compare user applied Jet Energy Corrections to PAT
//
// Created by Samvel Khalatyan, Jul 13, 2011
// Copyright 2011, All rights reserved

#include <sstream>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Modules/interface/JetCorrectionService.h"

#include "bsm_input_maker/maker/interface/JetEnergyCorrection.h"

using edm::Handle;
using edm::InputTag;
using edm::LogInfo;
using edm::LogWarning;
using edm::ParameterSet;

using bsm::JetEnergyCorrection;

using namespace std;

JetEnergyCorrection::JetEnergyCorrection(const ParameterSet &config)
{
    _jets_tag = config.getParameter<InputTag>("jets");
    _rho_tag = config.getParameter<InputTag>("rho");
}



// Privates
//
void JetEnergyCorrection::analyze(const edm::Event &event,
                        const edm::EventSetup &setup)
{
    using pat::JetCollection;

    Handle<double> rho;
    event.getByLabel(_rho_tag, rho);

    if (!rho.isValid())
    {
        LogWarning("JetEnergyCorrection") << "failed to extract rho";

        return;
    }

    Handle<JetCollection> jets;
    event.getByLabel(_jets_tag, jets);

    if (!jets.isValid())
    {
        LogWarning("JetEnergyCorrection") << "failed to extract jets";

        return;
    }

    /*
    const JetCorrector* corrector =
        JetCorrector::getJetCorrector(JetCorrectionService, setup);
        */

    for(JetCollection::const_iterator jet = jets->begin();
            jets->end() != jet;
            ++jet)
    {
        /*
        int index = jet - jets->begin();
        edm::RefToBase<reco::Jet> jetRef(edm::Ref<JetCollection>(jets, index));
        double jec = corrector->correction(*jet, jetRef, event, setup);
        */

        std::ostringstream message;

        message << "pat corrected    p4: " << jet->p4() << endl;
        message << "pat corrected l0 p4: " << jet->correctedP4(0) << endl;
        message << "pat corrected l1 p4: " << jet->correctedP4(1) << endl;
        message << "pat corrected l2 p4: " << jet->correctedP4(2) << endl;
        message << "pat corrected l3 p4: " << jet->correctedP4(3) << endl;
        message << "correction l1      : " << " p4: " << endl;
        message << "correction l1+l2   : " << " p4: " << endl;
        //message << "correction l1+l2+l3: " << jec << " p4: " << jet->correctedP4(0) * jec << endl;

        LogWarning("JetEnergyCorrection") << message.str();

        /*
        utility::set(pb_jet->mutable_physics_object()->mutable_p4(),
                &jet->p4());
        utility::set(pb_jet->mutable_physics_object()->mutable_vertex(),
            &jet->vertex());

        utility::set(pb_jet->mutable_uncorrected_p4(),
                &jet->correctedP4(0));

        pb_jet->mutable_extra()->set_area(jet->jetArea());
        */
    }
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out, const math::XYZTLorentzVector &p4)
{
    return out << "pt: " << p4.Pt() << " eta: " << p4.Eta();
}

DEFINE_FWK_MODULE(JetEnergyCorrection);
