// Convert CMSSW input (AOD, PAT) into BSM Input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
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
#include "bsm_input_maker/maker/interface/ElectronSelector.h"
#include "bsm_input_maker/maker/interface/MuonSelector.h"
#include "bsm_input_maker/maker/interface/Utility.h"

#include "bsm_input_maker/maker/interface/InputMaker.h"

using namespace std;
using namespace boost;
using namespace edm;
using namespace reco;

using bsm::InputMaker;

static void set_electronid(bsm::Electron *, bsm::Electron::ElectronIDName const,
        int const);

InputMaker::InputMaker(const ParameterSet &config):
    _input_type(Input::UNKNOWN)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    _event.reset(new Event());
    _hlt_config.reset(new HLTConfigProvider());

    _gen_particle_tag = config.getParameter<InputTag>("gen_particle");
    _gen_particle_depth_level =
        std::min(config.getParameter<uint32_t>("gen_particle_depth_level"),
            static_cast<uint32_t>(10));

    _jet_tag = config.getParameter<InputTag>("jet");
    _rho_tag = config.getParameter<InputTag>("rho");

    _primary_vertex_tag = config.getParameter<InputTag>("primary_vertex");
    _missing_energy_tag = config.getParameter<InputTag>("missing_energy");

    _electron_selector.reset(new ElectronSelector(
                config.getParameter<InputTag>("electron")));
    _muon_selector.reset(new MuonSelector(
                config.getParameter<InputTag>("muon"), _primary_vertex_tag));

    _trigger_results_tag = config.getParameter<InputTag>("hlt");
    _trigger_event_tag = config.getParameter<InputTag>("trigger_event");
    _hlt_path_pattern = regex(config.getParameter<string>("hlt_path_pattern"),
            regex_constants::icase | regex_constants::perl);

    _hlt_producer_pattern =
        regex(config.getParameter<string>("hlt_producer_pattern"),
            regex_constants::icase | regex_constants::perl);

    _hlt_filter_pattern = regex(config.getParameter<string>("hlt_filter_pattern"),
            regex_constants::icase | regex_constants::perl);

    setInputType(config.getParameter<string>("input_type"));

    _writer.reset(new Writer(config.getParameter<string>("output_filename")));
    _writer->setDelegate(this);
    _writer->open();
}

InputMaker::~InputMaker()
{
    _event.reset();
    _writer.reset();

    google::protobuf::ShutdownProtobufLibrary();
}

void InputMaker::fileDidOpen(const bsm::Writer *writer)
{
    using namespace posix_time;
    using namespace gregorian;

    if (writer != _writer.get())
        return;

    _writer->input()->set_type(_input_type);

    ptime now_utc = second_clock::universal_time();
    ptime epoch(date(1970, 1, 1));

    _writer->input()->set_create_date((now_utc - epoch).total_seconds());
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

    else if ("stop" == type)
        _input_type = Input::STOP;

    else if ("satop" == type)
        _input_type = Input::SATOP;

    else if ("zprime" == type)
        _input_type = Input::ZPRIME;

    else
        _input_type = Input::UNKNOWN;
}

void InputMaker::beginRun(const Run &run, const EventSetup &setup)
{
    initHLT(run, setup);
}

void InputMaker::analyze(const edm::Event &event,
                        const edm::EventSetup &setup)
{
    if (!_writer->isOpen())
        return;

    // Set event ID
    //
    _event->mutable_extra()->set_id(event.id().event());
    _event->mutable_extra()->set_run(event.id().run());
    _event->mutable_extra()->set_lumi(event.id().luminosityBlock());

    if (!triggers(event, setup))
        return;

    if (!_rho_tag.label().empty())
    {
        Handle<double> rho;
        event.getByLabel(_rho_tag, rho);

        if (rho.isValid())
            _event->mutable_extra()->set_rho(*rho);
        else
            LogWarning("InputMaker") << "failed to extract rho";
    }

    genParticle(event);

    jet(event);

    electron(event);

    muon(event);

    primaryVertex(event);
    met(event);

    _writer->write(_event);

    _event->Clear();
}

