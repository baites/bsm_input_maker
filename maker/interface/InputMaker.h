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

class HLTConfigProvider;
class PFJetIDSelectionFunctor;

namespace pat
{
    class Electron;
    class Muon;
}

namespace bsm
{
    class Electron;
    class Event;
    class Muon;
    class Writer;

    class InputMaker: public edm:: EDAnalyzer
    {
        public:
            InputMaker(const edm::ParameterSet &);
            virtual ~InputMaker();

        private:
            virtual void beginJob();
            virtual void beginRun(const edm::Run &, const edm::EventSetup &);
            virtual void analyze(const edm::Event &, const edm::EventSetup &);
            virtual void endJob();

            void jets(const edm::Event &);

            void pf_electrons(const edm::Event &);
            void gsf_electrons(const edm::Event &);

            void pf_muons(const edm::Event &);
            void reco_muons(const edm::Event &);

            void primaryVertices(const edm::Event &);
            void met(const edm::Event &);

            void triggers(const edm::Event &, const edm::EventSetup &);

            void fill(bsm::Electron *, const pat::Electron *);
            void fill(bsm::Muon *, const pat::Muon *);

            std::string _jets_tag;

            std::string _pf_electrons_tag;
            std::string _gsf_electrons_tag;

            std::string _pf_muons_tag;
            std::string _reco_muons_tag;

            std::string _primary_vertices_tag;
            std::string _missing_energies_tag;

            std::string _hlts_tag;

            std::string _input_type;

            boost::shared_ptr<Writer> _writer;
            boost::shared_ptr<Event> _event;

            boost::shared_ptr<HLTConfigProvider> _hlt_config;

            struct Trigger
            {
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
