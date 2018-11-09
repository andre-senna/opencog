#include "OpencogSNETServiceFactory.h"
#include "opencog_services/EchoTest.h"
#include "opencog_services/PatternMinerService.h"

using namespace opencogservices;
using namespace std;

OpencogSNETService *OpencogSNETServiceFactory::factory(const string &serviceName) 
{
    if (serviceName == "PatternMiner") {
        return new PatternMinerService();
    } else if (serviceName == "echo") {
        return new EchoTest();
    } else {
        return NULL;
    }
}

