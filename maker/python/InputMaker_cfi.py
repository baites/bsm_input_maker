import FWCore.ParameterSet.Config as cms

InputMaker = cms.EDAnalyzer(
    'InputMaker',
    fileName = cms.string("input.pb"),

    gen_particles = cms.string("prunedGenParticles"),
    gen_particle_levels = cms.uint32(2),

    jets = cms.string("goodPatJetsPFlow"),
    rho = cms.string("kt6PFJetsPFlow"),

    pf_electrons = cms.string("selectedPatElectronsLoosePFlow"),
    gsf_electrons = cms.string("selectedPatElectrons"),

    pf_muons = cms.string("selectedPatMuonsLoosePFlow"),
    reco_muons = cms.string("selectedPatMuons"),

    primary_vertices = cms.string("goodOfflinePrimaryVertices"),
    missing_energies = cms.string("patMETsPFlow"),

    hlts = cms.string("TriggerResults::REDIGI311X"),
    hlt_pattern = cms.string("hlt_ele*caloid*caloiso*,hlt_ht*ele*caloid*"),

    input_type = cms.string("unknown")
)
