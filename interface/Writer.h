// Input Writer
//
// Write messages into specified output file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_WRITER
#define BSM_IO_WRITER

#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>

namespace bsm
{
    class Event;
    class Input;

    class Writer
    {
        public:
            Writer(const std::string &output_file);
            virtual ~Writer();

            virtual bool write(const Event &);

        private:
            // Physical write to the file
            //
            void write(const std::string &);

            std::fstream _std_out;
            
            typedef ::google::protobuf::io::ZeroCopyOutputStream
                ZeroCopyOutputStream;

            typedef ::google::protobuf::io::CodedOutputStream
                CodedOutputStream;

            boost::shared_ptr<ZeroCopyOutputStream> _raw_out;
            boost::shared_ptr<CodedOutputStream> _coded_out;

            boost::shared_ptr<Input> _input;
    };
}

#endif
