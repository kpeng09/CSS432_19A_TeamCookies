#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include <limits.h>

#include <string>
#include <iostream>
#include <future>

#include "Player.cpp"
//#include "Etc.h"

#define DEFAULT_CUSERNAME "lllloyd"
#define DEFAULT_SPORT 6932
#define DEFAULT_CPORT 6932
#define DEFAULT_SADDRESS "uw1-320-01"
#define DEFAULT_CADDRESS "uw1-320-00"
#define DEFAULT_CSD -1

std::string cUsername = DEFAULT_CUSERNAME;
int sPort = DEFAULT_SPORT;
int cPort = DEFAULT_CPORT;
std::string sAddress = DEFAULT_SADDRESS;
std::string cAddress = DEFAULT_CADDRESS;
int cSd = DEFAULT_CSD;
sockaddr_in sendSockAddr;
bool registered;
std::string acknowledgement;

void registerUser()
{
    char loginname[LOGIN_NAME_MAX];
    getlogin_r(loginname, LOGIN_NAME_MAX);
    cUsername = std::string(loginname);
    //cUsername = Etc::consoleIn();
    Etc::consoleOut("cUsername: " + cUsername + "\n");

    srand(time(NULL));
    cPort = 1023 + rand() % (65535 - 1023);
    //cPort = stoi(Etc::consoleIn());
    Etc::consoleOut("cPort: " + std::to_string(cPort) + "\n");

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    cAddress = std::string(hostname);
    //cAddress = Etc::consoleIn();
    Etc::consoleOut("cAddress: " + cAddress + "\n");

    registered = true;
    Etc::consoleOut("registration successful!\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();
}

void listGames()
{
    std::string users = Etc::portIn(cSd);
	Etc::portOut(cSd, users); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
    Etc::consoleOut("users: \n" + users + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (users == acknowledgement), "Something has gone terribly wrong! " + users + " != " + acknowledgement);

    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();
}

void createGame()
{
	Etc::portOut(cSd, cUsername); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    Etc::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

	Etc::portOut(cSd, std::to_string(cPort)); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    Etc::consoleOut("cPort: " + std::to_string(cPort) + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (std::to_string(cPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(cPort) + " != " + acknowledgement);

	Etc::portOut(cSd, cAddress); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    Etc::consoleOut("cAddress: " + cAddress + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cAddress == acknowledgement), "Something has gone terribly wrong! " + cAddress + " != " + acknowledgement);

    std::string s = "./player.out " + std::to_string(cPort) + " " + cAddress;
    Etc::consoleOut(s + "\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();

    int ac;
    char **av = Etc::parsedargs(s, &ac);
    Player::main(ac, av);
    Etc::freeparsedargs(av);
}

void joinGame()
{
    listGames();
    Etc::consoleOut("type a username to enter a sea battle with: ");
    std::string pUsername = Etc::consoleIn();

	Etc::portOut(cSd, pUsername); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    Etc::consoleOut("pUsername: " + pUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (pUsername == acknowledgement), "Something has gone terribly wrong! " + pUsername + " != " + acknowledgement);
    
    int pPort = stoi(Etc::portIn(cSd));
	Etc::portOut(cSd, std::to_string(pPort)); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
    Etc::consoleOut("pPort: \n" + std::to_string(pPort) + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (std::to_string(pPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(pPort) + " != " + acknowledgement);

    std::string pAddress = Etc::portIn(cSd);
	Etc::portOut(cSd, pAddress); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
    Etc::consoleOut("pAddress: \n" + pAddress + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (pAddress == acknowledgement), "Something has gone terribly wrong! " + pAddress + " != " + acknowledgement);

    std::string s = "./player.out " + std::to_string(cPort) + " " + cAddress + " " + std::to_string(pPort) + " " + pAddress;
    Etc::consoleOut(s + "\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();

    int ac;
    char **av = Etc::parsedargs(s, &ac);
    Player::main(ac, av);
    Etc::freeparsedargs(av);
}

void unregisterUser()
{
    Etc::consoleOut("cUsername: " + cUsername);
	Etc::portOut(cSd, cUsername); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    Etc::consoleOut("\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    registered = false;
    Etc::consoleOut("unregistration successful!\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();
}

int main(int argc, char const *argv[])
{
    Etc::clientSetup(sPort, sAddress, cSd, sendSockAddr);
    Etc::portConnect(cSd, sendSockAddr);

    while (true) {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        Etc::consoleOut("Welcome to Sea Battle, a game by Team Cookies\n");
        if (registered) Etc::consoleOut("You are registered as " + cUsername + " on " + cAddress + ":" + std::to_string(cPort) + "\n");
        Etc::consoleOut("\nPlease select the following options by typing in the number:\n");
	    Etc::consoleOut("[1] Register User\n");
	    if (registered) Etc::consoleOut("[2] List Games\n");
	    if (registered) Etc::consoleOut("[3] Create Game\n");
	    if (registered) Etc::consoleOut("[4] Join Game\n");
	    if (registered) Etc::consoleOut("[5] Exit Game\n");
	    if (registered) Etc::consoleOut("[6] Unregister User\n");
        if (registered) Etc::consoleOut("\n");

        Etc::consoleOut("switchcase: ");
        int switchcase = stoi(Etc::consoleIn());
	    Etc::portOut(cSd, std::to_string(switchcase)); /// three-way handshake
	    acknowledgement = Etc::portIn(cSd);
	    Etc::portOut(cSd, acknowledgement);
        Etc::consoleOut("acknowledgement: " + acknowledgement + "\n");
        Etc::errChk(-1 + (std::to_string(switchcase) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(switchcase) + " != " + acknowledgement);

        switch(switchcase) {
            case 1:
                registerUser();
                break;
            case 2:
                if (registered) listGames();
                break;
            case 3:
                if (registered) createGame();
                break;
            case 4:
                if (registered) joinGame();
                break;
            // case 5:
            //     if (registered) exitGame();
            //     break;
            case 6:
                if (registered) unregisterUser();
                break;
            default:
                Etc::consoleOut("not a valid option...\n");
                Etc::consoleOut("press enter to continue\n");
                break;
        }
    }
	close(cSd);
	exit(0);
    return 0;
}
