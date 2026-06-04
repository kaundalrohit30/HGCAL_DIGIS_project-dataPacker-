// -*- C++ -*-
//
// Package:    DigiAnalyzer/hgcal_digiAnlzr
// Class:      hgcal_digiAnlzr
//
/**\class hgcal_digiAnlzr hgcal_digiAnlzr.cc DigiAnalyzer/hgcal_digiAnlzr/plugins/hgcal_digiAnlzr.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Rohit Kaundal
//         Created:  Thu, 28 May 2026 13:36:46 GMT
//
//

// system include files
#include <memory>
#include <iostream>
#include <sstream>
#include <string>


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/ESGetToken.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

//#include "DataFormats/NanoAOD/interface/FlatTable.h"

//Meta data (trigger, ...)
#include "HGCalCommissioning/SystemTestEventFilters/interface/HGCalTestSystemMetaData.h"
//DetId
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
//Digi information
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
//mapping information
#include "CondFormats/DataRecord/interface/HGCalElectronicsMappingRcd.h"
#include "CondFormats/DataRecord/interface/HGCalDenseIndexInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalMappingParameterHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalECONDPacketInfoSoA.h"
#include "DataFormats/HGCalDigi/interface/HGCalECONDPacketInfoHost.h"
#include "Geometry/HGCalMapping/interface/HGCalMappingTools.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <iostream>

#include "TTree.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class hgcal_digiAnlzr : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit hgcal_digiAnlzr(const edm::ParameterSet&);
  ~hgcal_digiAnlzr() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
  edm::EDGetTokenT<hgcaldigi::HGCalDigiHost> digisToken_;
  edm::EDGetTokenT<hgcaldigi::HGCalECONDPacketInfoHost> econdInfoTkn_;
  edm::EDGetTokenT<HGCalTestSystemTrigTimeCollection> trigtimeToken_;
  edm::EDGetTokenT<HGCalTestSystemMCP> mcpToken_;
  edm::EDGetTokenT<HGCalTestSystemTimingIn> timeinToken_;

  edm::ESGetToken<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd> denseIndexInfoTkn_;
  edm::ESGetToken<hgcal::HGCalMappingCellParamHost, HGCalElectronicsMappingRcd> cellTkn_;
  edm::ESGetToken<hgcal::HGCalMappingModuleParamHost, HGCalElectronicsMappingRcd> moduleTkn_;

  
  //std::vector<uint16_t>  tctp, adc, adcm1, tot, toa, cm, flags, channel, fedId, fedReadoutSeq;
  TTree* tree;
  int eventNum;

  std::vector<uint16_t> tctp ,adc, adcm1 ,tot ,toa ,cm ,flags ,channel ,fedId ,fedReadoutSeq;

  std::vector<int> chI1  ,chI2  ,modI1  ,modI2  ,chType;

  std::vector<uint8_t> isSiPM, iscalib;


//  edm::EDGetTokenT<TrackCollection> tracksToken_;  //used to select what tracks to read from configurationfile
//#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  //edm::ESGetToken<SetupData, SetupRecord> setupToken_;
//#endif
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
hgcal_digiAnlzr::hgcal_digiAnlzr(const edm::ParameterSet& iConfig)
    : digisToken_(consumes<hgcaldigi::HGCalDigiHost>(iConfig.getUntrackedParameter<edm::InputTag>("hgcalDigis"))),
    econdInfoTkn_(consumes<hgcaldigi::HGCalECONDPacketInfoHost>(iConfig.getUntrackedParameter<edm::InputTag>("hgcalDigis"))),
    trigtimeToken_(consumes<HGCalTestSystemTrigTimeCollection>(iConfig.getParameter<edm::InputTag>("metaData"))),
    mcpToken_(consumes<HGCalTestSystemMCP>(iConfig.getParameter<edm::InputTag>("metaData"))),
    timeinToken_(consumes<HGCalTestSystemTimingIn>(iConfig.getParameter<edm::InputTag>("metaData"))),
    denseIndexInfoTkn_(esConsumes<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd>()),
    cellTkn_(esConsumes()),
    moduleTkn_(esConsumes())

    {
    #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
      setupDataToken_ = esConsumes<SetupData, SetupRecord>();
    #endif

    edm::Service<TFileService> fs;
  
    tree = new TTree("Hits","HGCal_Digi_Info");

    tree->Branch("eventNum",&eventNum);
    tree->Branch("eventNum",&eventNum);
    //tree->Branch("energy",&hit_energy);
    tree->Branch("tctp",&tctp);
    tree->Branch("adc",&adc);
    tree->Branch("adcm1",&adcm1);
    //tree->Branch("detID",&detId);

    tree->Branch("tot",&tot);
    tree->Branch("toa",&toa);
    tree->Branch("cm",&cm);
    tree->Branch("flags",&flags);

    tree->Branch("channel",&channel);
    tree->Branch("fedId",&fedId);
    tree->Branch("fedReadoutSeq",&fedReadoutSeq);

    tree->Branch("chI1",&chI1);
    tree->Branch("chI2",&chI2);
    tree->Branch("modI1",&modI1);
    tree->Branch("modI2",&modI2);

    tree->Branch("isSiPM",&isSiPM);
    tree->Branch("iscalib",&iscalib);
  
    }

hgcal_digiAnlzr::~hgcal_digiAnlzr() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called for each event  ------------
void hgcal_digiAnlzr::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  // if the SetupData is always needed
  auto setup = iSetup.getData(setupToken_);
  // if need the ESHandle to check if the SetupData was there or not
  auto pSetup = iSetup.getHandle(setupToken_);
  #endif

  using namespace edm;
  using namespace std;

  eventNum = iEvent.id().event();
  tctp.clear();
  adc.clear();
  adcm1.clear();
  tot.clear();
  toa.clear();
  cm.clear();
  flags.clear();
  channel.clear();
  fedId.clear();
  fedReadoutSeq.clear();
  chI1.clear();
  chI2.clear();
  modI1.clear();
  modI2.clear();
  chType.clear();
  isSiPM.clear();
  iscalib.clear();

  const auto& digis = iEvent.getHandle(digisToken_);
  auto const& digis_view = digis->const_view();
  //int32_t ndigis = digis->const_view().metadata().size();

  auto const& denseIndexInfo = iSetup.getData(denseIndexInfoTkn_);
  auto const& denseIndexInfo_view = denseIndexInfo.const_view();
  int32_t ndenseIndices = denseIndexInfo_view.metadata().size();

  const auto& econdInfo = iEvent.getHandle(econdInfoTkn_);
  auto const& econdInfo_view = econdInfo->const_view();


  auto const& cellInfo = iSetup.getData(cellTkn_);
  auto const& cellInfo_view = cellInfo.const_view();
  auto const& moduleInfo = iSetup.getData(moduleTkn_);
  auto const& moduleInfo_view = moduleInfo.const_view();

  int32_t ndigis = 0;
  if(digis.isValid()){
  ndigis = digis->const_view().metadata().size();
  //assert(ndigis == ndenseIndices);
  
  }
  
  //if(eventNum == 1){
    cout << "EventNo.>  " << eventNum << endl;
    cout << "ndigis:> " << ndigis << "  ndenseIndices:> " << ndenseIndices << endl;
    /*cout << std::left
     << std::setw(10) << "tctp"
     << std::setw(10) << "adc"
     << std::setw(10) << "adcm1"
     << std::setw(10) << "tot"
     << std::setw(10) << "toa"
     << std::setw(10) << "cm"
     << std::setw(10) << "flags"
     << std::setw(10) << "channel"
     << std::setw(10) << "fedId"
     << std::setw(15) << "fedReadoutSeq"
     << std::setw(10) << "chI1"
     << std::setw(10) << "chI2"
     << std::setw(10) << "modI1"
     << std::setw(10) << "modI2"
     << std::setw(10) << "chType"
     << std::setw(10) << "isSiPM"
     << std::setw(10) << "iscalib"
     << '\n';*/
  for (int32_t i = 0; i < ndenseIndices && digis.isValid(); i++) {
      
      //cout << digis_view.chNumber()[i] << endl;
      tctp.push_back(digis_view.tctp()[i]);
      adc.push_back(digis_view.adcm1()[i]);
      adcm1.push_back(digis_view.adc()[i]);
      tot.push_back(digis_view.tot()[i]);
      toa.push_back(digis_view.toa()[i]);
      cm.push_back(digis_view.cm()[i]);
      flags.push_back(digis_view.flags()[i]);
      channel.push_back(denseIndexInfo_view.chNumber()[i]);
      fedId.push_back(denseIndexInfo_view.fedId()[i]);
      fedReadoutSeq.push_back(denseIndexInfo_view.fedReadoutSeq()[i]);
      uint32_t cellInfoIdx(denseIndexInfo_view.cellInfoIdx()[i]);
      chType.push_back(cellInfo_view.t()[cellInfoIdx]);
      chI1.push_back(cellInfo_view.i1()[cellInfoIdx]);
      chI2.push_back(cellInfo_view.i2()[cellInfoIdx]);   
      uint32_t modInfoIdx(denseIndexInfo_view.modInfoIdx()[i]);
      modI1.push_back(moduleInfo_view.i1()[modInfoIdx]);
      modI2.push_back(moduleInfo_view.i2()[modInfoIdx]); 
      isSiPM.push_back((uint8_t) moduleInfo_view.isSiPM()[modInfoIdx]);
      iscalib.push_back(cellInfo_view.iscalib()[cellInfoIdx]);
     
    /*cout << std::left
     << std::setw(10) << static_cast<unsigned int>(digis_view.tctp()[i])
     << std::setw(10) << digis_view.adc()[i]
     << std::setw(10) << digis_view.adcm1()[i]
     << std::setw(10) << digis_view.tot()[i]
     << std::setw(10) << digis_view.toa()[i]
     << std::setw(10) << digis_view.cm()[i]
     << std::setw(10) << digis_view.flags()[i]
     << std::setw(10) << denseIndexInfo_view.chNumber()[i]
     << std::setw(10) << denseIndexInfo_view.fedId()[i]
     << std::setw(15) << denseIndexInfo_view.fedReadoutSeq()[i]
     << std::setw(10) << cellInfo_view.i1()[cellInfoIdx]
     << std::setw(10) << cellInfo_view.i2()[cellInfoIdx]
     << std::setw(10) << moduleInfo_view.i1()[modInfoIdx]
     << std::setw(10) << moduleInfo_view.i2()[modInfoIdx]
     << std::setw(10) << cellInfo_view.t()[cellInfoIdx]
     << std::setw(10) << moduleInfo_view.isSiPM()[modInfoIdx]
     << std::setw(10) << cellInfo_view.iscalib()[cellInfoIdx]
     << '\n';*/

    }
  
    int32_t necons = 0;
  if(econdInfo.isValid()){
      necons = econdInfo->const_view().metadata().size();
      //assert(ndigis == ndenseIndices);
  }

  //cout << "necons:>  " << necons << endl;
  std::vector<uint16_t> payloads(necons);
  std::vector<std::vector<uint32_t>> cmsums(12);
  for(size_t ierx=0; ierx<12; ierx++) cmsums[ierx].resize(necons,0);
  for(int imod=0; imod<necons; imod++){
    const auto econd = econdInfo_view[imod];
    payloads[imod] = econd.payloadLength();
    //cout << "Econ:>  " << imod << "   " << econd.payloadLength() <<  "   BX:>  " << econd.BX() << "   L1A:>  " << static_cast<unsigned int>(econd.L1A()) << "   Orbit:>  " << static_cast<unsigned int>(econd.Orbit()) << endl;
    //cout << typeid(econd.L1A()).name() << endl;
    //cout << typeid(econd.Orbit()).name() << endl;
    for(size_t ierx=0; ierx<12; ierx++){
      cmsums[ierx][imod] = econd.cm().coeff(ierx,0) + econd.cm().coeff(ierx,1);
      //cout << econd.cm().coeff(ierx,0) + econd.cm().coeff(ierx,1) << endl;
    }

  }

  //}

  tree->Fill();

}

// ------------ method called once each job just before starting event loop  ------------
void hgcal_digiAnlzr::beginJob() {
  // please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop  ------------
void hgcal_digiAnlzr::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void hgcal_digiAnlzr::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

}

//define this as a plug-in
DEFINE_FWK_MODULE(hgcal_digiAnlzr);
