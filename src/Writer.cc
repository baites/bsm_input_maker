// Input Writer
//
// Write messages into specified output file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <string>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "interface/Event.pb.h"

#include "interface/Writer.h"

using std::ios;
using std::string;

using bsm::Writer;

Writer::Writer(const boost::filesystem::path &output):
    _output(output.string().c_str(),
            ios::out | ios::trunc | ios::binary),
    _events_written(0)
{
    _raw_out.reset(new ::google::protobuf::io::OstreamOutputStream(&_output));
    _coded_out.reset(new CodedOutputStream(_raw_out.get()));

    _coded_out->WriteLittleEndian32(_events_written);
}

Writer::~Writer()
{
    _coded_out.reset();
    _raw_out.reset();

    _output.seekp(0);

    _raw_out.reset(new ::google::protobuf::io::OstreamOutputStream(&_output));
    _coded_out.reset(new CodedOutputStream(_raw_out.get()));
    _coded_out->WriteLittleEndian32(_events_written);
}

bool Writer::write(const Event &event)
{
    string message;
    if (!event.SerializeToString(&message))
        return false;

    _coded_out->WriteVarint32(message.size());
    _coded_out->WriteString(message);

    ++_events_written;

    return true;
}
