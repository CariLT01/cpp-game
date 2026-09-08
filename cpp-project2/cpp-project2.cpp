// cpp-project2.cpp : Defines the entry point for the application.
//

#include "cpp-project2.h"
#include "engine.hpp"
#include "crash_handler.hpp"
#include <memory>

#include <spdlog/spdlog.h>


int main()
{

    std::unique_ptr<CrashHandler> crashHandler = std::make_unique<CrashHandler>();

	std::cout << "PowerPath but better" << std::endl;
	
    spdlog::set_level(spdlog::level::debug);

    try {
        std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	    engine->run();
    } catch (const std::exception &e) {
        spdlog::error(std::string("FATAL: runtime exception: ") + e.what());
    }


	

	return 0;
}
