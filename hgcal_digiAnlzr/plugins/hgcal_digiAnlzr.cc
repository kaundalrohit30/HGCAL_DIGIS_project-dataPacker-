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
  
  //std::vector<uint16_t>  tctp, adc, adcm1, tot, toa, cm, flags, channel, fedId, fedReadoutSeq;
  TTree* tree;
  int eventNum;

  std::vector<uint16_t> tctp;
  std::vector<uint16_t> adc;
  std::vector<uint16_t> adcm1;
  std::vector<uint16_t> tot;
  std::vector<uint16_t> toa;
  std::vector<uint16_t> cm;
  std::vector<uint16_t> flags;
  


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
    denseIndexInfoTkn_(esConsumes<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd>()) 
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

  const auto& digis = iEvent.getHandle(digisToken_);
  auto const& digis_view = digis->const_view();
  const auto& econdInfo = iEvent.getHandle(econdInfoTkn_);
  auto const& econdInfo_view = econdInfo->const_view();

  auto const& denseIndexInfo = iSetup.getData(denseIndexInfoTkn_);
  auto const& denseIndexInfo_view = denseIndexInfo.const_view();
  int32_t ndenseIndices = denseIndexInfo_view.metadata().size();

  int32_t ndigis = 0;
  if(digis.isValid()){
      ndigis = digis->const_view().metadata().size();
      assert(ndigis == ndenseIndices);
      
  }

  
  if(eventNum == 1){
    cout << "EventNo.>  " << eventNum << endl;
    cout << "ndigis:> " << ndigis << endl;
  for (int32_t i = 0; i < ndigis && digis.isValid(); i++) {
      //if(i == 50) break;
      cout << /*digis.isValid() <<*/ " tctp:> " << static_cast<unsigned int>(digis_view.tctp()[i]) << " ADC:> " << digis_view.adc()[i] << "   ADCm1:> " << digis_view.adcm1()[i] << "  TOT:>  " <<  digis_view.tot()[i] << "  TOA:> " << digis_view.toa()[i] << "   cm:>  " << digis_view.cm()[i] << "   flags:>  " << digis_view.flags()[i] << endl;
      //cout << digis_view.chNumber()[i] << endl;
      tctp.push_back(digis_view.tctp()[i]);
      adc.push_back(digis_view.adcm1()[i]);
      adcm1.push_back(digis_view.adc()[i]);
      tot.push_back(digis_view.tot()[i]);
      toa.push_back(digis_view.toa()[i]);
      cm.push_back(digis_view.cm()[i]);
      flags.push_back(digis_view.flags()[i]);
  
    }
  
    int32_t necons = 0;
  if(econdInfo.isValid()){
      necons = econdInfo->const_view().metadata().size();
      //assert(ndigis == ndenseIndices);
  }

  cout << "necons:>  " << necons << endl;
  std::vector<uint16_t> payloads(necons);
  std::vector<std::vector<uint32_t>> cmsums(12);
  for(size_t ierx=0; ierx<12; ierx++) cmsums[ierx].resize(necons,0);
  for(int imod=0; imod<necons; imod++){
    const auto econd = econdInfo_view[imod];
    payloads[imod] = econd.payloadLength();
    cout << "Econ:>  " << imod << "   " << econd.payloadLength() <<  "   BX:>  " << econd.BX() << "   L1A:>  " << static_cast<unsigned int>(econd.L1A()) << "   Orbit:>  " << static_cast<unsigned int>(econd.Orbit()) << endl;
    //cout << typeid(econd.L1A()).name() << endl;
    //cout << typeid(econd.Orbit()).name() << endl;
    for(size_t ierx=0; ierx<12; ierx++){
      cmsums[ierx][imod] = econd.cm().coeff(ierx,0) + econd.cm().coeff(ierx,1);
      //cout << econd.cm().coeff(ierx,0) + econd.cm().coeff(ierx,1) << endl;
    }

  }

  }

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

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //edm::ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks", edm::InputTag("ctfWithMaterialTracks"));
  //descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(hgcal_digiAnlzr);
