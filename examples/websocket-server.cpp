
#include <cstdio>
#include <cstdlib>
#include <csignal>

#include "jsonrpc.h"

#include "test-rpc.h"
/**
 * \var g_run
 * \brief Running state of the program.
 */
static volatile bool g_run = false;
Json::Rpc::WebsockServer g_server(std::string("127.0.0.1"), 8086);
/**
 * \brief Signal management.
 * \param code signal code
 */
static void signal_handler(int code)
{
  switch(code)
  {
    case SIGINT:
    case SIGTERM:
      g_run = false;
      g_server.Close();
      break;
    default:
      break;
  }
}

/**
 * \brief Entry point of the program.
 * \param argc number of argument
 * \param argv array of arguments
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char** argv)
{
  TestRpc a;
  
  
  /* to avoid compilation warnings */
  argc = argc;
  argv = argv;

  if(!networking::init())
  {
    std::cerr << "Networking initialization failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  if(signal(SIGTERM, signal_handler) == SIG_ERR)
  {
    std::cout << "Error signal SIGTERM will not be handled" << std::endl;
  }

  if(signal(SIGINT, signal_handler) == SIG_ERR)
  {
    std::cout << "Error signal SIGINT will not be handled" << std::endl;
  }
  g_server.AddMethod(new Json::Rpc::RpcMethod<TestRpc>(a, &TestRpc::Print,
        std::string("print"), a.GetDescription()));
  g_server.AddMethod(new Json::Rpc::RpcMethod<TestRpc>(a, &TestRpc::Notify,
        std::string("notify"), a.GetDescription()));

  
  if(!g_server.Bind())
  {
    std::cout << "Bind failed" << std::endl;
    exit(0);
  }

  if(!g_server.Listen())
  {
    std::cout << "Listen failed" << std::endl;
    exit(EXIT_FAILURE);
  } 


  std::cout << "Stop JSON-RPC websocket server" << std::endl;
  
  return EXIT_SUCCESS;
}

