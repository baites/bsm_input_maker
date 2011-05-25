// Input Reader
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_READER
#define BSM_IO_READER

#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace bsm
{
    class Event;
    class Input;

    class Reader
    {
        public:
            typedef boost::shared_ptr<Input> InputPtr;

            Reader(const std::string &input);
            virtual ~Reader();

            virtual bool good() const;

            virtual const InputPtr input() const;

            virtual bool read(Event &);
            virtual bool skip();

            virtual std::string filename() const;

        private:
            bool read(std::string &);

            std::string _filename;
            std::fstream _std_in;
            
            typedef ::google::protobuf::io::ZeroCopyInputStream
                ZeroCopyInputStream;

            typedef ::google::protobuf::io::CodedInputStream
                CodedInputStream;

            boost::shared_ptr<ZeroCopyInputStream> _raw_in;
            boost::shared_ptr<CodedInputStream> _coded_in;

            bool _is_good;
            uint32_t _current_event;
            InputPtr _input;
    };
}

#endif
