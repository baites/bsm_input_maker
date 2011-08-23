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
    class InputMaker: public edm:: EDAnalyzer,
        public bsm::Writer::Delegate
    {
        public:
            InputMaker(const edm::ParameterSet &);
            virtual ~InputMaker();

            // Writer::Delegate interface
            //
            virtual void fileDidOpen(const bsm::Writer *);

        private:
            enum
            {
                TOP = 6
            };

            void setInputType(std::string);

            virtual void beginJob();
            virtual void beginRun(const edm::Run &, const edm::EventSetup &);
            virtual void analyze(const edm::Event &, const edm::EventSetup &);
            virtual void endJob();

            void genParticles(const edm::Event &);
            void products(bsm::GenParticle *,
                    const reco::Candidate &,
                    const uint32_t &level = 0);

            void jets(const edm::Event &);

            void pf_electrons(const edm::Event &);
            void gsf_electrons(const edm::Event &);

            void pf_muons(const edm::Event &);
            void reco_muons(const edm::Event &);

            void primaryVertices(const edm::Event &);
            void met(const edm::Event &);

            bool triggers(const edm::Event &, const edm::EventSetup &);

            void fill(bsm::Electron *, const pat::Electron *);
            void fill(bsm::Muon *, const pat::Muon *);

            void addHLTtoMap(const std::size_t &hash, const std::string &name);
            void addBTags(Jet *, const pat::Jet *);

            std::string _gen_particles_tag;
            uint32_t _gen_particle_levels;

            std::string _jets_tag;
            std::string _rho_tag;

            std::string _pf_electrons_tag;
            std::string _gsf_electrons_tag;

            std::string _pf_muons_tag;
            std::string _reco_muons_tag;

            std::string _primary_vertices_tag;
            std::string _missing_energies_tag;

            std::string _hlts_tag;

            typedef std::vector<std::string> Patterns;
            Patterns _hlt_patterns;

            Input::Type _input_type;

            boost::shared_ptr<Writer> _writer;
            boost::shared_ptr<Event> _event;

            boost::shared_ptr<HLTConfigProvider> _hlt_config;

            struct Trigger
            {
                std::string full_name;
                std::string name;
                std::size_t hash;
                uint32_t version;
            };

            // ID <-> Trigger [Menu]
            //
            typedef std::map<uint32_t, Trigger> Triggers;

            Triggers _hlts;
    };
}

#endif
