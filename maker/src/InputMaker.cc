// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "PhysicsTools/SelectorUtils/interface/SimpleCutBasedElectronIDSelectionFunctor.h"

#include "bsm_input_maker/bsm_input/interface/Event.pb.h"
#include "bsm_input_maker/bsm_input/interface/Input.pb.h"
#include "bsm_input_maker/bsm_input/interface/Isolation.pb.h"
#include "bsm_input_maker/bsm_input/interface/Track.pb.h"
#include "bsm_input_maker/bsm_input/interface/Trigger.pb.h"
#include "bsm_input_maker/maker/interface/Selector.h"
#include "bsm_input_maker/maker/interface/Utility.h"

#include "bsm_input_maker/maker/interface/InputMaker.h"

using std::string;
using std::vector;

using boost::lexical_cast;
using boost::regex;
using boost::regex_match;
using boost::regex_replace;
using boost::regex_search;
using boost::smatch;
using boost::to_lower;

using edm::Handle;
using edm::InputTag;
using edm::LogInfo;
using edm::LogWarning;
using edm::ParameterSet;
using edm::TriggerResults;

using reco::Vertex;
using reco::GenParticle;
using reco::GenParticleCollection;

using bsm::InputMaker;

InputMaker::InputMaker(const ParameterSet &config):
    _input_type(Input::UNKNOWN)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    _writer.reset(new Writer(config.getParameter<string>("fileName")));
    _writer->setDelegate(this);
    _writer->open();

    _event.reset(new Event());

    _gen_particles_tag = config.getParameter<string>("gen_particles");

    _jets_tag = config.getParameter<string>("jets");
    _rho_tag = config.getParameter<string>("rho");

    _pf_electrons_tag = config.getParameter<string>("pf_electrons");
    _gsf_electrons_tag = config.getParameter<string>("gsf_electrons");

    _pf_muons_tag = config.getParameter<string>("pf_muons");
    _reco_muons_tag = config.getParameter<string>("reco_muons");

    _primary_vertices_tag = config.getParameter<string>("primary_vertices");
    _missing_energies_tag = config.getParameter<string>("missing_energies");

    _hlts_tag = config.getParameter<string>("hlts");
    _hlt_pattern = config.getParameter<string>("hlt_pattern");
    to_lower(_hlt_pattern);

    if (!regex_search(_hlt_pattern, regex("^(?:\\w*\\*?)+$")))
    {
        // Didn't understand the trigger pattern
        //
        _hlt_pattern = "";
    }
    else
    {
        _hlt_pattern = regex_replace(_hlt_pattern, regex("\\*+"), "\\.\\*");
        if (".*" == _hlt_pattern)
            _hlt_pattern = "";
    }

    setInputType(config.getParameter<string>("input_type"));
}

InputMaker::~InputMaker()
{
    _event.reset();
    _writer.reset();

    google::protobuf::ShutdownProtobufLibrary();
}

void InputMaker::fileDidOpen(const bsm::Writer *writer)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    if (writer != _writer.get())
        return;

    _writer->input()->set_type(_input_type);

    ptime now_utc = second_clock::universal_time();
    ptime epoch(date(1970, 1, 1));

    _writer->input()->set_create_date((now_utc - epoch).total_seconds());

    for(Triggers::const_iterator hlt = _hlts.begin();
            _hlts.end() != hlt;
            ++hlt)
    {
        addHLTtoMap(hlt->second.hash, hlt->second.name);
    }
}



// Privates
//
void InputMaker::setInputType(string type)
{
    to_lower(type);

    if ("data" == type)
        _input_type = Input::DATA;

    else if ("ttbar" == type)
        _input_type = Input::TTBAR;

    else if ("qcd" == type)
        _input_type = Input::QCD;

    else if ("wjets" == type)
        _input_type = Input::WJETS;

    else if ("zjets" == type)
        _input_type = Input::ZJETS;

    else if ("vqq" == type)
        _input_type = Input::VQQ;

    else if ("single_top_t_channel" == type)
        _input_type = Input::SINGLE_TOP_T_CHANNEL;

    else if ("single_top_s_channel" == type)
        _input_type = Input::SINGLE_TOP_S_CHANNEL;

    else if ("single_top_tw_channel" == type)
        _input_type = Input::SINGLE_TOP_TW_CHANNEL;

    else if ("wc" == type)
        _input_type = Input::WC;

    else if ("zprime" == type)
        _input_type = Input::ZPRIME;
}

