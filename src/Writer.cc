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

#include <stdexcept>
#include <sstream>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "interface/Event.pb.h"
#include "interface/Input.pb.h"

#include "interface/Writer.h"

using std::ios;
using std::ostringstream;
using std::runtime_error;
using std::string;

using bsm::Writer;

Writer::Writer(const string &output, const uint32_t &file_size):
    _path(output),
    _file_size(file_size),
    _file_number(1)
{
    _input.reset(new Input());

    open();
}

Writer::~Writer()
{
    close();
}

bool Writer::write(const Event &event)
{
    if (_file_size < _coded_out->ByteCount() / 1000)
    {
        close();
        open();
    }

    string message;
    if (!event.SerializeToString(&message))
        return false;

    write(message);

    _input->set_events(_input->events() + 1);

    return true;
}

void Writer::write(const string &message)
{
    _coded_out->WriteVarint32(message.size());
    _coded_out->WriteString(message);
}

void Writer::open()
{
    _std_out.open(filename().c_str(), ios::out | ios::trunc | ios::binary);
    ++_file_number;

    _raw_out.reset(new ::google::protobuf::io::OstreamOutputStream(&_std_out));
    _coded_out.reset(new CodedOutputStream(_raw_out.get()));

    // Pointer to the Input object
    //
    _coded_out->WriteLittleEndian64(0);
}

void Writer::close()
{
    if (!_std_out.is_open())
        return;

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

    _std_out.seekp(0);

    _raw_out.reset(new ::google::protobuf::io::OstreamOutputStream(&_std_out));
    _coded_out.reset(new CodedOutputStream(_raw_out.get()));

    _coded_out->WriteLittleEndian64(bytes_written);

    _coded_out.reset();
    _raw_out.reset();
    _std_out.close();
}

string Writer::filename()
{
    ostringstream file_name;

    string dirname = _path.parent_path().string();

    if (dirname.size())
        file_name << dirname << "/";
    
    file_name << _path.stem() << "_"
        << _file_number << _path.extension();

    return file_name.str();
}