void InputMaker::initHLT(const edm::Run &run, const edm::EventSetup &setup)
{
    // Skip triggers if _trigger_results_tag is empty
    //
    if (_trigger_results_tag.label().empty())
        return;

    bool is_changed = true;
    if (!_hlt_config->init(run,
                setup,
                _trigger_results_tag.process(),
                is_changed))
    {
        LogWarning("InputMaker")
            << "failed to initialize HLT Config Provider";

        _hlts.clear();

        return;
    }

    // Do nothing if menu didn't change and hlt list is already available
    //
    if (!is_changed
            && !_hlts.empty())
        return;

    // HLT Config has changed prepare for reading a new Menu
    //
    _hlts.clear();

    typedef std::vector<std::string> Names;

    // Get list of trigger names
    //
    const Names &triggers = _hlt_config->triggerNames();
    hash<string> make_hash;

    // The pattern is used to extract the trigger version
    //
    regex trigger_name_pattern("^(HLT_\\w+?)(?:_[vV](\\d+))?$",
            regex_constants::icase | regex_constants::perl);

    // Process available triggers
    //
    uint32_t cmssw_id = 0;
    for(Names::const_iterator trigger = triggers.begin();
            triggers.end() != trigger;
            ++trigger, ++cmssw_id)
    {
        // Keep only triggers that match the user pattern
        //
        if (!regex_search(*trigger, _hlt_path_pattern))
            continue;

        // Separate the trigger version and name
        //
        smatch matches;
        if (!regex_match(*trigger, matches, trigger_name_pattern))
        {
            // Didn't understand the trigger name
            //
            LogWarning("InputMaker")
                << "failed to process trigger name: " << *trigger;

            continue;
        }

        // Construct the trigger object
        //
        Trigger obj;

        obj.full_name = *trigger;

        // Trigger names are saved in lower case
        //
        obj.name = matches[1];
        to_lower(obj.name);

        obj.hash = make_hash(obj.name);
        obj.version = matches[2].matched
            ? lexical_cast<uint32_t>(matches[2])
            : 1;

        _hlts[cmssw_id] = obj;
    }
}

