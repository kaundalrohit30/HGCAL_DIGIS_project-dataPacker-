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
//Data packing
#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"
#include "SimCalorimetry/HGCalSimAlgos/interface/HGCalRawDataPackingTools.h"
//#include "​SimCalorimetry/​HGCalSimAlgos/​interface/​SlinkTypes.h"
//#include "EventFilter/HGCalRawToDigi/interface/HGCalECONDEmulator.h"
//#include "EventFilter/HGCalRawToDigi/interface/HGCalFrameGenerator.h"


#include <iostream>

#include "TTree.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//

/*struct ECONDHeader {
  uint32_t word0;
  uint32_t word1;
};

ECONDHeader makeECONDHeader(uint16_t payloadLength,
                            uint16_t bx,
                            uint16_t l1a,
                            uint8_t orbit) {
  ECONDHeader hdr{0, 0};

  constexpr uint32_t HEADER_MARKER = 0x1FE;

  // Word 0
  hdr.word0 |= (HEADER_MARKER & 0x1FF) << 23;
  hdr.word0 |= (payloadLength & 0x1FF) << 14;

  // P,E,H/T,E/B/O,M,T,Hamming left as zero

  // Word 1
  hdr.word1 |= (bx & 0xFFF) << 20;
  hdr.word1 |= (l1a & 0x3F) << 14;
  hdr.word1 |= (orbit & 0x7) << 11;

  // S and RR left as zero

  return hdr;
}

struct eRxHeader {
  uint32_t word0;
  uint32_t word1;
};

eRxHeader makeERxHeader(const std::vector<uint8_t>& channelMap, uint16_t cm0, uint16_t cm1) {
  eRxHeader hdr{0, 0};

  hdr.word0 |= (cm0 & 0x3FF) << 15;
  hdr.word0 |= (cm1 & 0x3FF) << 5;
  // Channel map bits
  for (size_t i = 0; i < channelMap.size(); i++) {
    if (i<32) {
      hdr.word1 |= (channelMap[i] & 0x1) << i;
    }
    else if(i>=32){
      hdr.word0 |= (channelMap[i] & 0x1) << (i-32);
    }
  }

  return hdr;
}*/

