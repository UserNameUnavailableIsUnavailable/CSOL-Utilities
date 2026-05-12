#include <grpcpp/grpcpp.h>

#include "Protos/Controller/Configuration.pb.h"

namespace CSOL_Utilities
{
class Server
{
public:
    Server(const std::string& address, const std::string& port);
    ~Server() noexcept;

    void Start();
    void Stop();
    void RegisterService(const std::string& service_name);

private:
    std::string address_;
    std::string port_;

    // Additional members for server implementation (e.g., socket, threads, etc.)
};
}