bool InputMaker::triggers(const edm::Event &event,
        const edm::EventSetup &setup)
{
    if (_trigger_results_tag.label().empty()
            || _hlts.empty())
        return true;

    // Save trigger info for the events that pass BSM PAT path
    //
    if (!event.triggerResultsByName("PAT").accept("p0"))
        return false;

    // Extract Trigger Results and Event from the event
    //
    Handle<TriggerResults> trigger_results;
    event.getByLabel(_trigger_results_tag, trigger_results);

    if (!trigger_results.isValid())
    {
        LogWarning("InputMaker")
            << "failed to extract HLTs";

        return false;
    }

    Handle<trigger::TriggerEvent> trigger_event;
    event.getByLabel(_trigger_event_tag, trigger_event);

    if (!trigger_event.isValid())
    {
        LogWarning("InputMaker")
            << "failed to extract Trigger Event";

        return false;
    }

    // Map object keys: CMSSW Key <-> ProtoBuf Key
    // Not all producers, filters and trigger objects are saved
    //
    typedef map<uint32_t, uint32_t> MapID;

    MapID object_map;
    MapID producer_map;

    // Map String to ProtoBuf ID
    //
    typedef map<string, uint32_t> MapNameToId;

    MapNameToId filter_map;

    // String to Hash convertion function
    //
    hash<string> make_hash;

    bsm::Event::TriggerInfo *pb_trigger_info = _event->mutable_hlt();

    // Cache producers, filters, objects for fast access
    //
    typedef vector<string> Names;

    const Names &producers = trigger_event->collectionTags();
    const trigger::TriggerObjectCollection &objects =
        trigger_event->getObjects();

    // Trigger Object keys associated with producers
    //
    const trigger::Keys &keys = trigger_event->collectionKeys();

    // Save producers in the ProtoBuf Event
    //
    size_t producer_id = 0;
    for(Names::const_iterator producer = producers.begin();
            producers.end() != producer;
            ++producer, ++producer_id)
    {
        // Save only producers that match user regular expression
        //
        if (!regex_search(*producer, _hlt_producer_pattern))
            continue;

        // Extract corresponding trigger objects
        //
        const size_t from = producer_id
            ? keys[producer_id - 1]
            : producer_id;
        const size_t to = keys[producer_id];

        // Get associated pb ids
        //
        const uint32_t pb_from = pb_trigger_info->object().size();
        const uint32_t pb_to = pb_from + (to - from);
        for(size_t k = from; to > k; ++k)
        {
            // Store key in map
            //
            object_map[k] = pb_trigger_info->object().size();

            addTriggerObject(pb_trigger_info->add_object(), objects[k]);
        }

        // Store producer key in map
        //
        producer_map[producer_id] = pb_trigger_info->producer().size();

        // Add trigger object producer to the event
        //
        string producer_name(*producer);
        to_lower(producer_name);

        bsm::TriggerProducer *producer = pb_trigger_info->add_producer();
        const size_t hash = make_hash(producer_name);
        producer->set_hash(hash);
        producer->set_from(pb_from);
        producer->set_to(pb_to);

        // Add trigger object producer to the input
        //
        addHLTProducer(hash, producer_name);
    }

    // Save filters
    //
    for(size_t filter = 0, filters = trigger_event->sizeFilters();
            filters > filter;
            ++filter)
    {
        // Test if filter name matches user pattern
        //
        string filter_name = trigger_event->filterTag(filter).label();
        if (!regex_search(filter_name, _hlt_filter_pattern))
            continue;

        // Vector of associated ProtoBuf object keys that triggered filter
        //
        vector<uint32_t> pb_keys;

        // Process associated trigger objects
        //
        const trigger::Keys &filter_keys = trigger_event->filterKeys(filter);
        for(trigger::Keys::const_iterator key = filter_keys.begin();
                filter_keys.end() != key;
                ++key)
        {
            // Save associated trigger object if it was not added by 
            // any producer yet
            //
            uint32_t pb_key = pb_trigger_info->object().size();
            if (object_map.end() == object_map.find(*key))
            {
                // Store key in map
                //
                object_map[*key] = pb_key;

                addTriggerObject(pb_trigger_info->add_object(),
                        objects[*key]);
            }
            else
                pb_key = object_map[*key];

            pb_keys.push_back(pb_key);
        }

        // Store filter key in map
        //
        filter_map[filter_name] = pb_trigger_info->filter().size();

        // Add trigger object filter to the event
        //
        to_lower(filter_name);

        bsm::TriggerFilter *filter = pb_trigger_info->add_filter();
        const size_t hash = make_hash(filter_name);
        filter->set_hash(hash);

        for(vector<uint32_t>::const_iterator key = pb_keys.begin();
                pb_keys.end() != key;
                ++key)
        {
            filter->add_key(*key);
        }

        // Add trigger object filter to the input
        //
        addHLTFilter(hash, filter_name);
    }

    // Process only triggers that are loaded in the menu 
    //
    for(Triggers::const_iterator hlt = _hlts.begin();
            _hlts.end() != hlt;
            ++hlt)
    {
        bsm::Trigger *trigger = _event->mutable_hlt()->add_trigger();

        trigger->set_hash(hlt->second.hash);
        trigger->set_pass(trigger_results->accept(hlt->first));
        trigger->set_version(hlt->second.version);

        // Add associated trigger filters
        //
        const Names &modules = _hlt_config->moduleLabels(hlt->second.full_name);
        for(Names::const_iterator module = modules.begin();
                modules.end() != module;
                ++module)
        {
            // Search for module in the extracted filters list
            //
            MapNameToId::const_iterator filter = filter_map.find(*module);
            if (filter_map.end() == filter)
                continue;

            trigger->add_filter(filter->second);
        }

        // Add new path to the ProtoBuf input map
        //
        addHLTPath(hlt->second.hash, hlt->second.name);
    }

    return true;
}

bool InputMaker::isTriggerItemInCollection(const TriggerItems &collection,
        const std::size_t &hash)
{
    for(TriggerItems::const_iterator trigger = collection.begin();
            collection.end() != trigger;
            ++trigger)
    {
        if (hash == trigger->hash())
        {
            // Trigger is already stored
            //
            return true;
        }
    }

    return false;
}

void InputMaker::addHLTPath(const std::size_t &hash, const std::string &name)
{
    bsm::Input::Info::Trigger *triggers =
        _writer->input()->mutable_info()->mutable_trigger();

    if (isTriggerItemInCollection(triggers->path(), hash))
        return;

    bsm::TriggerItem *item = triggers->add_path();
    item->set_hash(hash);
    item->set_name(name);
}

void InputMaker::addHLTProducer(const std::size_t &hash, const std::string &name)
{
    bsm::Input::Info::Trigger *triggers =
        _writer->input()->mutable_info()->mutable_trigger();

    if (isTriggerItemInCollection(triggers->producer(), hash))
        return;

    bsm::TriggerItem *item = triggers->add_producer();
    item->set_hash(hash);
    item->set_name(name);
}

