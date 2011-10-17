import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('python')

options.register('useData',
        False,
        VarParsing.multiplicity.singleton,
        VarParsing.varType.int,
        "Use Data global tag")

options.parseArguments();

process = cms.Process("jectxt")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

# define your favorite global tag
global_tag = 'START42_V13::All'
if options.useData:
    global_tag = 'GR_R_42_V12::All'

process.GlobalTag.globaltag = global_tag
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")
process.readAK5PF    = cms.EDAnalyzer('JetCorrectorDBReader',  
        # below is the communication to the database 
        payloadName    = cms.untracked.string('AK5PFchs'),

        # this is used ONLY for the name of the printed txt files. You can use
        # any name that you like, but it is recommended to use the GT name that
        # you retrieved the files from.
        globalTag      = cms.untracked.string(global_tag.split(':')[0]),

        printScreen    = cms.untracked.bool(False),
        createTextFile = cms.untracked.bool(True)
        )

process.p = cms.Path(process.readAK5PF)
