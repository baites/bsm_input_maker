import FWCore.ParameterSet.Config as cms

InputMaker = cms.EDAnalyzer(
    'InputMaker',

    # ProtoBuf output filename mask
    #
    output_filename = cms.string("input.pb"),

    gen_particle = cms.InputTag("prunedGenParticles::PAT"),
    
    # Define, now deep in the decay tree the gen-particles should be searched
    #
    gen_particle_depth_level = cms.uint32(2),

    jet = cms.InputTag("goodPatJetsPFlow::PAT"),
    rho = cms.InputTag("kt6PFJetsPFlow:rho:PAT"),

    electron = cms.InputTag("selectedPatElectronsLoosePFlow::PAT"),
    muon = cms.InputTag("selectedPatMuonsLoosePFlow::PAT"),

    primary_vertex = cms.InputTag("goodOfflinePrimaryVertices::PAT"),
    missing_energy = cms.InputTag("patMETsPFlow::PAT"),

    hlt = cms.InputTag("TriggerResults::HLT"),
    trigger_event = cms.InputTag("hltTriggerSummaryAOD::HLT"),

    # Regular expression for path patterns to be saved
    #
    hlt_path_pattern = cms.string("^hlt_ele.*caloid.*caloiso.*$"),

    # Pattern for HLT producers to be saved
    #
    hlt_producer_pattern = cms.string("^.*$"),

    # Patternf for path filters to be saved
    #
    hlt_filter_pattern = cms.string("^.*$"),

    input_type = cms.string("unknown")
)
