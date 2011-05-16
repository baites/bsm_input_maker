import FWCore.ParameterSet.Config as cms

InputMaker = cms.EDAnalyzer(
    'InputMaker',
    fileName = cms.string("input.pb"),

    jets = cms.string("goodPatJetsPFlow"),

    pf_electrons = cms.string("selectedPatElectronsLoosePFlow"),
    gsf_electrons = cms.string("selectedPatElectrons"),

    pf_muons = cms.string("selectedPatMuonsLoosePFlow"),
    reco_muons = cms.string("selectedPtMuons"),

    primary_vertices = cms.string("goodOfflinePrimaryVertices"),
    missing_energies = cms.string("patMETsPFlow"),

    input_type = cms.string("unknown")
)
