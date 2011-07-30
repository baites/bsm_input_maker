// Input Reader
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <math.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"

#include "bsm_input/interface/Reader.h"

using namespace std;

namespace sys = boost::filesystem;

using bsm::Reader;

Reader::Reader(const string &input):
    _filename(input),
    _is_good(true),
    _current_event(0)
{
    _delegate = 0;
}

Reader::~Reader()
{
    close();
}

void Reader::open()
{
    if (isOpen())
    {
        cerr << "attempt to re-open input file" << endl;

        return;
    }

    if (filename().empty())
    {
        cerr << "failed to open input: filename is empty" << endl;

        return;
    }

    if (!sys::exists(filename().c_str()))
    {
        cerr << "input file does not exist: " << filename() << endl;

        return;
    }

    if (_delegate)
        _delegate->fileWillOpen(this);

    _std_in.reset(new fstream(filename().c_str(), ios::in | ios::binary));
    if (!_std_in->is_open())
    {
        _std_in.reset();

        cerr << "failed to open input file: " << filename() << endl;

        return;
    }

    _raw_in.reset(new ::google::protobuf::io::IstreamInputStream(_std_in.get()));
    _coded_in.reset(new CodedInputStream(_raw_in.get()));
    _input.reset(new Input());

    // Push limit of read bytes
    //
    _coded_in->SetTotalBytesLimit(static_cast<int>(pow(1024, 3)),
            static_cast<int>(900 * pow(1024, 2)));

    uint64_t bytes_written;
    if (!_coded_in->ReadLittleEndian64(&bytes_written))
        throw runtime_error("failed to read events length");

    _coded_in->Skip(bytes_written);

    string message;
    if (!read(message))
        throw runtime_error("failed to read input");

    if (message.size()
            && !_input->ParseFromString(message))

            throw runtime_error("failed to decode input");

    _coded_in.reset();
    _raw_in.reset();

    _std_in.reset(new fstream(filename().c_str(), ios::in | ios::binary));
    if (!_std_in->is_open())
    {
        _std_in.reset();

        cerr << "failed to open input file: " << filename() << endl;

        return;
    }

    _raw_in.reset(new ::google::protobuf::io::IstreamInputStream(_std_in.get()));
    _coded_in.reset(new CodedInputStream(_raw_in.get()));
    _coded_in->Skip(8);

    _is_good = true;
    _current_event = 0;

    if (_delegate)
        _delegate->fileDidOpen(this);
}

void Reader::close()
{
    if (!isOpen())
        return;

    if (_delegate)
        _delegate->fileWillClose(this);

    _coded_in.reset();
    _raw_in.reset();
    _std_in.reset();
    _input.reset();

    _is_good = true;
    _current_event = 0;

    if (_delegate)
        _delegate->fileDidClose(this);
}

bool Reader::isOpen() const
{
    return _std_in
        && _std_in->is_open();
}

bool Reader::isGood() const
{
    return _is_good;
}

const Reader::InputPtr Reader::input() const
{
    return _input;
}

bool Reader::read(const EventPtr &event)
{
    if (!isOpen()
            || !isGood())
        return false;

    if (_current_event >= _input->events())
        return false;

    string message;
    if (!read(message))
        return false;

    if (message.size()
           && !event->ParseFromString(message))

        return false;

    ++_current_event;

    return true;
}

bool Reader::skip()
{
    if (!isOpen()
            || !isGood())
        return false;

    uint32_t message_size = readEventSize();
    if (!isGood())
        return false;

    if (message_size);
        _coded_in->Skip(message_size);

    ++_current_event;

    return true;
}

std::string Reader::filename() const
{
    return _filename;
}

void Reader::setDelegate(Delegate *delegate)
{
    _delegate = delegate;
}

Reader::Delegate *Reader::delegate() const
{
    return _delegate;
}



// Private
//
bool Reader::read(std::string &message)
{
    uint32_t message_size = readEventSize();
    if (!isGood())
        return false;

    if (0 < message_size)
    {
        if (!_coded_in->ReadString(&message, message_size))
            return false;
    }

    return true;
}

uint32_t Reader::readEventSize()
{
    uint32_t message_size;
    if (!_coded_in->ReadVarint32(&message_size))
    {
        _is_good = false;

        return 0;
    }

    return message_size;
}
