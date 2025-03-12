#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "demo.grpc.pb.h"

class GreeterServiceImpl : public hello::Greeter::Service {
    virtual ::grpc::Status SayHello(::grpc::ServerContext* context,
         const ::hello::HelloRequest* request, ::hello::HelloReply* response) override {
        std::string prefix("grpc server has receive data: ");
        response->set_message(prefix + request->message());
        return ::grpc::Status::OK;
    }
};

void RunServer() {
    std::string server_address("127.0.0.1:50051");
    GreeterServiceImpl service;

    ::grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server is listening on: " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}