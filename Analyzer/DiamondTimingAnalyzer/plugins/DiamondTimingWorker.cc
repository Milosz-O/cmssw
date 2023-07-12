// -*- C++ -*-
//
// Package:    Analyzer/DiamondTimingWorker
// Class:      DiamondTimingWorker
//
/**\class DiamondTimingWorker DiamondTimingWorker.cc Analyzer/DiamondTimingWorker/plugins/DiamondTimingWorker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Arkadiusz Wolk
//         Created:  Mon, 19 Jul 2021 13:22:33 GMT
//
//

#include <string>
#include <array>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/VeryForwardGeometryBuilder/interface/CTPPSGeometry.h"
#include "Geometry/Records/interface/VeryForwardRealGeometryRecord.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDiamondDetId.h"

#include "DataFormats/CTPPSDigi/interface/CTPPSDiamondDigi.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDiamondDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"

#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

#include "CondFormats/DataRecord/interface/PPSTimingCalibrationRcd.h"

#include "Analyzer/DiamondTimingAnalyzer/interface/DiamondDetectorClass.h"
#include "Analyzer/DiamondTimingAnalyzer/interface/JSON.h"

//
// class declaration
//
class DiamondTimingWorker : public DQMEDAnalyzer {
public:
    explicit DiamondTimingWorker(const edm::ParameterSet&);
    ~DiamondTimingWorker() = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void bookHistograms(DQMStore::IBooker& iBooker, edm::Run const&, edm::EventSetup const& iSetup) override;
    void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;

    // ---------- objects to retrieve ---------------------------
    edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondDigi>> tokenDigi_;
    edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondRecHit>> tokenRecHit_;
    edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondLocalTrack>> tokenLocalTrack_;
    edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>> tokenPixelLocalTrack_;
    edm::ESGetToken<CTPPSGeometry, VeryForwardRealGeometryRecord> geomEsToken_;
    edm::ESGetToken<PPSTimingCalibration, PPSTimingCalibrationRcd> calibEsToken_;

    // ------------ member data ------------
    struct Histograms_DiamondTiming {
        std::map<ChannelKey, MonitorElement*> t;
        std::map<ChannelKey, MonitorElement*> valid_t;
        std::map<ChannelKey, MonitorElement*> tot;
        std::map<ChannelKey, MonitorElement*> valid_tot;
        std::map<ChannelKey, MonitorElement*> t_vs_tot;

        std::map<ChannelKey, MonitorElement*> l2_res;
        std::map<ChannelKey, MonitorElement*> trk_time;
        std::map<ChannelKey, MonitorElement*> expected_trk_time_res;

        //pair == (sector, station)
        std::map<std::pair<int, int>, MonitorElement*> trk_time_SPC;
        std::map<std::pair<int, int>, MonitorElement*> trk_res;
        std::map<std::pair<int, int>, MonitorElement*> trk_time_SPC_vs_BX;
        std::map<std::pair<int, int>, MonitorElement*> trk_time_SPC_vs_LS;
    };
    Histograms_DiamondTiming histos;

    edm::ESHandle<CTPPSGeometry> geom;
    
    int validOOT;
    std::map<std::pair<int, int>, std::pair<int, int>> Ntracks_cuts_map_;  //arm, station ,, Lcut,Ucut
    std::map<ChannelKey, int> planes_config;
};

//
// constants, enums and typedefs
//

enum STATION { _210_M_ID, _TIMING_ID, _220_M_ID };

const int PLANES_X_DETECTOR = 4;

//
// constructors and destructor
//
DiamondTimingWorker::DiamondTimingWorker(const edm::ParameterSet& iConfig)
    : 
    tokenDigi_(
        consumes<edm::DetSetVector<CTPPSDiamondDigi>>(iConfig.getParameter<edm::InputTag>("tagDigi"))),
    tokenRecHit_(
        consumes<edm::DetSetVector<CTPPSDiamondRecHit>>(iConfig.getParameter<edm::InputTag>("tagRecHit"))),
    tokenLocalTrack_(
        consumes<edm::DetSetVector<CTPPSDiamondLocalTrack>>(iConfig.getParameter<edm::InputTag>("tagLocalTrack"))),
    tokenPixelLocalTrack_(
        consumes<edm::DetSetVector<CTPPSPixelLocalTrack>>(iConfig.getParameter<edm::InputTag>("tagPixelLocalTrack"))),
    geomEsToken_(esConsumes<CTPPSGeometry, VeryForwardRealGeometryRecord, edm::Transition::BeginRun>()),
    calibEsToken_(esConsumes<PPSTimingCalibration, PPSTimingCalibrationRcd>()),
    validOOT(iConfig.getParameter<int>("tagValidOOT")) {
    
    Ntracks_cuts_map_[std::make_pair(SECTOR::_45_ID, STATION::_210_M_ID)] =
        std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[0],
                       iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[0]);

    Ntracks_cuts_map_[std::make_pair(SECTOR::_45_ID, STATION::_220_M_ID)] =
        std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[1],
                       iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[1]);

    Ntracks_cuts_map_[std::make_pair(SECTOR::_56_ID, STATION::_210_M_ID)] =
        std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[2],
                       iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[2]);

    Ntracks_cuts_map_[std::make_pair(SECTOR::_56_ID, STATION::_220_M_ID)] =
        std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[3],
                       iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[3]);

    //read planes config
    planes_config = JSON::read_planes_config(iConfig.getParameter<std::string>("planesConfig"));
}

//
// member functions
//

// ------------ method called for each event  ------------

void DiamondTimingWorker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    using namespace edm;
    edm::LogWarning("MyInfoLog") << "Entering analyze method";
    //retrieve data
    edm::Handle<edm::DetSetVector<CTPPSDiamondRecHit>> recHits; // TODO
    edm::Handle<edm::DetSetVector<CTPPSDiamondLocalTrack>> localTracks;
    edm::Handle<edm::DetSetVector<CTPPSPixelLocalTrack>> pixelLocalTracks;
    edm::ESHandle<PPSTimingCalibration> calib;

    iEvent.getByToken(tokenRecHit_, recHits);
    iEvent.getByToken(tokenLocalTrack_, localTracks);
    iEvent.getByToken(tokenPixelLocalTrack_, pixelLocalTracks);
    calib = iSetup.getHandle(calibEsToken_);
    edm::LogWarning("MyInfoLog") << "analyze method read tokens complete";

    DiamondDetectorClass DiamondDet(validOOT, *geom, *recHits, *localTracks, DiamondTimingCalibration(*calib));

    ////////////////////////////////////////////////////////////////
    //
    //		EXTRACT PIXELS TRACK NUMBER
    //      Will be used for sector independent event selection
    //
    /////////////////////////////////////////////////////////////////
    std::map<std::pair<int, int>, int> Pixel_Mux_map_;  //arm, station
    Pixel_Mux_map_.clear();
    std::vector<bool> Sector_TBA(2, true);

    for (const auto& RP_trks : *pixelLocalTracks) {  //array of tracks
        const CTPPSDetId detId(RP_trks.detId());
        //std::cout << "Tracks in arm " << detId.arm() << ", station " << detId.station() << ", rp " << detId.rp() << std::endl;

        for (const auto& trk : RP_trks) {
            if (!trk.isValid())
                continue;
            Pixel_Mux_map_[std::make_pair(detId.arm(), detId.station())]++;
        }
    }
    edm::LogWarning("MyInfoLog") << "analyze method: Sector_TBA filled complete";

    for (const auto& Ntracks_cuts_iter_ : Ntracks_cuts_map_) {
        if ((Ntracks_cuts_iter_.second.first < 0) || (Ntracks_cuts_iter_.second.second < 0))
            continue;  // don't care condition
        if ((Pixel_Mux_map_[Ntracks_cuts_iter_.first] < Ntracks_cuts_iter_.second.first) ||
            (Pixel_Mux_map_[Ntracks_cuts_iter_.first] > Ntracks_cuts_iter_.second.second))  //condition violated
            Sector_TBA[Ntracks_cuts_iter_.first.first] = false;
    }
    edm::LogWarning("MyInfoLog") << "analyze method: Pixel MAP complete";


    if (!(Sector_TBA[0] || Sector_TBA[1]))
        return;

    ////////////////////////////////////////////////////////////////
    //
    //		control over PCL calibration quality
    //
    /////////////////////////////////////////////////////////////////
    edm::LogWarning("MyInfoLog") << "Analyze method: begin of control over PCL calibration quality";

    for (const auto& _recHits : *recHits) {  //rechits = array of hits in one channel
        const CTPPSDiamondDetId detid(_recHits.detId());
        const ChannelKey key(detid);
        if (!(Sector_TBA[detid.arm()]))
            continue;

        // Perform channel histogram
        for (const auto& recHit : _recHits) {  //rechit
            if (((recHit.ootIndex() != 0) && validOOT != -1) || recHit.multipleHits())
                continue;

            //T,TOT and OOT for all hits, important for monitoring the calibration
            histos.t[key]->Fill(recHit.time());
            histos.tot[key]->Fill(recHit.toT());

            // T,TOT and OOT complete hits (T and TOT available), important for monitoring the calibration
            if (DiamondDet.PadActive(key)) {
                histos.valid_tot[key]->Fill(DiamondDet.GetToT(key));
                histos.t_vs_tot[key]->Fill(DiamondDet.GetToT(key), DiamondDet.GetTime(key));
                histos.valid_t[key]->Fill(DiamondDet.GetTime(key));
            }
        }
    }

    ////////////////////////////////////////////////////////////////
    //
    //		RESOLUTION STUDIES
    //
    /////////////////////////////////////////////////////////////////
    edm::LogWarning("MyInfoLog") << "Analyze method: begin of resolution studies";

    //which planes are active
    std::array<bool, 4> active_plane{ {false, false, false, false} };

    for (const auto& LocalTrack_mapIter : DiamondDet.GetDiamondTrack_map()) {  // loop on predigested tracks
        int sector = LocalTrack_mapIter.first.z0() > 0.0 ? SECTOR::_45_ID : SECTOR::_56_ID;

        if (!(Sector_TBA[sector]))
            continue;

        if (LocalTrack_mapIter.second.size() == 0)
            continue;

        //station id
        int station = LocalTrack_mapIter.second.at(0).first.planeKey.station;
        auto stationKey = std::pair<int, int>{sector, station};

        //which planes are active
        active_plane[0] = DiamondDet.GetMuxInTrack(PlaneKey(sector, station, 0)) == 1;
        active_plane[1] = DiamondDet.GetMuxInTrack(PlaneKey(sector, station, 1)) == 1;
        active_plane[2] = DiamondDet.GetMuxInTrack(PlaneKey(sector, station, 2)) == 1;
        active_plane[3] = DiamondDet.GetMuxInTrack(PlaneKey(sector, station, 3)) == 1;
        //number of active planes
        int active_num = std::count_if(active_plane.begin(), active_plane.end(), [](bool it) -> bool{return it;});

        //we don't check active planes here, because each channel might require different number of them
        bool mark_tag = DiamondDet.GetTrackMuxInSector(sector) == 1;

        std::vector<ChannelKey> hit_selected(PLANES_X_DETECTOR);
        std::vector<std::pair<ChannelKey, CTPPSDiamondRecHit>>::const_iterator hit_iter;

        double Track_time_SPC = 100.0;
        double Track_precision_SPC = 100.0;

        // hits in track loop LocalTrack_mapIter.second = std::vector<std::pair<ChannelKey,CTPPSDiamondRecHit>>
        edm::LogWarning("MyInfoLog") << "Analyze method: before for L274";
        for (hit_iter = LocalTrack_mapIter.second.begin(); hit_iter < LocalTrack_mapIter.second.end(); hit_iter++) {
            auto& key = (*hit_iter).first;

            if(!active_plane[key.planeKey.plane])
                continue;

            double hit_time_SPC = DiamondDet.GetTime(key);
            // double hit_prec_SPC = DiamondDet.GetPadPrecision(key); //TODO: Unused variable
            double hit_weig_SPC = DiamondDet.GetPadWeight(key);

            if (mark_tag)
                hit_selected[key.planeKey.plane] = key;  // save for resolution reco

            Track_time_SPC = (Track_time_SPC * pow(Track_precision_SPC, -2) + hit_time_SPC * hit_weig_SPC) /
                             (pow(Track_precision_SPC, -2) + hit_weig_SPC);
            Track_precision_SPC = pow((pow(Track_precision_SPC, -2) + hit_weig_SPC), -0.5);
        }

        histos.trk_time_SPC[stationKey]->Fill(Track_time_SPC);
        histos.trk_res[stationKey]->Fill(Track_precision_SPC);

        histos.trk_time_SPC_vs_BX[stationKey]->Fill(iEvent.bunchCrossing(), Track_time_SPC);
        histos.trk_time_SPC_vs_LS[stationKey]->Fill(iEvent.luminosityBlock(), Track_time_SPC);

        if (mark_tag) {
            for (int pl_mark = 0; pl_mark < PLANES_X_DETECTOR; pl_mark++) {
                if(!active_plane[pl_mark]) continue;

                double Marked_track_time = 12.5;
                double Marked_track_precision = 25.0;

                edm::LogWarning("MyLogInfoGetTimeDebug") << "Analyze methodL get item form hit_selected " << hit_selected[pl_mark];
                for(auto& item_map: DiamondDet.RecHit_map_){
                    edm::LogWarning("MyLogInfoGetTimeDebug") << "Analyze method: RecHit_map " << item_map.first << " ", item_map.second;
                }


                double Marked_hit_time = DiamondDet.GetTime(hit_selected[pl_mark]);; //TODO: error is here
                int Marked_hit_channel = hit_selected[pl_mark].channel;

                ChannelKey key(sector, station, pl_mark, Marked_hit_channel);

                //we continue calculations for this channel only if the number of active planes is the same as in the configuration
                if(active_num != planes_config[key]) continue;

                for (int pl_loop = 0; pl_loop < PLANES_X_DETECTOR; pl_loop++) {
                    if(!active_plane[pl_loop]) continue;
                    if (pl_loop == pl_mark) continue;

                    double Others_hit_time = DiamondDet.GetTime(hit_selected[pl_loop]);
                    double Others_hit_weig = DiamondDet.GetPadWeight(hit_selected[pl_loop]);

                    Marked_track_time =
                        (Marked_track_time * pow(Marked_track_precision, -2) + Others_hit_time * Others_hit_weig) /
                        (pow(Marked_track_precision, -2) + Others_hit_weig);
                    Marked_track_precision = pow((pow(Marked_track_precision, -2) + Others_hit_weig), -0.5);
                }

                //std::cout<<"T = "<<Marked_hit_time<<"; Trk = "<<Marked_track_time<<std::endl;
                double Marked_hit_difference = Marked_hit_time - Marked_track_time;

                histos.l2_res[key]->Fill(Marked_hit_difference);
                histos.trk_time[key]->Fill(Marked_track_time);
                histos.expected_trk_time_res[key]->Fill(Marked_track_precision);
            }
        }
    }
    edm::LogWarning("MyInfoLog") << "Analyze method complete";

}

void DiamondTimingWorker::bookHistograms(DQMStore::IBooker& iBooker,
                                         edm::Run const& run,
                                         edm::EventSetup const& iSetup) {
    edm::LogWarning("MyInfoLog") << "Entering bookHistograms method";
    std::string ch_path;
    geom = iSetup.getHandle(geomEsToken_);
    for (auto it = (*geom).beginSensor(); it != (*geom).endSensor(); ++it) {
        if (!CTPPSDiamondDetId::check(it->first))
            continue;

        const CTPPSDiamondDetId detid(it->first);
        ChannelKey key(detid);

        if (histos.trk_time_SPC.count({detid.arm(), detid.station()}) == 0) {
            std::string station_path;
            edm::LogWarning("MyInfoLog") << "Before stationName method";

            detid.stationName(station_path, CTPPSDiamondDetId::nPath);

            edm::LogWarning("MyInfoLog") << "After stationName method";

            iBooker.setCurrentFolder(station_path);

            auto stationKey = std::make_pair<int, int>(detid.arm(), detid.station());

            std::string name = "Timing track time SPC sector " + std::to_string(detid.arm());
            histos.trk_time_SPC[stationKey] = iBooker.book1D(name.c_str(), name.c_str(), 1200, -60, 60);

            name = "Timing track resolution sector " + std::to_string(detid.arm());
            histos.trk_res[stationKey] = iBooker.book1D(name.c_str(), name.c_str(), 1000, 0, 1);

            name = "Timing track time SPC Vs BX sector " + std::to_string(detid.arm());
            histos.trk_time_SPC_vs_BX[stationKey] = iBooker.book2D(name.c_str(), name.c_str(), 4000, 0, 4000, 500, -5, 5);

            name = "Timing track time SPC Vs LS sector " + std::to_string(detid.arm());
            histos.trk_time_SPC_vs_LS[stationKey] = iBooker.book2D(name.c_str(), name.c_str(), 4000, 0, 4000, 500, -5, 5);
        }

        auto ch_name = key.to_string();

        detid.channelName(ch_path, CTPPSDiamondDetId::nPath);
        iBooker.setCurrentFolder(ch_path);

        std::string name;

        name = "Time distribution " + ch_name;
        histos.t[key] = iBooker.book1D(name.c_str(), (name + ";t (ns);Entries").c_str(), 1200, -60., 60.);

        name = "Valid Time distribution " + ch_name;
        histos.valid_t[key] = iBooker.book1D(name.c_str(), (name + ";t (ns);Entries").c_str(), 1200, -60., 60.);

        name = "TOT distribution " + ch_name;
        histos.tot[key] = iBooker.book1D(name.c_str(), (name + ";TOT (ns);Entries").c_str(), 100, -20., 20.);

        name = "Valid TOT distribution " + ch_name;
        histos.valid_tot[key] = iBooker.book1D(name.c_str(), (name + ";TOT (ns);Entries").c_str(), 100, -20., 20.);

        name = "Time vs TOT distribution " + ch_name;
        histos.t_vs_tot[key] = iBooker.book2D(name.c_str(), (name + ";TOT (ns);t (ns)").c_str(), 240, 0., 60., 450, -20., 25.);

        name = "l2_res_" + ch_name;
        histos.l2_res[key] = iBooker.book1D(name.c_str(), ("Difference between track time and hit time " + ch_name + ";t (ns);Entries").c_str(), 1200, -60, 60);

        name = "Track time distribution " + ch_name;
        histos.trk_time[key] = iBooker.book1D(name.c_str(), (name + ";t (ns);Entries").c_str(), 1200, -60, 60);

        name = "Expected track time resolution distribution " + ch_name;
        histos.expected_trk_time_res[key] =
            iBooker.book1D(name.c_str(), (name + ";t (ns);Entries").c_str(), 1000, 0, 2);
    }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void DiamondTimingWorker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    // The following says we do not know what parameters are allowed so do no
    // validation
    // Please change this to state exactly what you do use, even if it is no
    // parameters
    edm::ParameterSetDescription desc;
    desc.setAllowAnything();
    // desc.add<std::string>("folder", "MY_FOLDER");
    // descriptions.add("DiamondTimingWorker", desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(DiamondTimingWorker);