void InputMaker::addHLTFilter(const std::size_t &hash, const std::string &name)
{
    bsm::Input::Info::Trigger *triggers =
        _writer->input()->mutable_info()->mutable_trigger();

    if (isTriggerItemInCollection(triggers->filter(), hash))
        return;

    bsm::TriggerItem *item = triggers->add_filter();
    item->set_hash(hash);
    item->set_name(name);
}

void InputMaker::addBTags(Jet *pb, const pat::Jet *pat)
{
    Jet::BTag *btag = pb->add_btag();
    btag->set_type(Jet::BTag::TCHE);
    btag->set_discriminator(
            pat->bDiscriminator("trackCountingHighEffBJetTags"));

    btag = pb->add_btag();
    btag->set_type(Jet::BTag::TCHP);
    btag->set_discriminator(
            pat->bDiscriminator("trackCountingHighPurBJetTags"));

    btag = pb->add_btag();
    btag->set_type(Jet::BTag::SSVHE);
    btag->set_discriminator(
            pat->bDiscriminator("simpleSecondaryVertexHighEffBJetTags"));

    btag = pb->add_btag();
    btag->set_type(Jet::BTag::SSVHP);
    btag->set_discriminator(
            pat->bDiscriminator("simpleSecondaryVertexHighPurBJetTags"));
}

void InputMaker::genParticle(const edm::Event &event)
{
    if (_gen_particle_tag.label().empty())
        return;

    if (event.isRealData())
        return;

    Handle<GenParticleCollection> gen_particle;
    event.getByLabel(_gen_particle_tag, gen_particle);

    for(GenParticleCollection::const_iterator particle = gen_particle->begin();
            gen_particle->end() != particle;
            ++particle)
    {
        // Skip anything that is neither stable, nor Top
        //
        if (3 != particle->status()
                || TOP != abs(particle->pdgId()))
            continue;

        products(_event->add_gen_particle(),
                *particle,
                _gen_particle_depth_level);
    }
}

void InputMaker::products(bsm::GenParticle *pb_particle,
        const reco::Candidate &particle,
        const uint32_t &level)
{
    // Save current particle in ProtoBuf
    //
    pb_particle->set_id(particle.pdgId());
    pb_particle->set_status(particle.status());

    utility::set(pb_particle->mutable_physics_object()->mutable_p4(),
            &particle.p4());

    utility::set(pb_particle->mutable_physics_object()->mutable_vertex(),
            &particle.vertex());

    if (!level)
        return;

    // Save its stable children
    //
    for(reco::Candidate::const_iterator product = particle.begin();
            particle.end() != product;
            ++product)
    {
        if (3 != product->status())
            continue;

        products(pb_particle->add_child(), *product, level - 1);
    }
}

