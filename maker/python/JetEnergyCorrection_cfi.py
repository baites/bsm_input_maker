import FWCore.ParameterSet.Config as cms

JetEnergyCorrection = cms.EDAnalyzer(
    'JetEnergyCorrection',

    jets = cms.InputTag("goodPatJetsPFlow"),
    rho = cms.InputTag("kt6PFJetsPFlow", "rho")
)