/*struct ERxData {
     uint32_t cm0{0}, cm1{0};
     std::vector<uint8_t> tctp;  // vector of hgcal::econd::ToTStatus
     std::vector<uint16_t> adc, adcm, toa, tot;
     std::vector<uint32_t> meta;  ///< additional words accompanying the e-rx data
     uint32_t crc32{0};
   };*/


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

  std::vector<uint16_t> tctp ,adc, adcm1 ,tot ,toa ,cm ,flags ,channel ,fedId ,fedReadoutSeq, payloadLength, BX, L1A;

  std::vector<int> chI1  ,chI2  ,modI1  ,modI2  ,chType;

  std::vector<uint8_t> isSiPM, iscalib, Orbit;

  std::vector<uint16_t> cm0, cm1;

  //std::vector<uint32_t> allDataWords;
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
    tree->Branch("payloadLength",&payloadLength);

    tree->Branch("chI1",&chI1);
    tree->Branch("chI2",&chI2);
    tree->Branch("modI1",&modI1);
    tree->Branch("modI2",&modI2);

    tree->Branch("isSiPM",&isSiPM);
    tree->Branch("iscalib",&iscalib);
    tree->Branch("BX",&BX);
    tree->Branch("L1A",&L1A);
    tree->Branch("Orbit",&Orbit);

    tree->Branch("cm0",&cm0);
    tree->Branch("cm1",&cm1);



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
  payloadLength.clear();
  chI1.clear();
  chI2.clear();
  modI1.clear();
  modI2.clear();
  chType.clear();
  isSiPM.clear();
  iscalib.clear();
  BX.clear();
  L1A.clear();
  Orbit.clear();
  cm0.clear();
  cm1.clear();
  //allDataWords.clear();

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

  std::vector<hgcal::econd::ERxData> allERxData(72);
  std::vector<hgcal::econd::ERxChannelEnable> enableMaps(
    72,
    hgcal::econd::ERxChannelEnable(37,false)
  );
  
  using Digi = HGCROCChannelDataFrame<uint32_t>;

  for (int32_t i = 0; i < ndenseIndices && digis.isValid(); i++) {
      
    //cout << digis_view.chNumber()[i] << endl;
    tctp.push_back(digis_view.tctp()[i]);
    adc.push_back(digis_view.adc()[i]);
    adcm1.push_back(digis_view.adcm1()[i]);
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
     

    int erx = i/37;
    int chInERx = i%37;
    //cout << "ERx: " << erx << ", Channel in ERx: " << chInERx << endl;

    allERxData[erx].tctp.push_back(digis_view.tctp()[i]);
    allERxData[erx].adc.push_back(digis_view.adc()[i]);
    allERxData[erx].adcm.push_back(digis_view.adcm1()[i]);
    allERxData[erx].toa.push_back(digis_view.toa()[i]);
    allERxData[erx].tot.push_back(digis_view.tot()[i]);

    enableMaps[erx][chInERx] = true;
      /*bool tc = (digis_view.tctp()[i] >> 1) & 0x1;
      bool tp = digis_view.tctp()[i] & 0x1;
      

      Digi digi;

      digi.fill(
          false,   
          tc,
          tp,   
          digis_view.adcm1()[i],     
          digis_view.adc()[i],     
          digis_view.tot()[i],
          digis_view.toa()[i]      
      );

      uint32_t word = digi.raw();*/
      //cout << "Raw word:>  " << std::hex << word << std::dec << endl;
      //allDataWords.push_back(word);
      

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

  for(int imod=0; imod<necons; imod++){
    const auto econd = econdInfo_view[imod];
    //payloads[imod] = econd.payloadLength();
    payloadLength.push_back(econd.payloadLength());
    BX.push_back(econd.BX());
    L1A.push_back(econd.L1A());
    Orbit.push_back(econd.Orbit());

    //cout << "nEcond = " << imod <<  "  BX = " << econd.BX() << "  L1A = " << static_cast<unsigned int>(econd.L1A()) << "  Orbit = " << static_cast<unsigned int>(econd.Orbit()) << endl;
    
    /*auto hdr = makeECONDHeader(
    econd.payloadLength(),
    econd.BX(),
    econd.L1A(),
    econd.Orbit()
    );

    std::cout << std::hex
          << "Header Word0 = 0x" << hdr.word0 << "\n"
          << "Header Word1 = 0x" << hdr.word1 << "\n"
          << std::dec;*/

    for(size_t ierx=0; ierx<6; ierx++){
      //cmsums[ierx][imod] = econd.cm().coeff(ierx,0) + econd.cm().coeff(ierx,1);
      //cout << econd.cm().coeff(ierx,0) + econd.cm().coeff(ierx,1) << endl;
      cm0.push_back(econd.cm().coeff(ierx,0));
      cm1.push_back(econd.cm().coeff(ierx,1));

      //cout << "ierx: " << ierx << "  cm0: " << econd.cm().coeff(ierx,0) << "  cm1: " << econd.cm().coeff(ierx,1) << endl;
    }

  }
  tree->Fill();

  /////////////// eRx payload + header generation ///////////////

  std::vector<uint32_t> econdPacket{0}; // for now only with Econd header + eRx header + payload (without ECOND trailer and Idle word)
  econdPacket.clear();
  int econdCounter = 0;
  uint16_t header =  170;
  uint8_t ht = 0;
  uint8_t ebo = 0;
  uint8_t ehHam = 0;
  uint8_t rr = 0;
  //for(int j = 0; j < 5; j++){
  for(size_t erx = 0; erx < allERxData.size(); ++erx){   //looping over all 72 eRx

    if(erx % 6 == 0){  //Each ECOND have 6 eRX input (LD modules)
      const auto econdHeader = hgcal::econd::eventPacketHeader(header,
                                                              payloadLength[econdCounter]-1, // since we are not have trailer in the econdpacket so the payload length is reduced by 1 (ideally it should be (37+2)*6 + 1 = 235)
                                                              false,
                                                              true,
                                                              ht,
                                                              ebo,
                                                              true,
                                                              false,
                                                              ehHam,
                                                              BX[econdCounter],
                                                              L1A[econdCounter],
                                                              Orbit[econdCounter],
                                                              false,
                                                              rr);
      econdPacket.push_back(econdHeader[0]);  //filling EcondHeaders
      econdPacket.push_back(econdHeader[1]);
      econdCounter++;
    }



    const auto eRxheader = hgcal::econd::eRxSubPacketHeader(1, 1, false, cm0[erx], cm1[erx], enableMaps[erx]);  //eRx Header for each eRX
    econdPacket.push_back(eRxheader[0]);     //filling ERxHeaders 
    econdPacket.push_back(eRxheader[1]);
    //for(size_t j=0; j<eRxheader.size(); ++j)
    //{
     /*std::cout  
          << "ERx = " << erx
          << "  Header1: "
          << std::hex
          << eRxheader[0]
          << std::dec
          << "  cm0 = " << cm0[erx]
          << "  cm1 = " << cm1[erx]
          << std::endl;
      std::cout
          << "ERx = " << erx
          << "  Header2: "
          << std::hex
          << eRxheader[1]
          << std::dec
          << std::endl;    
      std::cout << std::endl;*/
   // }
    //cout << "ERx: " << erx << endl;
    const auto erx_chan_data = hgcal::econd::produceERxData(enableMaps[erx], allERxData[erx], true, true, true, false);  //eRx data for each eRX
    ///int i = 0;
    for(size_t ch = 0; ch < erx_chan_data.size(); ++ch){
      econdPacket.push_back(erx_chan_data[ch]);   //filling eRx payload(channel data)
        /*std::cout
          << "Ch = " << (ch)
          << "   Payload Word = "  
          << std::hex << erx_chan_data[ch]
          << std::dec
          << "   ADC = "  << allERxData[erx].adc[ch]
          << "   ADCm1 = " << allERxData[erx].adcm[ch]
          << "   TOA = "  << allERxData[erx].toa[ch]
          << "   TOT = "  << allERxData[erx].tot[ch]
          << "   TCTP = " << static_cast<unsigned int>(allERxData[erx].tctp[ch])
          << std::endl;*/
      
      //i++;
      
    }
    //std::cout << std::endl;
  }

  /*int counter = 0;
  for(size_t i = 0; i < econdPacket.size()/2; i++){
    if(i%236 == 0){
      cout <<  endl;
      cout << "Econd " << counter << endl;//"   " <<  i << "  " << i%234 << endl;
      counter++;
    }  
    cout << "idx: " << i << "  Word = " << std::hex << econdPacket[i] << std::dec << endl;
  }*/


    /////////ooooooooooOOOOOOOOOO ECOND header + idle words + trailer generation ooooooooooOOOOOOOOOO//////// (Work in progress!!!!)
    /*uint16_t header =  170;
    uint8_t ht = 0;
    uint8_t ebo = 0;
    uint8_t ehHam = 0;
    uint8_t rr = 0;
    for(int i = 0; i < necons; i++){
      const auto econdHeader = hgcal::econd::eventPacketHeader(header,
                                                                payloadLength[i],
                                                                false,
                                                                true,
                                                                ht,
                                                                ebo,
                                                                true,
                                                                false,
                                                                ehHam,
                                                                BX[i],
                                                                L1A[i],
                                                                Orbit[i],
                                                                false,
                                                                rr);

        cout << "ECOND" << i << std::setw(10) << "   Header Word0:  " << std::hex << econdHeader[0] << "  Header Word1:  " << econdHeader[1] << std::dec << 
        "   Payload Length: " << payloadLength[i]  << "   BX: " << BX[i] << "     L1A: " << L1A[i] << "     Orbit: " << static_cast<unsigned int>(Orbit[i]) << endl;
    }
    */

  //}

  


  /*std::vector<std::vector<uint32_t>> erxPayloads(72);

  for(int erx = 0; erx < 72; ++erx)
  {
      int start = erx * 37;

      erxPayloads[erx].assign(
          allDataWords.begin() + start,
          allDataWords.begin() + start + 37
      );
  }*/


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
