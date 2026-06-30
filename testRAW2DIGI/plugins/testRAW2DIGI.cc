// -*- C++ -*-
//
// Package:    RawDataBufferProducer/testRAW2DIGI
// Class:      testRAW2DIGI
//
/**\class testRAW2DIGI testRAW2DIGI.cc RawDataBufferProducer/testRAW2DIGI/plugins/testRAW2DIGI.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Rohit Kaundal
//         Created:  Tue, 30 Jun 2026 06:01:31 GMT
//
//

// system include files
#include <memory>
#include <algorithm> // for std::min
#include <string> // for std::string, std::to_string()

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/FEDRawData/interface/RawDataBuffer.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalECONDPacketInfoHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalFEDPacketInfoHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"

#include "CondFormats/DataRecord/interface/HGCalElectronicsMappingRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalMappingModuleIndexer.h"
#include "CondFormats/HGCalObjects/interface/HGCalMappingCellIndexer.h"
#include "CondFormats/DataRecord/interface/HGCalModuleConfigurationRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalConfiguration.h"

#include "EventFilter/HGCalRawToDigi/interface/HGCalUnpacker.h"


//
// class declaration
//

class testRAW2DIGI : public edm::stream::EDProducer<> {
public:
  explicit testRAW2DIGI(const edm::ParameterSet&);
  ~testRAW2DIGI() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  //void beginStream(edm::StreamID) override;
  void produce(edm::Event&, const edm::EventSetup&) override;
  //void endStream() override;

  void beginRun(edm::Run const&, edm::EventSetup const&) override;
  void endRun(edm::Run const&, edm::EventSetup const&) override;
  //void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------
  // input tokens
  const edm::EDGetTokenT<RawDataBuffer> fedRawToken_;

  // output tokens
  const edm::EDPutTokenT<hgcaldigi::HGCalDigiHost> digisToken_;
  const edm::EDPutTokenT<hgcaldigi::HGCalECONDPacketInfoHost> econdPacketInfoToken_;
  const edm::EDPutTokenT<hgcaldigi::HGCalFEDPacketInfoHost> fedPacketInfoToken_;
  
  // config tokens and objects
  edm::ESWatcher<HGCalElectronicsMappingRcd> mapWatcher_;
  edm::ESGetToken<HGCalMappingCellIndexer, HGCalElectronicsMappingRcd> cellIndexToken_;
  edm::ESGetToken<HGCalMappingModuleIndexer, HGCalElectronicsMappingRcd> moduleIndexToken_;
  edm::ESGetToken<HGCalConfiguration, HGCalModuleConfigurationRcd> configToken_; 
  HGCalMappingCellIndexer cellIndexer_;
  HGCalMappingModuleIndexer moduleIndexer_;
  HGCalConfiguration config_;
  HGCalUnpacker unpacker_;
  std::map<uint32_t, std::vector<uint32_t> > aveadc_map_;

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
testRAW2DIGI::testRAW2DIGI(const edm::ParameterSet& iConfig)
  : fedRawToken_(consumes<RawDataBuffer>(iConfig.getParameter<edm::InputTag>("src"))),
      digisToken_(produces<hgcaldigi::HGCalDigiHost>()),
      econdPacketInfoToken_(produces<hgcaldigi::HGCalECONDPacketInfoHost>()),
      fedPacketInfoToken_(produces<hgcaldigi::HGCalFEDPacketInfoHost>()),
      cellIndexToken_(esConsumes<edm::Transition::BeginRun>()),
      moduleIndexToken_(esConsumes<edm::Transition::BeginRun>()),
      configToken_(esConsumes<edm::Transition::BeginRun>()) { }



testRAW2DIGI::~testRAW2DIGI() {
  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

void testRAW2DIGI::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {
  if (mapWatcher_.check(iSetup)) {
    moduleIndexer_ = iSetup.getData(moduleIndexToken_);
    cellIndexer_ = iSetup.getData(cellIndexToken_);
    config_ = iSetup.getData(configToken_);
  }
}

void testRAW2DIGI::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {
  std::cout << ">>> TestHGCalRawToDigi::endRun: " << std::endl;
  std::cout << "  Dense indices  |  ADC averaged over channels" << std::endl;
  std::cout << "  fed econd  eRx |";
  std::size_t nevts = std::min(20,int(aveadc_map_[0].size()));
  for (std::size_t i = 0; i < nevts; ++i) {
    std::cout << std::setw(6) << ("evt" + std::to_string(i+1));
  }
  std::cout << std::endl;
  for (unsigned fedId = 0; fedId < moduleIndexer_.fedCount(); ++fedId) {
    const auto econdMax = moduleIndexer_.getNumModules(fedId);
    for (uint32_t econdIdx = 0; econdIdx < econdMax; econdIdx++) {
      const auto erxMax = moduleIndexer_.getNumERxs(fedId,econdIdx);
      for (uint32_t erxIdx = 0; erxIdx < erxMax; erxIdx++) {
        uint32_t eRxDenseIdx = moduleIndexer_.getIndexForModuleErx(fedId, econdIdx, erxIdx);
        std::cout << std::setw(5) << fedId << std::setw(6) << econdIdx << std::setw(5) << erxIdx << " |";
        for (std::size_t i = 0; i < nevts; ++i) {
          std::cout << std::setw(6) << aveadc_map_[eRxDenseIdx][i];
        }
        std::cout << std::endl;
      } // close loop over eRx ROCs
    } // close loop over ECON-Ds
  } // close loop over FEDs
}


// ------------ method called to produce the data  ------------
void testRAW2DIGI::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  std::cout << ">>> TestHGCalRawToDigi: Event " << iEvent.id()
            << " ========================================================================================" << std::endl;
  hgcaldigi::HGCalDigiHost digis(cms::alpakatools::host(), moduleIndexer_.maxDataSize());
  hgcaldigi::HGCalECONDPacketInfoHost econdPacketInfo(cms::alpakatools::host(), moduleIndexer_.maxModulesCount());
  hgcaldigi::HGCalFEDPacketInfoHost fedPacketInfo(cms::alpakatools::host(), moduleIndexer_.fedCount());
  
  // CREATE DIGIs
  // std::cout << "Created DIGIs SOA with " << digis.view().metadata().size() << " entries" << std::endl;
  const auto& fedBuffer = iEvent.get(fedRawToken_);
  for (unsigned fedId = 0; fedId < moduleIndexer_.fedCount(); ++fedId) {
    const auto& fed_data = fedBuffer.fragmentData(fedId);
    //std::cout << "FED " << fedId
    //      << " fragment size = "
    //      << fed_data.size() << std::endl;

    if (fed_data.size() == 0)
      continue;
    std::cout << "Calling parseFEDData()" << std::endl;
    unpacker_.parseFEDData(fedId, fed_data, moduleIndexer_, config_, digis, fedPacketInfo, econdPacketInfo, /*headerOnlyMode*/ false);
  }
  
  // CHECK DIGIs
  for (unsigned fedId = 0; fedId < moduleIndexer_.fedCount(); ++fedId) {
    //std::cout << "fed=" << fedId << std::endl;
    const auto econdMax = moduleIndexer_.getNumModules(fedId);
    for (uint32_t econdIdx = 0; econdIdx < econdMax; econdIdx++) {
      //std::cout << "fed=" << fedId << ", econdIdx=" << econdIdx << std::endl;
      const auto erxMax = moduleIndexer_.getNumERxs(fedId,econdIdx);
      //std::cout << "   fed econd   eRx  chan |  tctp adcm1   adc   tot   toa    cm  flags" << std::endl;
      for (uint32_t erxIdx = 0; erxIdx < erxMax; erxIdx++) {
        uint32_t eRxDenseIdx = moduleIndexer_.getIndexForModuleErx(fedId, econdIdx, erxIdx);
        //std::cout << "   erxIdx=" << erxIdx << ", eRxDenseIdx=" << eRxDenseIdx << std::endl;
        uint32_t aveadc = 0, nchans = 0; // averaged over channels
        for (uint32_t channelIdx = 0; channelIdx < HGCalMappingCellIndexer::maxChPerErx_; channelIdx++) {
          uint32_t denseIdx = moduleIndexer_.getIndexForModuleData(fedId, econdIdx, erxIdx, channelIdx);
          if(digis.view()[denseIdx].flags()!=hgcal::DIGI_FLAG::NotAvailable) {
            aveadc += digis.view()[denseIdx].adc();
            nchans++;
          }
          //std::cout << ">>> HGCalUnpacker:    channelIdx= " << channelIdx << ", denseIdx = " << denseIdx
          //          << ", ADC=" << adc << std::endl;
          /*std::cout << std::dec << std::setfill(' ')
                    << std::setw(6) << fedId << std::setw(6) << econdIdx
                    << std::setw(6) << erxIdx << std::setw(6) << channelIdx << " |"
                    << std::setw(6) << (uint32_t) digis.view()[denseIdx].tctp()
                    << std::setw(6) << digis.view()[denseIdx].adcm1() << std::setw(6) << digis.view()[denseIdx].adc()
                    << std::setw(6) << digis.view()[denseIdx].tot()   << std::setw(6) << digis.view()[denseIdx].toa() 
                    << std::setw(6) << digis.view()[denseIdx].cm()
                    << " 0x" << std::hex << std::setfill('0') << std::setw(4) << digis.view()[denseIdx].flags()
                    << std::dec << std::setfill(' ') << std::endl;*/
        }
        aveadc *= 1./nchans; //HGCalMappingCellIndexer::maxChPerErx_;
        //if (aveadc_map_.find(eRxDenseIdx)!=aveadc_map_.end())
        //  aveadc_map_[eRxDenseIdx] = { };
        aveadc_map_[eRxDenseIdx].push_back(aveadc);
      } // close loop over eRx ROCs
    } // close loop over ECON-Ds
  } // close loop over FEDs

}



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void testRAW2DIGI::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("rawDataCollector"));
  desc.add<std::vector<unsigned int> >("fedIds", {});
  descriptions.add("hgcalDigis", desc);

}

//define this as a plug-in
DEFINE_FWK_MODULE(testRAW2DIGI);
