// Input Writer example
//
// Write messages into specified output file
//
// Created by Samvel Khalatyan, Apr 19, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/filesystem.hpp>

#include "interface/Event.pb.h"
#include "interface/Writer.h"

using bsm::Event;
using bsm::Writer;

using std::cerr;
using std::cout;
using std::endl;

namespace fs = boost::filesystem;

int main(int argc, const char *argv[])
try
{
    if (2 > argc)
    {
        cerr << "Usage: " << argv[0] << " output.pb" << endl;

        return 1;
    }

    {
        boost::shared_ptr<Writer> writer(new Writer(fs::path(argv[1])));
        boost::shared_ptr<Event> event(new Event());

        const int events = 100;

        cout << "Generate: " << events << " events" << endl;

        for(int i = 0; events > i; ++i)
        {
            writer->write(*event);
        }
    }

    return 0;
}
catch(...)
{
    cerr << "Unknown error" << endl;
}
