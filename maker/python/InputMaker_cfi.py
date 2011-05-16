import FWCore.ParameterSet.Config as cms

InputMaker = cms.EDAnalyzer(
    'InputMaker',
    fileName = cms.string("input.pb"),

    jets = cms.string("goodPatJetsPFlow"),
    electrons = cms.string("selectedPatElectronsLoosePFlow"),
    muons = cms.string("selectedPatMuonsLoosePFlow"),
    primary_vertices = cms.string("offlinePrimaryVertices"),
    missing_energies = cms.string("patMETsPFlow"),

    input_type = cms.string("unknown")
)
