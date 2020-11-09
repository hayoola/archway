#include <drogon/drogon.h>
#include <iostream>

int main() {
    //Set HTTP listener address and port
    //drogon::app().addListener("0.0.0.0",7671);
    //Load config file
    drogon::app().loadConfigFile("./config.json");
    std::cout << "The drogon config file was parsed.\n";
    //Run HTTP framework,the method will block in the internal event loop
    std::cout << "Starting the web server....\n";

    drogon::app().run();

    return 0;
}
