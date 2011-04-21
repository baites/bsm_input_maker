import FWCore.ParameterSet.Config as cms

InputMaker = cms.EDAnalyzer(
    'InputMaker',
    fileName = cms.string("input.pb"),

    jets = cms.string("goodPatJetsCA8PF"),
    electrons = cms.string("selectedPatElectrons"),
    muons = cms.string("selectedPatMuons")
)
