import FWCore.ParameterSet.Config as cms

from PhysicsTools.SelectorUtils.pfElectronSelector_cfi import pfElectronSelector
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector
from PhysicsTools.SelectorUtils.pfMuonSelector_cfi import pfMuonSelector

InputMaker = cms.EDAnalyzer(
    'InputMaker',
    fileName = cms.string("input.pb"),

    jets = cms.string("goodPatJetsPFlow"),
    electrons = cms.string("selectedPatElectronsLoosePFlow"),
    muons = cms.string("selectedPatMuonsLoosePFlow"),
    primary_vertices = cms.string("offlinePrimaryVertices"),
    missing_energies = cms.string("patMETsPFlow"),

    input_type = cms.string("unknown"),

    electron_selector = pfElectronSelector.clone(),
    jet_selector = pfJetIDSelector.clone(),
    muon_selector = pfMuonSelector.clone()
)
