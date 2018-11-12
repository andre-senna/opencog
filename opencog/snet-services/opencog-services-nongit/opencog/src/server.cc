#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include <future>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <string>

#include "opencog.grpc.pb.h"
#include "OpencogSNETServiceFactory.h"
#include "OpencogSNETService.h"

// seconds
#define ASYNCHRONOUS_API_UPDATE_INTERVAL ((unsigned int) 60)

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace opencogservices;
using namespace std;

static unsigned long int nextTicketCount;
static string baseOutputURL;

void exec_service(ServerContext* context, const Command* input, CommandOutput* output) {
    OpencogSNETService *opencogService = OpencogSNETServiceFactory::factory(input->cmd());
    if (opencogService == NULL) {
        output->set_s(input->cmd() + ": Opencog service not found");
    } else {
        vector<string> args;
        string out;
        // The 'while (true)' is just to avoid an annoying chain of if's 
        while (true) {
            // feed 'args' with arguments until the first empty string
            // is found
            if (input->arg1() == "") break;
            args.push_back(input->arg1());
            if (input->arg2() == "") break;
            args.push_back(input->arg2());
            if (input->arg3() == "") break;
            args.push_back(input->arg3());
            if (input->arg4() == "") break;
            args.push_back(input->arg4());
            if (input->arg5() == "") break;
            args.push_back(input->arg5());
            // Breaks anyway after feeding 5 arguments
            break;
        }
        if (opencogService->execute(out, args)) {
            output->set_s(input->cmd() + ": " + out);
        } else {
            output->set_s(out);
        }
    }
}

static void threadJobManager(ServerContext* context, const Command* input, const string &url)
{

    CommandOutput output;

    auto future = std::async(std::launch::async, exec_service, context, input, &output);

    unsigned int count = ASYNCHRONOUS_API_UPDATE_INTERVAL;
    while (true) {
        auto threadStatus = future.wait_for(std::chrono::seconds(0));
        if (threadStatus == std::future_status::ready) {
            break;
        } else {
            if (count == ASYNCHRONOUS_API_UPDATE_INTERVAL) {
                count = 0;
                auto now = std::chrono::system_clock::now();
                auto now_c = std::chrono::system_clock::to_time_t(now);
                //string s = "Work in progress. Last update: " + std::time_put(std::localtime(&now_c), "%Y-%m-%d %X") + "\n";
                char buffer[256];
                strftime(buffer, sizeof(buffer)," Work in progress. Last update: %d-%m-%Y %H:%M:%S\n", std::localtime(&now_c));
                // TODO : this need to be improved to use a more robust method
                FILE *f = fopen(url.c_str(), "w");
                fputs(buffer, f);
                fclose(f);
            }
            count++;
            sleep(1); // 1 second
        }
    }

    string s = "Service finished. Output:\n" + output.s() + "\n";
    FILE *f = fopen(url.c_str(), "w");
    fputs(s.c_str(), f);
    fclose(f);
}

class ServiceImpl final : public ServiceDefinition::Service
{

private:

public:

    explicit ServiceImpl() {
        nextTicketCount = 1;
        baseOutputURL = "/tmp/opencog-service-output-";
    }

    // SERVICE_API

    Status execute(ServerContext* context, const Command* input, CommandOutput* output) override {
        exec_service(context, input, output);
        return Status::OK;
    }

    Status asynchronousTask(ServerContext* context, const Command* input, Ticket* ticket) override {
        string url = baseOutputURL + to_string(::getpid()) + to_string(nextTicketCount++);
        ticket->set_url(url);
        printf("INPUT: %s %s\n", input->cmd().c_str(), input->arg1().c_str());
        thread t(threadJobManager, context, input, url);
        t.detach();
        sleep(1);
        return Status::OK;
    }

};

void RunServer() {
    std::string server_address("0.0.0.0:7070");
    ServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
