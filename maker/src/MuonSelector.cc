// Select muons with loose selection
//
// Created by Samvel Khalatyan, Oct 10, 2011
// Copyright 2011, All rights reserved

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "bsm_input_maker/maker/interface/MuonSelector.h"

using namespace bsm;
using namespace edm;
using namespace pat;
using namespace std;

MuonSelector::MuonSelector(const edm::InputTag &muon_tag,
        const edm::InputTag &primary_vertex_tag):
    Selector(muon_tag),
    _primary_vertex_tag(primary_vertex_tag)
{
}

bool MuonSelector::init(const edm::Event *event)
{
    _muon.clear();

    typedef vector<reco::Vertex> PrimaryVertices;

    Handle<PrimaryVertices> primary_vertices;
    event->getByLabel(primaryVertexTag(), primary_vertices);

    Handle<MuonCollection> muons;
    event->getByLabel(tag(), muons);

    bool result = false;
    if (!primary_vertices.isValid())
    {
        LogWarning("MuonSelector")
            << "failed to extract primary vertices. Check Input Tag: "
            << primaryVertexTag();
    }
    else if (!muons.isValid())
    {
        LogWarning("MuonSelector")
            << "failed to extract muons. Check Input Tag: "
            << tag();
    }
    else if (primary_vertices->empty())
    {
        result = true;
    }
    else
    {
        const reco::Vertex *primary_vertex =
            &*primary_vertices->begin();
        for(MuonCollection::const_iterator muon = muons->begin();
                muons->end() != muon;
                ++muon)
        {
            if (35 < muon->pt()
                    && 2.1 > fabs(muon->eta())
                    && muon->isGlobalMuon()
                    && muon->isTrackerMuon()
                    && 1 < muon->numberOfMatches()
                    && 0 < muon->globalTrack()->hitPattern().numberOfValidMuonHits()
                    && 10 > muon->globalTrack()->normalizedChi2()
                    && 10 < muon->innerTrack()->numberOfValidHits()
                    && 0 < muon->innerTrack()->hitPattern().pixelLayersWithMeasurement()
                    && 0.02 > fabs(muon->dB())
                    && 1 > fabs(primary_vertex->position().z()
                        - muon->vertex().z()))
            {
                _muon.push_back(&*muon);
            }
        }

        result = true;
    }

    return result;
}

const MuonSelector::Muons &MuonSelector::muon() const
{
    return _muon;
}

const edm::InputTag &MuonSelector::primaryVertexTag() const
{
    return _primary_vertex_tag;
}
