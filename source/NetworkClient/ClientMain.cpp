#include "ClientApplication.h"

#include <thread>
using namespace std::literals::chrono_literals;

// =============================================================================
int main()
{
	printf("Please enter valid IP address of the server:\n");

	std::string address;
	std::getline(std::cin, address);

	ClientApplication client;
	client.Connect(address.c_str()/*"127.0.0.1"*/, 60000);

	bool key[3] = { false, false, false };
	bool old_key[3] = { false, false, false };

	bool shouldExit = false;
	while (!shouldExit)
	{
		// Check keyboard input
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0]) client.SendMessagePingServer();
		if (key[1] && !old_key[1]) client.SendMessageMessageAll();
		if (key[2] && !old_key[2]) shouldExit = true;

		for (int i = 0; i < 3; i++) old_key[i] = key[i];

		// Poll incoming network messages
		if (client.IsConnected())
		{
			client.Update();
		}
		else
		{
			std::cout << "Server is down.\n";
			shouldExit = true;
		}

		// Limit FPS
		std::this_thread::sleep_for(20ms);
	}

	std::cin.get();
	return 0;
}