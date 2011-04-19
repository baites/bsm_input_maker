// Input Writer
//
// Write messages into specified output file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_WRITER
#define BSM_IO_WRITER

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace bsm
{
    class Event;

    class Writer
    {
        public:
            Writer(const boost::filesystem::path &output_file);
            virtual ~Writer();

            virtual bool write(const Event &);

        private:
            std::fstream _output;
            
            typedef ::google::protobuf::io::ZeroCopyOutputStream
                ZeroCopyOutputStream;

            typedef ::google::protobuf::io::CodedOutputStream
                CodedOutputStream;

            boost::shared_ptr<ZeroCopyOutputStream> _raw_out;
            boost::shared_ptr<CodedOutputStream> _coded_out;

            int _events_written;
    };
}

#endif
