import FWCore.ParameterSet.Config as cms
import copy
from FWCore.ParameterSet.VarParsing import VarParsing
from Geometry.VeryForwardGeometry.geometryRPFromDB_cfi import * # TODO it looks like it can be deleted


# TEMP config:
apply_shift = True                                        
saveToDQM = False


process = cms.Process('PPSTiming2') # or TIMINGSTUDY??
options = VarParsing ('analysis')

process.verbosity = cms.untracked.PSet( input = cms.untracked.int32(-1) ) # TODO: confirm if this is needed.

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.threshold = ''
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

options.register ('outputFileName',
		  'run_output.root',
		  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "output file name")

options.register ('calibInput',
				  '',
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.string,
				  "Calibration input file for this iteration")

options.register ('planesConfig',
				  'planes.json',
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.string,
				  "planes config file")

options.register ('validOOT',
				  -1,
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.int,
				  "valid OOT slice")	
options.register('useSQLFile', 
                    '',
				  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  'useSQLFileAsACalibration'
)

options.register('sqlFileName', 
                    '',
				  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  'sqlFileNameForCalib'
)

options.register('maxEventsToProcess', 
                    -1,
				  VarParsing.multiplicity.singleton,
                  VarParsing.varType.int,
                  'maxEvents to process'
)


			  				  
options.parseArguments()

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.maxEventsToProcess))


if options.useSQLFile=='True':
    use_sqlite_file=True
elif options.useSQLFile=='False':
    use_sqlite_file=False
else:
    print('Provide true or false for useSQLLiteFile')
    exit()

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Geometry.VeryForwardGeometry.geometryRPFromDB_cfi') #TODO: use geometry form DB not from file 
# process.load('Geometry.VeryForwardGeometry.geometryRPFromDD_2022_cfi')

process.source = cms.Source ("PoolSource",
                             fileNames = cms.untracked.vstring(
    # "file:/eos/home-m/mobrzut/357/734/00000/73c5eeec-578d-438f-92f1-3e1fff2ed390.root",   
    # "file:/eos/home-m/mobrzut/357/734/00000/08e5ee70-ebff-4571-b14c-806961a3d9dd.root",
    # "file:/eos/home-m/mobrzut/357/734/00000/d42eaf2c-57bb-48fe-9a22-317053f89115.root"
# "file:/eos/home-m/mobrzut/357/479/00000/68a5a64a-b756-4cf5-9a2a-00181e34f501.root"
# RUN 440 - "357440": [[1,354]],
"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/440/00000/53746080-d21b-4fdf-9d19-67c6dae347ac.root",
"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/440/00000/b8d3f012-0b59-43b4-adbc-811bcb59c9c4.root",
"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/440/00000/cfa9f82a-5296-49cf-8353-11f8761b675c.root",
"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/440/00000/db3c6c33-9ad0-4d61-9a64-a1aa9578550e.root"


# RUN 442 - "357442": [[1,1392]],
# "/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/19b41c28-cc6e-4df9-b73a-ae8b187021c7.root",
# "/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/4d2b38b9-f03f-4887-ad72-e53e448cc52d.root"
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/5987c37e-850f-4c00-9e69-d6a3ed8df910.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/9450abb5-9478-4209-b1dd-7ff06beab620.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/a2536da7-b15f-4d3c-be30-9feefb303f41.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/a2f71e31-b36b-4a05-bb14-17e6b404ceec.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/ca8d0e9c-5ac7-4987-ba7b-b771cd933485.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/e6a28ca6-f978-430c-bacf-7b0d59f8c069.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/eac6865b-3265-4d9d-aee9-579dc95e7f63.root",
#"/store/data/Run2022C/AlCaPPSPrompt/ALCARECO/PPSCalMaxTracks-PromptReco-v1/000/357/442/00000/ee2f7caa-dcb5-4ab3-b6cf-b302031dd105.root"
)
)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
# 2022 prompt: to be updated
process.GlobalTag = GlobalTag(process.GlobalTag, "124X_dataRun3_v9") 


#JH - use new tag for timing calibrations
if (use_sqlite_file):                                        
        process.load('CondCore.CondDB.CondDB_cfi')
        process.CondDB.connect = options.sqlFileName # SQLite input TODO: migrate to using tag
        process.PoolDBESSource = cms.ESSource('PoolDBESSource',
                process.CondDB,
                DumpStats = cms.untracked.bool(True),
                toGet = cms.VPSet(
                    cms.PSet(
                        record = cms.string('PPSTimingCalibrationRcd'),
                        tag = cms.string('DiamondTimingCalibration'),
                                label = cms.untracked.string('PPSTestCalibration'), 
                    )
                )
)

if options.calibInput == '':
    print('') 
    #TODO: uncomment below when delete sqlite file dependency 
    # process.GlobalTag.toGet = cms.VPSet(
    #     cms.PSet(record = cms.string('PPSTimingCalibrationRcd'),
    #                 tag = cms.string('PPSDiamondTimingCalibration_Run3_v1_hlt'), # working tag: PPSDiamondTimingCalibration_Run3_v1_hlt
    #                 #TODO: old tag PPSDiamondTimingCalibration_v1  - to delete
    #             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
    #             )
    # )
else:
    process.ppsTimingCalibrationESSource = cms.ESSource('PPSTimingCalibrationESSource',
        calibrationFile = cms.string(options.calibInput),
        subDetector = cms.uint32(2),
        appendToDataLabel = cms.string('')
    )

