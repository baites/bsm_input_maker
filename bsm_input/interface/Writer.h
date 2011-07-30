// Input Writer
//
// Write messages into specified output file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_IO_WRITER
#define BSM_IO_WRITER

#include <iosfwd>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace fs = boost::filesystem;

namespace bsm
{
    class Writer
    {
        public:
            class Delegate
            {
                public:
                    virtual ~Delegate() {};

                    virtual void fileWillOpen(const Writer *) {};
                    virtual void fileDidOpen(const Writer *) {};

                    virtual void fileWillClose(const Writer *) {};
                    virtual void fileDidClose(const Writer *) {};
            };

            typedef boost::shared_ptr<Input> InputPtr;
            typedef boost::shared_ptr<Event> EventPtr;

            Writer(const std::string &output_file,
                    // Filesize is in KB
                    //
                    const uint32_t &file_size = 10000);
            virtual ~Writer();

            virtual void open();
            virtual void close();

            virtual bool isOpen() const;

            virtual const InputPtr input() const;

            virtual bool write(const EventPtr &);

            virtual std::string filename() const;

            void setDelegate(Delegate *);
            Delegate *delegate() const;

        private:
            // Physical write to the file
            //
            void write(const std::string &);

            void generateFilename();

            const uint32_t _max_file_name_attempts;
            const uint32_t _max_file_size;

            fs::path _path;
            std::string _filename;

            uint32_t _file_number;
            
            typedef ::google::protobuf::io::ZeroCopyOutputStream
                ZeroCopyOutputStream;

            typedef ::google::protobuf::io::CodedOutputStream
                CodedOutputStream;

            boost::shared_ptr<std::fstream> _std_out;
            boost::shared_ptr<ZeroCopyOutputStream> _raw_out;
            boost::shared_ptr<CodedOutputStream> _coded_out;

            InputPtr _input;

            Delegate *_delegate;
    };
}

#endif
