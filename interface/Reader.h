// Input Reader
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_READER
#define BSM_IO_READER

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace bsm
{
    class Event;
    class Input;

    class Reader
    {
        public:
            Reader(const boost::filesystem::path &input);
            virtual ~Reader();

            virtual bool read(Event &);

        private:
            bool good() const;

            bool read(std::string &);

            std::fstream _std_in;
            
            typedef ::google::protobuf::io::ZeroCopyInputStream
                ZeroCopyInputStream;

            typedef ::google::protobuf::io::CodedInputStream
                CodedInputStream;

            boost::shared_ptr<ZeroCopyInputStream> _raw_in;
            boost::shared_ptr<CodedInputStream> _coded_in;

            bool _is_good;
            uint32_t _events_read;
            boost::shared_ptr<Input> _input;
    };
}

#endif