# TODO: we miss tjhis from worker.py vvvvv
#  rechits production
#process.load('RecoPPS.Local.ctppsDiamondLocalReconstruction_cff')
# ^^^^^^^^^^^^^^^^^^^^^^^^^

# JH - rerun reco sequence with new timing conditions
process.load("RecoPPS.Configuration.recoCTPPS_cff")
process.ctppsDiamondLocalTracks.recHitsTag = cms.InputTag("ctppsDiamondRecHits","","PPSTiming2")
process.ctppsLocalTrackLiteProducer.tagDiamondTrack = cms.InputTag("ctppsDiamondLocalTracks","","PPSTiming2")
process.ctppsProtons.tagLocalTrackLite = cms.InputTag("ctppsLocalTrackLiteProducer","","PPSTiming2")
process.ctppsLocalTrackLiteProducer.includeDiamonds = cms.bool(True)
process.ctppsLocalTrackLiteProducer.tagPixelTrack = cms.InputTag("ctppsPixelLocalTracksAlCaRecoProducer")
process.ctppsLocalTrackLiteProducer.includePixels = cms.bool(True)
process.ctppsDiamondRecHits.timingCalibrationTag=cms.string("PoolDBESSource:PPSTestCalibration")
process.ctppsDiamondRecHits.digiTag= cms.InputTag("ctppsDiamondRawToDigiAlCaRecoProducer:TimingDiamond")

# from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
# if(apply_shift):
#     process.diamondTimingWorker = DQMEDAnalyzer("DiamondTimingWorker",
#         tagDigi = cms.InputTag("ctppsDiamondRawToDigiAlCaRecoProducer", "TimingDiamond"),
#         tagRecHit = cms.InputTag("ctppsDiamondRecHits","","PPSTiming2"),
#         tagPixelLocalTrack = cms.InputTag("ctppsPixelLocalTracksAlCaRecoProducer"),
#         timingCalibrationTag=cms.string("PoolDBESSource:PPSTestCalibration"),
#         tagLocalTrack = cms.InputTag("ctppsDiamondLocalTracks","","PPSTiming2"),
#         tagValidOOT = cms.int32(-1), #TODO: remove parameter from options or don't hardcode it. 
#         planesConfig = cms.string("planes.json"), #TODO: remove parameter from options or don't hardcode it. 
#         Ntracks_Lcuts = cms.vint32([-1,1,-1,1]), # minimum number of tracks in pots [45-210, 45-220, 56-210, 56-220]
#         Ntracks_Ucuts = cms.vint32([-1,6,-1,6]), # maximum number of tracks in pots [45-210, 45-220, 56-210, 56-220]
#     )
# else:
#     process.diamondTimingWorker = DQMEDAnalyzer("DiamondTimingWorker",
#     tagDigi = cms.InputTag("ctppsDiamondRawToDigiAlCaRecoProducer", "TimingDiamond"),
#     tagRecHit = cms.InputTag("ctppsDiamondRecHitsAlCaRecoProducer"),
#     tagPixelLocalTrack = cms.InputTag("ctppsPixelLocalTracksAlCaRecoProducer"),
#     tagLocalTrack = cms.InputTag("ctppsDiamondLocalTracksAlCaRecoProducer"),
#     timingCalibrationTag=cms.string("PoolDBESSource:PPSTestCalibration"),
#     tagValidOOT = cms.int32(-1), #TODO: remove parameter from options or don't hardcode it. 
#     planesConfig = cms.string("planes.json"),
#     Ntracks_Lcuts = cms.vint32([-1,1,-1,1]), # minimum number of tracks in pots [45-210, 45-220, 56-210, 56-220]
#     Ntracks_Ucuts = cms.vint32([-1,6,-1,6]), # maximum number of tracks in pots [45-210, 45-220, 56-210, 56-220]
# ) 

if(apply_shift):
    process.ALL = cms.Path(
        # Re-run the PPS local+proton reconstruction starting from AOD with new timing calibrations
        process.ctppsDiamondRecHits *
        process.ctppsDiamondLocalTracks
        # process.diamondTimingWorker
                        )
else: 
    process.ALL = cms.Path(process.diamondTimingWorker)

if(saveToDQM):
    process.load("DQMServices.Core.DQMStore_cfi")
    process.load("DQMServices.Components.DQMEnvironment_cfi")
    process.dqmEnv.subSystemFolder = "CalibPPS"
    process.dqmSaver.convention = 'Offline'
    process.dqmSaver.workflow = "/CalibPPS/TimingCalibration/CMSSW_12_0_0_pre2"
    process.dqmSaver.saveByRun = -1
    process.dqmSaver.saveAtJobEnd = True
    process.dqmSaver.forceRunNumber = 999999


    process.dqmOutput = cms.OutputModule("DQMRootOutputModule",
        fileName = cms.untracked.string(options.outputFileName)
    )

    process.load("DQMServices.Components.EDMtoMEConverter_cff")
    process.end_path = cms.EndPath(process.dqmSaver)
else:
    process.dqmOutput = cms.OutputModule("DQMRootOutputModule", fileName=cms.untracked.string(options.outputFileName))
    process.end_path = cms.EndPath(process.dqmOutput)


process.schedule = cms.Schedule(process.ALL,   process.end_path)

#print(process.dumpPython())
