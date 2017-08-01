import os, sys
from multiprocessing import Process
from WMCore.Configuration import Configuration

#### Basic crab config structure

config = Configuration()

pyCfgParams = ['filterOnHLT=True',
               'metRecoilCut=250']

config.section_('General')
config.General.transferLogs = False
config.General.workArea     = 'crab_projects_MC_80X'  # Make sure you set this parameter

config.section_('JobType')
config.JobType.psetName         = '../makeAnalysisNtuples.py'
config.JobType.pluginName       = 'Analysis'
config.JobType.outputFiles      = ['tree.root']
config.JobType.allowUndistributedCMSSW = True


config.section_('Data')    
config.Data.inputDBS      = 'global'
config.Data.splitting     = 'EventAwareLumiBased'
config.Data.unitsPerJob   = 25000
config.Data.outLFNDirBase = '/store/group/phys_exotica/monojet/rgerosa/PhysicsObjectSchool/'
config.Data.allowNonValidInputDataset = True

config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'

config.JobType.pyCfgParams = list(pyCfgParams)
