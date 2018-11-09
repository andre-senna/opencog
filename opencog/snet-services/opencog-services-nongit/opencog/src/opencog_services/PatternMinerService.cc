#include "PatternMinerService.h"

using namespace opencog;
using namespace opencog::PatternMining;
using namespace opencogservices;
using namespace std;

PatternMinerService::PatternMinerService() 
{

    config().load("/tmp/opencog_patternminer_nlp.conf");
    config().set("Pattern_Max_Gram", "3");
    //config().set("Enable_Frequent_Pattern", "true");
    //config().set("Enable_Interesting_Pattern", "true");
    //config().set("Enable_surprisingness", "true");
    //config().set("enable_filter_leaves_should_not_be_vars", "true");
    //config().set("enable_filter_links_should_connect_by_vars", "true");
    //config().set("enable_filter_not_inheritant_from_same_var", "true");
    //config().set("enable_filter_not_same_var_from_same_predicate", "true");
    //config().set("enable_filter_not_all_first_outgoing_const", "true");
    //config().set("enable_filter_first_outgoing_evallink_should_be_var", "true");
    //config().set("enable_filter_node_types_should_not_be_vars", "true");
    //config().set("node_types_should_not_be_vars", "PredicateNode");
    //config().set("Pattern_mining_mode", "Depth_First");
    config().set("Max_thread_num", "8");

    //schemeEval->eval("(add-to-load-path /opencog/opencog/learning/PatternMiner)");
}

PatternMinerService::~PatternMinerService() 
{
}

// This method is called by superclass constructor
void PatternMinerService::loadModules()
{
    OpencogSNETService::loadModules();
    evaluateScheme("(use-modules (opencog nlp))");
}

bool PatternMinerService::execute(string &output, const vector<std::string> &args)
{
    if ((args.size() < 1) || (args.size() > 2)) {
        output.assign("Usage: PatternMiner <url> [config]\n\t<url> Valid URL.\n\t[config] Optional JSON array with Opencog configuration parameters.");
        return true;
    }

    string url = args.at(0);

    if (url == "") {
        output.assign("Invalid empty URL");
        return true;
    }

    if (args.size() > 1) {
        setConfigurationParameters(args.at(1));
    }

    string errorMessage;
    if (loadAtomeseFile(errorMessage, url)) {
        output.assign("Error fetching URL: " + url + "\n" + errorMessage);
        return true;
    }

    PatternMiner pm(atomSpace);
    pm.runPatternMiner(false);
    int g = config().get_int("Pattern_Max_Gram");
    vector<HTreeNode*>& topPatterns = pm.getFinalPatternsForGram(g);
    printf("g = %d size = %lu\n", g, topPatterns.size());
    string patternStr = pm.unifiedPatternToKeyString(topPatterns[0]->pattern);

    output.assign(patternStr);
    return false;
}
