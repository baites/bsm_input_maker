// Reader Test
//
// Read messages from specified input file
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Reader.h"

using namespace bsm;

using namespace std;

std::ostream &operator <<(std::ostream &, const Reader &);

void testEmptyFilename();
void testNonExistingFile();
void testInputFiles(const uint32_t &, char *[]);

int main(int argc, char *argv[])
try
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (2 > argc)
    {
        cerr << "Usage: " << argv[0] << " input.pb" << endl;

        google::protobuf::ShutdownProtobufLibrary();

        return 1;
    }

    testEmptyFilename();
    testNonExistingFile();
    testInputFiles(argc, argv);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
catch(...)
{
    cerr << "Unknown error" << endl;

    google::protobuf::ShutdownProtobufLibrary();
}

std::ostream &operator <<(std::ostream &out, const Reader &reader)
{
    out << (reader.isOpen() ? "open" : "closed")
        << " "
        << (reader.isGood() ? "good" : "bad");

    return out;
}

void testEmptyFilename()
{
    cout << "Empty filename" << endl;
    boost::shared_ptr<Reader> reader(new Reader(""));
    reader->open();
    cout << "Reader: " << *reader << endl;
    cout << endl;
}

void testNonExistingFile()
{
    cout << "Empty filename" << endl;
    boost::shared_ptr<Reader> reader(new Reader("/bla/bla.pb"));
    reader->open();
    cout << "Reader: " << *reader << endl;
    cout << endl;
}

void testInputFiles(const uint32_t &argc, char *argv[])
{
    cout << "Input files" << endl;
    for(uint32_t i = 1; argc > i; ++i)
    {
        boost::shared_ptr<Reader> reader(new Reader(argv[i]));
        reader->open();

        if (!reader->isOpen())
            continue;

        cout << "process: " << reader->filename();
        for(boost::shared_ptr<Event> event(new Event());
                reader->read(event);
           )
        {
            event->Clear();
        }
    }
}
