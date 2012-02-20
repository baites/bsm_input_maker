// Select jets with loose selection
//
// Created by Samvel Khalatyan, Oct 10, 2011
// Copyright 2011, All rights reserved

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "bsm_input_maker/maker/interface/JetSelector.h"

using namespace bsm;
using namespace edm;
using namespace pat;
using namespace std;

JetSelector::JetSelector(const InputTag &jet_tag,
        const InputTag &primary_vertex_tag,
        const InputTag &rho_tag,
        const JECFiles &jec_files):
    Selector(jet_tag),
    _primary_vertex_tag(primary_vertex_tag),
    _rho_tag(rho_tag)
{
    vector<JetCorrectorParameters> corrections;
    for(JECFiles::const_iterator file = jec_files.begin();
            jec_files.end() != file;
            ++file)
    {
        LogWarning("JetSelector")
            << "Load JEC: " << *file;

        corrections.push_back(JetCorrectorParameters(*file));
    }

    _jec.reset(new FactorizedJetCorrector(corrections));
}

bool JetSelector::init(const Event *event,
        const Electrons &electrons,
        const Muons &muons)
{
    _jet.clear();

    typedef vector<reco::Vertex> PrimaryVertices;
    typedef math::XYZTLorentzVector LorentzVector;

    // Extract Primary Vertices, jets, rho
    //
    Handle<PrimaryVertices> primary_vertices;
    event->getByLabel(primaryVertexTag(), primary_vertices);

    Handle<JetCollection> jets;
    event->getByLabel(tag(), jets);

    Handle<double> rho;
    event->getByLabel(rhoTag(), rho);

    bool result = false;
    if (!primary_vertices.isValid())
    {
        LogWarning("JetSelector")
            << "failed to extract primary vertices. Check Input Tag: "
            << primaryVertexTag();
    }
    else if (!jets.isValid())
    {
        LogWarning("JetSelector")
            << "failed to extract jets. Check Input Tag: "
            << tag();
    }
    else if (!rho.isValid())
    {
        LogWarning("JetSelector")
            << "failed to extract rho. Check Input Tag: "
            << rhoTag();
    }
    else
    {
        // Clean up jets: remove leptons
        //
        for(JetCollection::const_iterator jet = jets->begin();
                jets->end() != jet;
                ++jet)
        {
            const LorentzVector *pat_p4 = &jet->p4();
            LorentzVector raw_p4 = jet->correctedP4(0);

            // Remove leptons from the jet
            //
            for(Electrons::const_iterator e = electrons.begin();
                    electrons.end() != e;
                    ++e)
            {
                if (0.5 >= reco::deltaR((*e)->eta(),
                            (*e)->phi(),
                            pat_p4->eta(),
                            pat_p4->phi()))
                {
                    raw_p4 -= (*e)->p4();
                }
            }

            for(Muons::const_iterator m = muons.begin();
                    muons.end() != m;
                    ++m)
            {
                if (0.5 >= reco::deltaR((*m)->eta(),
                            (*m)->phi(),
                            pat_p4->eta(),
                            pat_p4->phi()))
                {
                    raw_p4 -= (*m)->p4();
                }
            }

            _jec->setJetEta(raw_p4.eta());
            _jec->setJetPt(raw_p4.pt());
            _jec->setJetE(raw_p4.e());
            _jec->setNPV(primary_vertices->size());
            _jec->setJetA(jet->jetArea());
            _jec->setRho(*rho);

            const double &correction = _jec->getCorrection();
            raw_p4 *= correction;

            if (50 < raw_p4.pt()
                    && 2.4 > fabs(raw_p4.eta()))
            {
                _jet.push_back(&*jet);
            }
        }

        result = true;
    }

    return result;
}

const JetSelector::Jets &JetSelector::jet() const
{
    return _jet;
}

const edm::InputTag &JetSelector::primaryVertexTag() const
{
    return _primary_vertex_tag;
}

const edm::InputTag &JetSelector::rhoTag() const
{
    return _rho_tag;
}
