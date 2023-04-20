#include "ServerApplication.h"

#include <thread>
using namespace std::literals::chrono_literals;

// =============================================================================
int main()
{
	ServerApplication server(60000);
	server.Start();

	while (true)
	{
		server.Update();

		std::this_thread::sleep_for(20ms);
	}

	return 0;
}