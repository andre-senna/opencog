#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "opencog.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using namespace opencogservices;

class ServiceClient {

    public:

        ServiceClient(std::shared_ptr<Channel> channel) : stub_(ServiceDefinition::NewStub(channel)) {
        }

        // TEST_CODE

        void doSomething(int argc, char** argv) {

            Command cmd;
            CommandOutput output;
            Status status;
            ClientContext context;

            if ((argc < 2) || (argc > 7)) {
                printf("Usage: client <cmd> <args>\n(0 to 5 args)\n");
                exit(0);
            } 

            cmd.set_cmd(argv[1]);
            unsigned int nargs = argc - 2;
            unsigned int p = 0;
            while (true) {
                if (p++ == nargs) break;
                cmd.set_arg1(argv[p + 1]);
                if (p++ == nargs) break;
                cmd.set_arg2(argv[p + 1]);
                if (p++ == nargs) break;
                cmd.set_arg3(argv[p + 1]);
                if (p++ == nargs) break;
                cmd.set_arg4(argv[p + 1]);
                if (p++ == nargs) break;
                cmd.set_arg5(argv[p + 1]);
            }


            status = stub_->execute(&context, cmd, &output);

            if (! status.ok()) {
                printf("RPC failed - %s\n", status.error_message().c_str());
            } else {
                printf("%s\n", output.s().c_str());
            }
        }

    private:

        std::unique_ptr<ServiceDefinition::Stub> stub_;
};

int main(int argc, char** argv) {
    ServiceClient guide(grpc::CreateChannel("localhost:7070", grpc::InsecureChannelCredentials()));
    guide.doSomething(argc, argv);
    return 0;
}
