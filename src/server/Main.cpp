#include "GameServer.hpp"
#include <spdlog/spdlog.h>
#include <exception>


int main() 
{
    try 
    {
        GameServer server;
        server.run();
    } 
    catch (const std::exception& e) 
    {
        spdlog::critical("Server Fatal Error: {}", e.what());
        return -1;
    }
    
    return 0;
}