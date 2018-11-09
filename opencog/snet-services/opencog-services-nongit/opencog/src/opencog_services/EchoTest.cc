#include "EchoTest.h"

using namespace opencogservices;
using namespace std;

EchoTest::EchoTest() 
{
}

EchoTest::~EchoTest() 
{
}

bool EchoTest::execute(string &output, const vector<std::string> &args)
{
    string out = "echo: ";
    for (unsigned int i = 0; i < args.size(); i++) {
        out += args.at(i);
        if (i != (args.size() - 1)) {
            out += " ";
        }
    }

    output.assign(out);
    return false;
}

