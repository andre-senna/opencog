#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include <vector>
#include <string>

#include "opencog.grpc.pb.h"
#include "OpencogSNETServiceFactory.h"
#include "OpencogSNETService.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace opencogservices;
using namespace std;

class ServiceImpl final : public ServiceDefinition::Service
{

public:

    explicit ServiceImpl() {
    }

    // SERVICE_API

    Status execute(ServerContext* context, const Command* input, CommandOutput* output) override {
        OpencogSNETService *opencogService = OpencogSNETServiceFactory::factory(input->cmd());
        if (opencogService == NULL) {
            //output->set_s(input->cmd() + ": Opencog service not found");
            Status error(grpc::NOT_FOUND, input->cmd() + ": Opencog service not found");
            return error;
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
                Status error(grpc::NOT_FOUND, input->cmd() + ": " + out);
                return error;
            } else {
                output->set_s(out);
                return Status::OK;
            }
        }
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
