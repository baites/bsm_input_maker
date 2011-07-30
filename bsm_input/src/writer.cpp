// Writer Test
//
// Write messages to specified input file
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Writer.h"

using namespace bsm;

using namespace std;

std::ostream &operator <<(std::ostream &, const Writer &);

void testEmptyFilename();
void testExistingFile();
void testExistingFiles();
void testOutputFiles(const string &);

int main(int argc, char *argv[])
try
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (2 != argc)
    {
        cerr << "Usage: " << argv[0] << " output.pb" << endl;

        google::protobuf::ShutdownProtobufLibrary();

        return 1;
    }

    testEmptyFilename();
    testExistingFile();
    testExistingFiles();
    testOutputFiles(argv[1]);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
catch(...)
{
    cerr << "Unknown error" << endl;

    google::protobuf::ShutdownProtobufLibrary();
}

std::ostream &operator <<(std::ostream &out, const Writer &writer)
{
    out << (writer.isOpen() ? "open" : "closed")
        << " " << writer.filename();

    return out;
}

void testEmptyFilename()
{
    cout << "Empty filename" << endl;
    boost::shared_ptr<Writer> writer(new Writer(""));
    writer->open();
    cout << "Writer: " << *writer << endl;
    cout << endl;
}

void testExistingFile()
{
    cout << "Existing file [existing_test_1.pb]" << endl;
    system("touch existing_test_1.pb");
    boost::shared_ptr<Writer> reader(new Writer("existing_test.pb"));
    reader->open();
    cout << "Writer: " << *reader << endl;
    cout << endl;

    system("rm existing_test_1.pb");
}

void testExistingFiles()
{
    cout << "Existing file [existing_test_1.pb]" << endl;
    for(uint32_t i = 1; 10 > i; ++i)
    {
        ostringstream command;
        command << "touch existing_test_" << i << ".pb";
        system(command.str().c_str());
    }

    boost::shared_ptr<Writer> reader(new Writer("existing_test.pb"));
    reader->open();
    cout << "Writer: " << *reader << endl;
    cout << endl;

    for(uint32_t i = 1; 10 > i; ++i)
    {
        ostringstream command;
        command << "rm existing_test_" << i << ".pb";
        system(command.str().c_str());
    }
}

void testOutputFiles(const string &filename)
{
    cout << "Output file" << endl;
    boost::shared_ptr<Writer> writer(new Writer(filename));
    writer->open();

    if (!writer->isOpen())
        return;

    cout << "generate output: " << writer->filename() << endl;
    uint32_t events = 0;
    for(boost::shared_ptr<Event> event(new Event());
            writer->write(event)
                && 1000 > events;
            ++events)
    {
        event->Clear();
    }
}
