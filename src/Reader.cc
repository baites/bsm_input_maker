// Input Reader
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <math.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "interface/Event.pb.h"
#include "interface/Input.pb.h"

#include "interface/Reader.h"

using std::cout;
using std::endl;
using std::ios;
using std::string;
using std::runtime_error;

using bsm::Reader;

Reader::Reader(const boost::filesystem::path &input):
    _std_in(input.string().c_str(),
            ios::in | ios::binary),
    _is_good(true),
    _current_event(0)
{
    _raw_in.reset(new ::google::protobuf::io::IstreamInputStream(&_std_in));
    _coded_in.reset(new CodedInputStream(_raw_in.get()));

    // Push limit of read bytes
    //
    _coded_in->SetTotalBytesLimit(pow(1024, 3), 900 * pow(1024, 2));

    _input.reset(new Input());

    uint64_t bytes_written;
    if (!_coded_in->ReadLittleEndian64(&bytes_written))
        throw runtime_error("failed to read events length");

    _coded_in->Skip(bytes_written);

    string message;
    if (!read(message))
        throw runtime_error("failed to read input");

    if (message.size()
            && !_input->ParseFromString(message))

            throw runtime_error("failed to read input");

    _coded_in.reset();
    _raw_in.reset();

    _std_in.close();
    _std_in.open(input.string().c_str(), ios::in | ios::binary);

    _raw_in.reset(new ::google::protobuf::io::IstreamInputStream(&_std_in));
    _coded_in.reset(new CodedInputStream(_raw_in.get()));

    _coded_in->Skip(8);
}

Reader::~Reader()
{
}

bool Reader::good() const
{
    return _is_good;
}

const Reader::InputPtr Reader::input() const
{
    return _input;
}

bool Reader::read(Event &event)
{
    if (!good())
        return false;

    if (_current_event == _input->events())
        return false;

    string message;
    if (!read(message))
        return false;

    if (message.size()
           && !event.ParseFromString(message))

        return false;

    ++_current_event;

    return true;
}

bool Reader::skip()
{
    if (!good())
        return false;

    uint32_t message_size;
    if (!_coded_in->ReadVarint32(&message_size))
    {
        _is_good = false;

        return false;
    }

    _coded_in->Skip(message_size);

    ++_current_event;

    return true;
}

bool Reader::read(std::string &message)
{
    uint32_t message_size;
    if (!_coded_in->ReadVarint32(&message_size))
    {
        _is_good = false;

        return false;
    }

    if (0 < message_size)
    {
        if (!_coded_in->ReadString(&message, message_size))
            return false;
    }

    return true;
}
