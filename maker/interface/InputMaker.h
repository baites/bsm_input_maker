// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_INPUT_MAKER
#define BSM_INPUT_MAKER

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "bsm_input_maker/bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input_maker/bsm_input/interface/Input.pb.h"
#include "bsm_input_maker/bsm_input/interface/Writer.h"

class HLTConfigProvider;
class PFJetIDSelectionFunctor;

namespace pat
{
    class Electron;
    class Muon;
}

namespace reco
{
    class Candidate;
}

namespace bsm
{
    class ElectronSelector;
    class MuonSelector;

    class InputMaker: public edm::EDAnalyzer,
        public bsm::WriterDelegate
    {
        public:
            InputMaker(const edm::ParameterSet &);
            virtual ~InputMaker();

            // WriterDelegate interface
            //
            virtual void fileDidOpen(const bsm::Writer *);

        private:
            typedef ::google::protobuf::RepeatedPtrField<bsm::TriggerItem>
                TriggerItems;

            enum
            {
                TOP = 6
            };

            void setInputType(std::string);

            virtual void beginRun(const edm::Run &, const edm::EventSetup &);
            virtual void analyze(const edm::Event &, const edm::EventSetup &);

            void initHLT(const edm::Run &, const edm::EventSetup &);

            bool triggers(const edm::Event &, const edm::EventSetup &);

            bool isTriggerItemInCollection(const TriggerItems &collection,
                    const std::size_t &hash);

            void addHLTPath(const std::size_t &hash,
                    const std::string &name);

            void addHLTProducer(const std::size_t &hash,
                    const std::string &name);

            void addHLTFilter(const std::size_t &hash,
                    const std::string &name);

            void addTriggerObject(bsm::TriggerObject *,
                    const trigger::TriggerObject &);

            void addBTags(Jet *, const pat::Jet *);

            void genParticle(const edm::Event &);
            void products(bsm::GenParticle *,
                    const reco::Candidate &,
                    const uint32_t &level = 0);

            void jet(const edm::Event &);

            void electron(const edm::Event &);

            void muon(const edm::Event &);

            void primaryVertex(const edm::Event &);
            void met(const edm::Event &);

            void fill(bsm::Electron *, const pat::Electron *);
            void fill(bsm::Muon *, const pat::Muon *);

            edm::InputTag _gen_particle_tag;
            uint32_t _gen_particle_depth_level;

            edm::InputTag _jet_tag;
            edm::InputTag _rho_tag;

            edm::InputTag _electron_tag;
            edm::InputTag _muon_tag;

            edm::InputTag _primary_vertex_tag;
            edm::InputTag _missing_energy_tag;

            edm::InputTag _trigger_results_tag;
            edm::InputTag _trigger_event_tag;

            boost::regex _hlt_path_pattern;
            boost::regex _hlt_producer_pattern;
            boost::regex _hlt_filter_pattern;

            Input::Type _input_type;

            boost::shared_ptr<Writer> _writer;
            boost::shared_ptr<Event> _event;

            boost::shared_ptr<HLTConfigProvider> _hlt_config;

            struct TriggerItem
            {
                std::string full_name;
                std::string name;
                std::size_t hash;
            };

            struct Trigger: public TriggerItem
            {
                uint32_t version;
            };

            // CMSSW ID/key <-> Trigger object [Menu]
            //
            typedef std::map<uint32_t, Trigger> Triggers;

            Triggers _hlts;

            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;
    };
}

#endif