void InputMaker::beginJob()
{
}

void InputMaker::beginRun(const edm::Run &run,
        const edm::EventSetup &setup)
{
    // Skip triggers is _hlts_tag is empty
    //
    if (_hlts_tag.empty())
        return;

    bool is_changed = true;

    _hlt_config.reset(new HLTConfigProvider());
    if (!_hlt_config->init(run, setup, InputTag(_hlts_tag).process(),
                is_changed))
    {
        LogWarning("InputMaker")
            << "failed to initialize HLT Config Provider";

        _hlt_config.reset();
        _hlts.clear();

        return;
    }

    if (!is_changed
            && !_hlts.empty())
        return;

    // HLT Config has changed
    //
    _hlts.clear();

    typedef std::vector<std::string> CMSSW_Triggers;

    // Get list of trigger names
    //
    const CMSSW_Triggers &triggers = _hlt_config->triggerNames();
    uint32_t cmssw_id = 0;
    boost::hash<std::string> make_hash;

    regex user_pattern(_hlt_pattern, boost::regex_constants::icase | boost::regex_constants::perl);
    for(CMSSW_Triggers::const_iterator trigger = triggers.begin();
            triggers.end() != trigger;
            ++trigger, ++cmssw_id)
    {
        smatch matches;
        if (!regex_match(*trigger, matches, regex("^(HLT_\\w+?)(?:_[vV](\\d+))?$")))
        {
            // Didn't understand the trigger name
            //

            continue;
        }

        if (!_hlt_pattern.empty()
                && !regex_search(*trigger, user_pattern))
            continue;

        Trigger obj;

        obj.full_name = *trigger;
        obj.name = matches[1];
        to_lower(obj.name);
        obj.hash = make_hash(obj.name);
        obj.version = matches[2].matched
            ? lexical_cast<uint32_t>(matches[2])
            : 1;

        _hlts[cmssw_id] = obj;

        addHLTtoMap(obj.hash, obj.name);
    }
}

void InputMaker::analyze(const edm::Event &event,
                        const edm::EventSetup &setup)
{
    if (!_writer->isOpen())
        return;

    _event->mutable_extra()->set_id(event.id().event());
    _event->mutable_extra()->set_run(event.id().run());
    _event->mutable_extra()->set_lumi(event.id().luminosityBlock());

    if (triggers(event, setup))
    {
        if (!_rho_tag.empty())
        {
            Handle<double> rho;
            event.getByLabel(edm::InputTag(_rho_tag, "rho"), rho);
            _event->mutable_extra()->set_rho(*rho);
        }

        genParticles(event);

        jets(event);

        pf_electrons(event);
        gsf_electrons(event);

        pf_muons(event);
        reco_muons(event);

        primaryVertices(event);
        met(event);
    }

    _writer->write(_event);

    _event->Clear();
}

void InputMaker::endJob()
{
}

void InputMaker::genParticles(const edm::Event &event)
{
    if (_gen_particles_tag.empty())
        return;

    if (event.isRealData())
        return;

    Handle<GenParticleCollection> gen_particles;
    event.getByLabel(InputTag(_gen_particles_tag), gen_particles);

    for(GenParticleCollection::const_iterator particle = gen_particles->begin();
            gen_particles->end() != particle;
            ++particle)
    {
        if (3 != particle->status()
                || TOP != abs(particle->pdgId()))
            continue;

        products(_event->add_gen_particles(), *particle, 2);
    }
}

void InputMaker::products(bsm::GenParticle *pb_particle,
        const reco::Candidate &particle,
        const uint32_t &level)
{
    pb_particle->set_id(particle.pdgId());
    pb_particle->set_status(particle.status());

    utility::set(pb_particle->mutable_physics_object()->mutable_p4(),
            &particle.p4());

    utility::set(pb_particle->mutable_physics_object()->mutable_vertex(),
            &particle.vertex());

    if (!level)
        return;

    for(reco::Candidate::const_iterator product = particle.begin();
            particle.end() != product;
            ++product)
    {
        if (3 != product->status())
            continue;

        products(pb_particle->add_children(), *product, level - 1);
    }
}

