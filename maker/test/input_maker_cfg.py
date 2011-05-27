import os

import FWCore.ParameterSet.Config as cms

process = cms.Process("MCInputMaker")

#-- Message Logger ------------------------------------------------------------
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories.append('PATSummaryTables')
process.MessageLogger.cerr = cms.untracked.PSet(
    default          = cms.untracked.PSet(limit = cms.untracked.int32(-1)),
    PATSummaryTables = cms.untracked.PSet(limit = cms.untracked.int32(-1)),
    FwkReport = cms.untracked.PSet(
        reportEvery = cms.untracked.int32(1000)
    )
)

#-- Input Source --------------------------------------------------------------
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)

if (os.path.exists("input.txt")):
    for file in open("input.txt").readlines():
        process.source.fileNames.append("dcap:///pnfs/cms/WAX/11" + file.strip())

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(1000)
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

#-- Analysis ------------------------------------------------------------------
process.load("bsm_input_maker.maker.InputMaker_cfi")
process.InputMaker.input_type = cms.string("ttbar")
process.InputMaker.hlt_pattern = cms.string("hlt_isomu*")

process.p = cms.Path(
    process.InputMaker
)

import FWCore.ParameterSet.printPaths as pp      
pp.printPaths(process)
