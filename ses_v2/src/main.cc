#include "../include/SearchEngineServer.h"
#include "../include/Configuration.h"

Configuration * Configuration::_pInstance = nullptr;
Configuration::AutoRelease Configuration::_ar ;

int main(int argc, char **argv)
{
    Configuration * conf = Configuration::getInstance();
    conf->init("../conf/myconf.conf");

    SearchEngineServer ses("127.0.0.1", 8888);
    ses.start();
    
    return 0;
}