void InputMaker::jets(const edm::Event &event)
{
    if (_jets_tag.empty())
        return;

    using pat::JetCollection;

    Handle<JetCollection> jets;
    event.getByLabel(InputTag(_jets_tag), jets);

    if (!jets.isValid())
    {
        LogWarning("InputMaker") << "failed to extract jets";

        return;
    }

    for(JetCollection::const_iterator jet = jets->begin();
            jets->end() != jet;
            ++jet)
    {
        bsm::Jet *pb_jet = _event->add_jets();

        utility::set(pb_jet->mutable_physics_object()->mutable_p4(),
                &jet->p4());
        utility::set(pb_jet->mutable_physics_object()->mutable_vertex(),
            &jet->vertex());

        utility::set(pb_jet->mutable_uncorrected_p4(),
                &jet->correctedP4(0));

        addBTags(pb_jet, &*jet);

        pb_jet->mutable_extra()->set_area(jet->jetArea());

        /*
        // Process children (constituents)
        //
        for(uint32_t i = 0, max = jet->numberOfDaughters(); max > i; ++i)
        {
            const reco::PFJet *child = dynamic_cast<const reco::PFJet *>(jet->daughter(i));
            if (!child)
                continue;

            bsm::Jet::Child *pb_child = pb_jet->add_children();

            utility::set(pb_child->mutable_physics_object()->mutable_p4(),
                    &child->p4());
            utility::set(pb_child->mutable_physics_object()->mutable_vertex(),
                    &child->vertex());
        }
        */

        typedef std::vector<reco::PFCandidatePtr> Constituents;
        
        const Constituents &constituents = jet->getPFConstituents();
        for(Constituents::const_iterator child = constituents.begin();
                constituents.end() != child;
                ++child)
        {
            bsm::Jet::Child *pb_child = pb_jet->add_children();

            utility::set(pb_child->mutable_physics_object()->mutable_p4(),
                    &(*child)->p4());
            utility::set(pb_child->mutable_physics_object()->mutable_vertex(),
                    &(*child)->vertex());
        }

        // Skip the rest if Generator Parton is not found for the jet
        //
        if (!jet->genParton())
            continue;

        const reco::GenParticle *parton = jet->genParton();
        bsm::GenParticle *pb_gen_particle = pb_jet->mutable_gen_parton();

        utility::set(pb_gen_particle->mutable_physics_object()->mutable_p4(),
                &parton->p4());

        utility::set(pb_gen_particle->mutable_physics_object()->mutable_vertex(),
                &parton->vertex());

        pb_gen_particle->set_id(parton->pdgId());
        pb_gen_particle->set_status(parton->status());
    }
}

void InputMaker::pf_electrons(const edm::Event &event)
{
    if (_pf_electrons_tag.empty())
        return;

    using pat::ElectronCollection;

    Handle<ElectronCollection> electrons;
    event.getByLabel(InputTag(_pf_electrons_tag), electrons);

    if (!electrons.isValid())
    {
        LogWarning("InputMaker") << "failed to extract PF electrons";

        return;
    }

    for(ElectronCollection::const_iterator electron = electrons->begin();
            electrons->end() != electron;
            ++electron)
    {
        /*
        SimpleCutBasedElectronIDSelectionFunctor electronID(SimpleCutBasedElectronIDSelectionFunctor::cIso70);

        if (!electronID(*electron))
            continue;
            */

        bsm::Electron *pb_electron = _event->add_pf_electrons();

        fill(pb_electron, &*electron);
    }
}

void InputMaker::gsf_electrons(const edm::Event &event)
{
    if (_gsf_electrons_tag.empty())
        return;

    using pat::ElectronCollection;

    Handle<ElectronCollection> electrons;
    event.getByLabel(InputTag(_gsf_electrons_tag), electrons);

    if (!electrons.isValid())
    {
        LogWarning("InputMaker") << "failed to extract GSF electrons";

        return;
    }

    for(ElectronCollection::const_iterator electron = electrons->begin();
            electrons->end() != electron;
            ++electron)
    {
        /*
        SimpleCutBasedElectronIDSelectionFunctor electronID(SimpleCutBasedElectronIDSelectionFunctor::cIso70);

        if (!electronID(*electron))
            continue;
            */

        bsm::Electron *pb_electron = _event->add_gsf_electrons();

        fill(pb_electron, &*electron);
    }
}

void InputMaker::pf_muons(const edm::Event &event)
{
    if (_pf_muons_tag.empty())
        return;

    using pat::MuonCollection;

    Handle<MuonCollection> muons;
    event.getByLabel(InputTag(_pf_muons_tag), muons);

    if (!muons.isValid())
    {
        LogWarning("InputMaker") << "failed to extract PF muons";

        return;
    }

    for(MuonCollection::const_iterator muon = muons->begin();
            muons->end() != muon;
            ++muon)
    {
        bsm::Muon *pb_muon = _event->add_pf_muons();

        fill(pb_muon, &*muon);
    }
}

