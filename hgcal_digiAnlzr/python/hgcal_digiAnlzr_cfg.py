import FWCore.ParameterSet.Config as cms

#from HGCalCommissioning.SystemTestEventFilters.configure_sysval_raw2digi_cff import *
#from HGCalCommissioning.Configuration.configure_sysval_reco_cff import *
#from HGCalCommissioning.NanoTools.configure_sysval_nano_cff import *
##from HGCalCommissioning.Geometry.HGCalProducerDatabaseGen_cfi import *
##from HGCalCommissioning.Geometry.HGCalProducerSimHit_cfi import *
#from HGCalCommissioning.DQM.hgcalSysValDQM_cff import *
#from HGCalCommissioning.Configuration.SysValEras_cff import *
#from Geometry.HGCalMapping.hgcalmapping_cff import *#customise_hgcalmapper
from HGCalCommissioning.Configuration.ErasTB2025_cff import *

process = cms.Process("HGC")

# Message logger
process.load("Geometry.HGCalMapping.hgcalmapping_cff")
process.load('HGCalCommissioning.Configuration.ErasTB2025_cff')
#process.load('Geometry.HGCalMapping.hgCalMappingESProducer_cfi')
#process.load('Geometry.HGCalGeometry.HGCalGeometryESProducer_cfi')

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
#process.load('Configuration.EventContent.EventContent_cff')
#process.load('Configuration.StandardSequences.EndOfProcess_cff')

#process.load('Configuration.Geometry.GeometryDD4hepExtended2026D99_cff')
#process.load('Configuration.Geometry.GeometryDD4hepExtended2026D99Reco_cff')

#process.load('Configuration.Geometry.GeometryExtended2026D99Reco_cff')
from Geometry.HGCalMapping.hgcalmapping_cff import customise_hgcalmapper
#process = customise_hgcalmapper(process)
#process.load('Configuration.Geometry.GeometryExtended2025Reco_cff')

#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

# Input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "file:RAW2DIGI_110723_1.root"
    )
)

#process.TFileService = cms.Service(
#    "TFileService",
#    fileName = cms.string("HGCalDigi_outCheck.root")
#)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    

)

process.hits = cms.EDAnalyzer("hgcal_digiAnlzr",
    #recHits = cms.InputTag("HGCalRecHit","HGCEERecHits")
    #simHitsToken = cms.InputTag("g4SimHits", "HGCHitsEE")
    hgcalDigis = cms.untracked.InputTag("hgcalDigis","","RAW2DQM"),
    #econds = cms.untracked.InputTag("hgcalDigis","","RAW2DQM")
    metaData = cms.InputTag("hgcalTrigTimeProducer","","RAW2DQM")
)

process.p = cms.Path(process.hits)

