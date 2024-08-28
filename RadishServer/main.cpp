#include "RadishServer.h"
#include "ChatMessage.h"

int main()
{
	/*ChatMessage msg;
	std::string str("Ê®ÔÂ");
	msg.SetProperty("name", str);
	msg.SetTextMessage("Riancy", "Haha", "login", "ÃÜÂë");

	std::cout << msg.GetData();*/

	Network::RadishServer server(8016);
	server.Accept();
	server.Run();

	std::cin.get();
	return 0;
}