void InputMaker::reco_muons(const edm::Event &event)
{
    if (_reco_muons_tag.empty())
        return;

    using pat::MuonCollection;

    Handle<MuonCollection> muons;
    event.getByLabel(InputTag(_reco_muons_tag), muons);

    if (!muons.isValid())
    {
        LogWarning("InputMaker") << "failed to extract RECO muons";

        return;
    }

    for(MuonCollection::const_iterator muon = muons->begin();
            muons->end() != muon;
            ++muon)
    {
        bsm::Muon *pb_muon = _event->add_reco_muons();

        fill(pb_muon, &*muon);
    }
}

void InputMaker::primaryVertices(const edm::Event &event)
{
    if (_primary_vertices_tag.empty())
        return;

    typedef vector<Vertex> PVCollection;

    Handle<PVCollection> primary_vertices;
    event.getByLabel(InputTag(_primary_vertices_tag), primary_vertices);

    if (!primary_vertices.isValid())
    {
        LogWarning("InputMaker") << "failed to extract primary_vertices";

        return;
    }

    for(PVCollection::const_iterator vertex = primary_vertices->begin();
            primary_vertices->end() != vertex;
            ++vertex)
    {
        bsm::PrimaryVertex *pb_vertex = _event->add_primary_vertices();

        utility::set(pb_vertex->mutable_vertex(), &vertex->position());

        bsm::PrimaryVertex::Extra *extra = pb_vertex->mutable_extra();
        extra->set_ndof(vertex->ndof());
        extra->set_rho(vertex->position().Rho());
    }
}

void InputMaker::met(const edm::Event &event)
{
    if (_missing_energies_tag.empty())
        return;

    using pat::METCollection;

    Handle<METCollection> mets;
    event.getByLabel(InputTag(_missing_energies_tag), mets);

    if (!mets.isValid())
    {
        LogWarning("InputMaker") << "failed to extract mets";

        return;
    }

    if (!mets->size())
    {
        LogWarning("InputMaker") << "empty mets";

        return;
    }

    bsm::MissingEnergy *pb_met = _event->mutable_missing_energy();

    utility::set(pb_met->mutable_p4(),
            &mets->begin()->p4());
}

bool InputMaker::triggers(const edm::Event &event,
        const edm::EventSetup &setup)
{
    if (_hlts_tag.empty()
            || !_hlt_config
            || _hlts.empty())
        return true;

    Handle<TriggerResults> hlts;
    event.getByLabel(InputTag(_hlts_tag), hlts);

    if (!hlts.isValid())
    {
        LogWarning("InputMaker")
            << "failed to extract HLTs";

        return false;
    }

    for(Triggers::const_iterator hlt = _hlts.begin();
            _hlts.end() != hlt;
            ++hlt)
    {
        bsm::Trigger *pb_hlt = _event->add_hlts();

        pb_hlt->set_hash(hlt->second.hash);
        pb_hlt->set_pass(false);
        pb_hlt->set_pass(hlts->accept(hlt->first));
        pb_hlt->set_version(hlt->second.version);
        pb_hlt->set_prescale(1);

        //pb_hlt->set_prescale(_hlt_config->prescaleValue(event, setup, hlt->second.full_name));
        //
    }

    edm::TriggerResultsByName resultsByNameHLT = event.triggerResultsByName("PAT");
    const bool scraping_veto = resultsByNameHLT.accept("filter_scraping");
    const bool hbhe_noise = resultsByNameHLT.accept("filter_hbhenoise");
    const bool pat_sequence = resultsByNameHLT.accept("p0");
    _event->mutable_filters()->set_scraping_veto(scraping_veto);
    _event->mutable_filters()->set_hbhe_noise(hbhe_noise);

    return pat_sequence;
}

