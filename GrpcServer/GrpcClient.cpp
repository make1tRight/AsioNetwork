#include <iostream>
#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "demo.grpc.pb.h"

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<::grpc::Channel> channel)
        : _stub(hello::Greeter::NewStub(channel)) {

    }

    std::string SayHello(std::string name) {
        grpc::ClientContext context;
        hello::HelloRequest request;
        hello::HelloReply response;

        request.set_message(name);
        ::grpc::Status status = _stub->SayHello(&context, request, &response);
        if (status.ok()) {
            return response.message();
        } else {
            return status.error_message();
        }
    }

private:
    std::unique_ptr<::hello::Greeter::Stub> _stub;
};

int main() {
    auto channel = ::grpc::CreateChannel("127.0.0.1:50051",
         ::grpc::InsecureChannelCredentials());
    GreeterClient client(channel);

    std::string result = client.SayHello("hello, make1tRight!");
    std::cout << "the result is: " << result << std::endl;
    return 0;
}