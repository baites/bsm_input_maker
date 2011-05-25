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
#include <boost/filesystem.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace fs = boost::filesystem;

namespace bsm
{
    class Event;
    class Input;

    class Writer
    {
        public:
            typedef boost::shared_ptr<Input> InputPtr;

            Writer(const std::string &output_file,
                    // Filesize is in KB
                    //
                    const uint32_t &file_size = 10000);
            virtual ~Writer();

            virtual const InputPtr input() const;

            virtual bool write(const Event &);

            virtual std::string filename() const;

        private:
            // Physical write to the file
            //
            void write(const std::string &);

            void open();
            void close();

            void generateFilename();

            std::fstream _std_out;
            fs::path _path;
            std::string _filename;
            const int _file_size;

            uint32_t _file_number;
            
            typedef ::google::protobuf::io::ZeroCopyOutputStream
                ZeroCopyOutputStream;

            typedef ::google::protobuf::io::CodedOutputStream
                CodedOutputStream;

            boost::shared_ptr<ZeroCopyOutputStream> _raw_out;
            boost::shared_ptr<CodedOutputStream> _coded_out;

            InputPtr _input;
    };
}

#endif