void InputMaker::fill(bsm::Electron *pb_electron, const pat::Electron *electron)
{
    utility::set(pb_electron->mutable_physics_object()->mutable_p4(),
            &electron->p4());
    utility::set(pb_electron->mutable_physics_object()->mutable_vertex(),
            &electron->vertex());

    bsm::Isolation *pb_isolation = pb_electron->mutable_isolation();
    pb_isolation->set_track(electron->dr03TkSumPt());
    pb_isolation->set_ecal(electron->dr03EcalRecHitSumEt());
    pb_isolation->set_hcal(electron->dr03HcalTowerSumEt());

    bsm::PFIsolation *pb_pf_isolation = pb_electron->mutable_pf_isolation();
    pb_pf_isolation->set_particle(electron->particleIso());
    pb_pf_isolation->set_charged_hadron(electron->chargedHadronIso());
    pb_pf_isolation->set_neutral_hadron(electron->neutralHadronIso());
    pb_pf_isolation->set_photon(electron->photonIso());

    bsm::Electron::Extra *extra = pb_electron->mutable_extra();
    extra->set_d0_bsp(electron->dB());
    extra->set_super_cluster_eta(electron->superCluster()->eta());
    extra->set_inner_track_expected_hits(electron->gsfTrack()->trackerExpectedHitsInner().numberOfHits());
}

void InputMaker::fill(bsm::Muon *pb_muon, const pat::Muon *muon)
{
    utility::set(pb_muon->mutable_physics_object()->mutable_p4(),
            &muon->p4());
    utility::set(pb_muon->mutable_physics_object()->mutable_vertex(),
            &muon->vertex());

    bsm::Isolation *pb_isolation = pb_muon->mutable_isolation();
    pb_isolation->set_track(muon->trackIso());
    pb_isolation->set_ecal(muon->ecalIso());
    pb_isolation->set_hcal(muon->hcalIso());

    bsm::PFIsolation *pb_pf_isolation = pb_muon->mutable_pf_isolation();
    pb_pf_isolation->set_particle(muon->particleIso());
    pb_pf_isolation->set_charged_hadron(muon->chargedHadronIso());
    pb_pf_isolation->set_neutral_hadron(muon->neutralHadronIso());
    pb_pf_isolation->set_photon(muon->photonIso());

    bsm::Muon::Extra *extra = pb_muon->mutable_extra();
    extra->set_is_global(muon->isGlobalMuon());
    extra->set_is_tracker(muon->isTrackerMuon());
    extra->set_d0_bsp(muon->dB());
    extra->set_number_of_matches(muon->numberOfMatches());

    if (muon->isTrackerMuon())
    {
        bsm::Track *track = pb_muon->mutable_inner_track();
        track->set_hits(muon->innerTrack()->numberOfValidHits());
        track->set_normalized_chi2(muon->innerTrack()->normalizedChi2());

        extra->set_pixel_hits(muon->innerTrack()->hitPattern().pixelLayersWithMeasurement());
    }

    if (muon->isGlobalMuon())
    {
        bsm::Track *track = pb_muon->mutable_global_track();
        track->set_hits(muon->globalTrack()->hitPattern().numberOfValidMuonHits());
        track->set_normalized_chi2(muon->globalTrack()->normalizedChi2());
    }
}

void InputMaker::addHLTtoMap(const std::size_t &hash, const std::string &name)
{
    typedef ::google::protobuf::RepeatedPtrField<bsm::TriggerItem>
        TriggerItems;

    bsm::Input::Info *info = _writer->input()->mutable_info();
    for(TriggerItems::const_iterator trigger = info->triggers().begin();
            info->triggers().end() != trigger;
            ++trigger)
    {
        if (hash == trigger->hash())
        {
            // Trigger is already stored
            //
            return;
        }
    }

    bsm::TriggerItem *item = info->add_triggers();
    item->set_hash(hash);
    item->set_name(name);
}

void InputMaker::addBTags(Jet *pb, const pat::Jet *pat)
{
    Jet::BTag *btag = pb->add_btags();
    btag->set_type(Jet::BTag::TCHE);
    btag->set_discriminator(
            pat->bDiscriminator("trackCountingHighEffBJetTags"));

    btag = pb->add_btags();
    btag->set_type(Jet::BTag::TCHP);
    btag->set_discriminator(
            pat->bDiscriminator("trackCountingHighPurBJetTags"));

    btag = pb->add_btags();
    btag->set_type(Jet::BTag::SSVHE);
    btag->set_discriminator(
            pat->bDiscriminator("simpleSecondaryVertexHighEffBJetTags"));

    btag = pb->add_btags();
    btag->set_type(Jet::BTag::SSVHP);
    btag->set_discriminator(
            pat->bDiscriminator("simpleSecondaryVertexHighPurBJetTags"));
}

DEFINE_FWK_MODULE(InputMaker);
