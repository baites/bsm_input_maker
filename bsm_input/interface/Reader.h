// Input Reader
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_READER
#define BSM_IO_READER

#include <iosfwd>
#include <string>

#include <boost/shared_ptr.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    class Reader
    {
        public:
            class Delegate
            {
                public:
                    virtual ~Delegate() {};

                    virtual void fileWillOpen(const Reader *) {};
                    virtual void fileDidOpen(const Reader *) {};

                    virtual void fileWillClose(const Reader *) {};
                    virtual void fileDidClose(const Reader *) {};
            };

            typedef boost::shared_ptr<Input> InputPtr;
            typedef boost::shared_ptr<Event> EventPtr;

            Reader(const std::string &input);
            virtual ~Reader();

            virtual void open();
            virtual void close();

            virtual bool isOpen() const;
            virtual bool isGood() const;

            virtual const InputPtr input() const;

            virtual bool read(const EventPtr &);
            virtual bool skip();

            virtual std::string filename() const;

            void setDelegate(Delegate *);
            Delegate *delegate() const;

        private:
            // Physical read
            //
            bool read(std::string &);
            uint32_t readEventSize();

            typedef ::google::protobuf::io::ZeroCopyInputStream
                ZeroCopyInputStream;

            typedef ::google::protobuf::io::CodedInputStream
                CodedInputStream;

            std::string _filename;

            boost::shared_ptr<std::fstream> _std_in;
            boost::shared_ptr<ZeroCopyInputStream> _raw_in;
            boost::shared_ptr<CodedInputStream> _coded_in;

            bool _is_good;
            uint32_t _current_event;

            InputPtr _input;

            Delegate *_delegate;
    };
}

#endif
