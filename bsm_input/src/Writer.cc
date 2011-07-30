// Input Writer
//
// Write messages into specified output file. The file format:
//
//  0-8         length of [event] section
//  [event]     events are stored one after another
//  [Input]     Input message is always the last one in the file
//
// Each event is written in a form:
//
//  Varint32    serialized event length
//  string      serialized event
//
// Input is saved in the form similar to event:
//
//  Varint32    serialized input length
//  string      serialized input
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <fstream>
#include <iostream>
#include <sstream>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"

#include "bsm_input/interface/Writer.h"

using namespace std;

namespace sys = boost::filesystem;

using bsm::Writer;

Writer::Writer(const string &output, const uint32_t &file_size):
    _max_file_name_attempts(5),
    _max_file_size(file_size),
    _path(output),
    _file_number(0)
{
    _delegate = 0;
}

Writer::~Writer()
{
    close();
}

void Writer::open()
{
    if (isOpen())
    {
        cerr << "attempt to re-open output file" << endl;

        return;
    }

    generateFilename();
    if (filename().empty())
    {
        cerr << "failed to generate output filename" << endl;

        return;
    }

    if (_delegate)
        _delegate->fileWillOpen(this);

    _std_out.reset(new fstream(filename().c_str(), ios::out | ios::trunc | ios::binary));
    if (!_std_out->is_open())
    {
        _std_out.reset();

        cerr << "failed to open output file: " << filename() << endl;

        return;
    }

    _raw_out.reset(new ::google::protobuf::io::OstreamOutputStream(_std_out.get()));
    _coded_out.reset(new CodedOutputStream(_raw_out.get()));
    _input.reset(new Input());

    // Pointer to the Input object
    //
    _coded_out->WriteLittleEndian64(0);

    if (_delegate)
        _delegate->fileDidOpen(this);
}

void Writer::close()
{
    if (!isOpen())
        return;

    if (_delegate)
        _delegate->fileWillClose(this);

    // Write Input at the end of the file
    //
    int bytes_written = _coded_out->ByteCount() - 8;

    string message;
    _input->SerializeToString(&message);

    write(message);

    // Update the pointer to the stored Input at the beginning of the file
    //
    _coded_out.reset();
    _raw_out.reset();

    _std_out->seekp(0);

    _raw_out.reset(new ::google::protobuf::io::OstreamOutputStream(_std_out.get()));
    _coded_out.reset(new CodedOutputStream(_raw_out.get()));

    _coded_out->WriteLittleEndian64(bytes_written);

    _coded_out.reset();
    _raw_out.reset();
    _std_out.reset();

    _input.reset();
    _filename.clear();

    if (_delegate)
        _delegate->fileDidClose(this);
}

bool Writer::isOpen() const
{
    return _std_out
        && _std_out->is_open();
}

const Writer::InputPtr Writer::input() const
{
    return _input;
}

bool Writer::write(const EventPtr &event)
{
    if (!isOpen())
        return false;

    if (_max_file_size < static_cast<uint32_t>(_coded_out->ByteCount() / 1000))
    {
        close();
        open();
    }

    string message;
    if (!event->SerializeToString(&message))
        return false;

    write(message);

    _input->set_events(_input->events() + 1);

    return true;
}

std::string Writer::filename() const
{
    return _filename;
}

void Writer::setDelegate(Delegate *delegate)
{
    _delegate = delegate;
}

Writer::Delegate *Writer::delegate() const
{
    return _delegate;
}



// Privates
//
void Writer::write(const string &message)
{
    _coded_out->WriteVarint32(message.size());
    _coded_out->WriteString(message);
}

// Attempt to generate "unique" output filename
//
void Writer::generateFilename()
{

#if BOOST_VERSION < 104600
    if (_path.empty())
        return;
#else
    if (_path.generic_string().empty())
        return;
#endif

    _filename.clear();
    for(uint32_t attempts = 0; _max_file_name_attempts > attempts; ++attempts)
    {
        ostringstream file_name;

        string dirname = _path.parent_path().string();

        if (dirname.size())
            file_name << dirname << "/";
        
        file_name << _path.stem() << "_"
            << ++_file_number << _path.extension();

        if (sys::exists(file_name.str().c_str()))
            continue;

        _filename = file_name.str();

        break;
    }
}