void InputMaker::jet(const edm::Event &event)
{
    if (_jet_tag.label().empty())
        return;

    using pat::JetCollection;

    Handle<JetCollection> jets;
    event.getByLabel(_jet_tag, jets);

    if (!jets.isValid())
    {
        LogWarning("InputMaker") << "failed to extract jet";

        return;
    }

    for(JetCollection::const_iterator jet = jets->begin();
            jets->end() != jet;
            ++jet)
    {
        bsm::Jet *pb_jet = _event->add_jet();

        // Save PAT corrected jet p4
        //
        utility::set(pb_jet->mutable_physics_object()->mutable_p4(),
                &jet->p4());
        utility::set(pb_jet->mutable_physics_object()->mutable_vertex(),
            &jet->vertex());

        // Extract uncorrected p4
        //
        utility::set(pb_jet->mutable_uncorrected_p4(),
                &jet->correctedP4(0));

        addBTags(pb_jet, &*jet);

        pb_jet->mutable_extra()->set_area(jet->jetArea());

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

void InputMaker::electron(const edm::Event &event)
{
    if (!_electron_selector->init(&event))
        return;

    typedef ElectronSelector::Electrons Electrons;

    const Electrons &electrons = _electron_selector->electron();
    for(Electrons::const_iterator electron = electrons.begin();
            electrons.end() != electron;
            ++electron)
    {
        bsm::Electron *pb_electron = _event->add_electron();

        fill(pb_electron, *electron);
    }
}

void InputMaker::muon(const edm::Event &event)
{
    if (!_muon_selector->init(&event))
        return;

    typedef MuonSelector::Muons Muons;

    const Muons &muons = _muon_selector->muon();
    for(Muons::const_iterator muon = muons.begin();
            muons.end() != muon;
            ++muon)
    {
        bsm::Muon *pb_muon = _event->add_muon();

        fill(pb_muon, *muon);
    }
}

void InputMaker::primaryVertex(const edm::Event &event)
{
    if (_primary_vertex_tag.label().empty())
        return;

    typedef vector<Vertex> PVCollection;

    Handle<PVCollection> primary_vertices;
    event.getByLabel(_primary_vertex_tag, primary_vertices);

    if (!primary_vertices.isValid())
    {
        LogWarning("InputMaker") << "failed to extract primary_vertices";

        return;
    }

    for(PVCollection::const_iterator vertex = primary_vertices->begin();
            primary_vertices->end() != vertex;
            ++vertex)
    {
        bsm::PrimaryVertex *pb_vertex = _event->add_primary_vertex();

        utility::set(pb_vertex->mutable_vertex(), &vertex->position());

        bsm::PrimaryVertex::Extra *extra = pb_vertex->mutable_extra();
        extra->set_ndof(vertex->ndof());
        extra->set_rho(vertex->position().Rho());
    }
}

void InputMaker::met(const edm::Event &event)
{
    if (_missing_energy_tag.label().empty())
        return;

    using pat::METCollection;

    Handle<METCollection> mets;
    event.getByLabel(_missing_energy_tag, mets);

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

void InputMaker::addTriggerObject(bsm::TriggerObject *to,
        const trigger::TriggerObject &from)
{
    // Fill ProtoBuf with values
    //
    to->set_particle_id(from.id());

    bsm::LorentzVector *p4 = to->mutable_p4();
    p4->set_e(from.energy());
    p4->set_px(from.px());
    p4->set_py(from.py());
    p4->set_pz(from.pz());
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
    extra->set_d0(electron->dB());
    extra->set_super_cluster_eta(electron->superCluster()->eta());
    extra->set_inner_track_expected_hits(electron->gsfTrack()->trackerExpectedHitsInner().numberOfHits());

    // Adding all the electron id info
    // 
    std::string postfix = "MC";
    
    // VeryLoose
    set_electronid(
       pb_electron,
       bsm::Electron::VeryLoose,
       electron->electronID(std::string("eidVeryLoose")+postfix)
    );
    // Loose 
    set_electronid(
       pb_electron,
       bsm::Electron::Loose,
       electron->electronID(std::string("eidLoose")+postfix)
    );
    // Loose 
    set_electronid(
       pb_electron,
       bsm::Electron::Medium,
       electron->electronID(std::string("eidMedium")+postfix)
    );
    // Tight 
    set_electronid(
       pb_electron,
       bsm::Electron::Tight,
       electron->electronID(std::string("eidTight")+postfix)
    );
    // SuperTight 
    set_electronid(
       pb_electron,
       bsm::Electron::SuperTight,
       electron->electronID(std::string("eidSuperTight")+postfix)
    );
    // HyperTight1 
    set_electronid(
       pb_electron,
       bsm::Electron::HyperTight1,
       electron->electronID(std::string("eidHyperTight1")+postfix)
    );
    // HyperTight2 
    set_electronid(
       pb_electron,
       bsm::Electron::HyperTight2,
       electron->electronID(std::string("eidHyperTight2")+postfix)
    );
    // HyperTight3
    set_electronid(
       pb_electron,
       bsm::Electron::HyperTight3,
       electron->electronID(std::string("eidHyperTight3")+postfix)
    );
    // HyperTight4
    set_electronid(
       pb_electron,
       bsm::Electron::HyperTight4,
       electron->electronID(std::string("eidHyperTight4")+postfix)
    );
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
    extra->set_d0(muon->dB());
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

// Auxiliary function to set the electron id
void set_electronid(bsm::Electron * electron, bsm::Electron::ElectronIDName const name, int const value)
{
	bsm::Electron::ElectronID * electronid = electron->add_id();
    electronid->set_name(name);
    electronid->set_identification((value & 1) == 1); 
    electronid->set_isolation((value & 2) == 2);
    electronid->set_conversion_rejection((value & 4) == 4);
    electronid->set_impact_parameter((value & 8) == 8);    
}

DEFINE_FWK_MODULE(InputMaker);
