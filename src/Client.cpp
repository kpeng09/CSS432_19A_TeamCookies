#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include <string>
#include <iostream>
#include <future>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#define PORT_NUMBER_MAX 65535
#define PORT_NUMBER_MIN 1023
#include <pwd.h>

#include "Static.h"

#define DEFAULT_CUSERNAME ""
#define DEFAULT_SPORT 6932
#define DEFAULT_CPORT 6932
#define DEFAULT_SADDRESS "uw1-320-00"
#define DEFAULT_CADDRESS "uw1-320-01"
#define DEFAULT_CSD -1

std::string cUsername = DEFAULT_CUSERNAME;
int sPort = DEFAULT_SPORT;
int cPort = DEFAULT_CPORT;
std::string sAddress = DEFAULT_SADDRESS;
std::string cAddress = DEFAULT_CADDRESS;
int cSd = DEFAULT_CSD;
sockaddr_in sendSockAddr;

void registerUser()
{
    Static::portOut(cSd, std::to_string(1));
    //char loginname[LOGIN_NAME_MAX];
    //getlogin_r(loginname, LOGIN_NAME_MAX); // Does not work on WSL
    //cUsername = std::string(loginname); // user info from computer
    cUsername = getpwuid(getuid()) -> pw_name; // user info from computer
    //cUsername = Static::consoleIn(); // user info from console, prone to errors
    //Static::consoleOut("cUsername: " + cUsername + "\n");
    srand(time(NULL));
    cPort = PORT_NUMBER_MIN + rand() % (PORT_NUMBER_MAX - PORT_NUMBER_MIN);
    //cPort = Static::to_int(Static::consoleIn());
    //Static::consoleOut("cPort: " + std::to_string(cPort) + "\n");
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    cAddress = std::string(hostname);
    //cAddress = Static::consoleIn();
    //Static::consoleOut("cAddress: " + cAddress + "\n");

    Static::consoleOut("\nregistration successful!\n");
    Static::consoleOut("press enter to return to menu\n");
    Static::consoleIn();
}

void listGames()
{
    Static::portOut(cSd, std::to_string(2));
    std::string games = Static::portIn(cSd);
    Static::consoleOut("\ngames: " + games + "\n");
    Static::consoleOut("press enter to return to menu\n");
    Static::consoleIn();
}

void createGame()
{
    Static::portOut(cSd, std::to_string(3));
    std::string games = Static::portIn(cSd);
    Static::consoleOut("\ngames: " + games + "\n");

	Static::portOut(cSd, cUsername);
	Static::portOut(cSd, std::to_string(cPort));
	Static::portOut(cSd, cAddress);

    std::string arg = "./peer.out " + std::to_string(cPort) + " " + cAddress;
    Static::consoleOut("\nrunning: " + arg + "\n");
    Static::consoleOut("press enter to launch the game\n");
    Static::consoleIn();
    //int ac; // call Player::main(s)
    //char **av = Static::parsedargs(s, &ac);
    //Player::main(ac, av);
    //Static::freeparsedargs(av);
    system(arg.c_str()); // run (s)
}

void joinGame()
{
    Static::portOut(cSd, std::to_string(4));
    std::string games = Static::portIn(cSd);
    Static::consoleOut("\ngames: " + games + "\n");

    Static::consoleOut("type a username to enter a sea battle with: ");
    std::string pUsername = Static::consoleIn();
    if (pUsername.empty()) {
        Static::consoleOut("come on now, that user wasn't listed...\n");
        Static::consoleOut("press enter to return to menu\n");
        Static::consoleIn();
    }
    else {
	    Static::portOut(cSd, pUsername);
        int pPort = Static::to_int(Static::portIn(cSd));
        if (pPort == -1){
            Static::consoleOut("come on now, that user wasn't listed...\n");
            Static::consoleOut("press enter to return to menu\n");
            Static::consoleIn();
    }
    else {
        Static::validatePort(pPort);
        std::string pAddress = Static::portIn(cSd);
        std::string s = "./peer.out " + std::to_string(cPort) + " " + cAddress + " " + std::to_string(pPort) + " " + pAddress;
        Static::consoleOut("\nrunning: " + s + "\n");
        Static::consoleOut("press enter to launch the game\n");
        Static::consoleIn();
        //int ac; // call Player::main(s)
        //char **av = Static::parsedargs(s, &ac);
        //Player::main(ac, av);
        //Static::freeparsedargs(av);
        system(s.c_str()); // run (s)
        }
    }
}

int exitGame()
{
    Static::portOut(cSd, std::to_string(5));
    Static::consoleOut("thanks for playing Sea Battle, a game by Team Cookies");
    return 0;
}

void unregisterUser()
{
    Static::portOut(cSd, std::to_string(6));
	Static::portOut(cSd, cUsername);

    cUsername = DEFAULT_CUSERNAME;
    sPort = DEFAULT_SPORT;
    cPort = DEFAULT_CPORT;
    sAddress = DEFAULT_SADDRESS;
    cAddress = DEFAULT_CADDRESS;
    cSd = DEFAULT_CSD;

    Static::consoleOut("\nunregistration successful!\n");
    Static::consoleOut("press enter to return to menu\n");
    Static::consoleIn();
}

int main(int argc, char const *argv[])
{
    if (argc == 1){ // prompt for hostname and port
        Static::consoleOut("enter server's hostname: "); // default hostname will work 00.01% of the time
        sAddress = Static::consoleIn();
    //    Static::consoleOut("enter server's port: "); // default port will work fine 99.99% of the time
    //    sPort = Static::consoleIn();
    }
    else if (argc == 2){ // argument for hostname
        sAddress = argv[1];
    }
    else if (argc == 3){ // argument for hostname and port
        sAddress = argv[1];
        sPort = Static::to_int(argv[2]);
    }
    else{
		Static::errChk(-1, "usage: ./client.out [server hostname] [server port]");
    }

    Static::clientSetup(sPort, sAddress, cSd, sendSockAddr);
    Static::portConnect(cSd, sendSockAddr);
    while (true) {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        Static::consoleOut("welcome to Sea Battle, a game by Team Cookies\n");
        /* Credit to artist jgs for ASCII art
	    https://www.oocities.org/spunk1111/transp.htm?201911 */
        std::cout << R"(

                   ,:',:`,:'
                __||_||_||_||_||__
           ____[""""""""""""""""""]____
           \ " ''''''''''''''''''''''' \
    ~~jgs~^~^~^^~^~^~^~^~^~^~^~~^~^~~^~^~^^~~^~^
        )" << '\n';
        
        if (!cUsername.empty()) Static::consoleOut("\nyou are registered as " + cUsername + "@" + cAddress + ":" + std::to_string(cPort) + "\n");
        Static::consoleOut("select from the following options:\n");
	    if (cUsername.empty()) Static::consoleOut("[1] register user\n");
	    if (!cUsername.empty()) Static::consoleOut("[2] list games\n");
	    if (!cUsername.empty()) Static::consoleOut("[3] create game\n");
	    if (!cUsername.empty()) Static::consoleOut("[4] join game\n");
	    if (!cUsername.empty()) Static::consoleOut("[5] exit game\n");
	    if (!cUsername.empty()) Static::consoleOut("[6] unregister user\n");
        if (!cUsername.empty()) Static::consoleOut("\n");

        Static::consoleOut("→ ");
        int selection = Static::to_int(Static::consoleIn());
        //Static::portOut(cSd, std::to_string(selection)); // prone to user error
        //Static::validateSelection(selection); // handled by else below

        if (selection == 1 && cUsername.empty()) registerUser();
        else if (selection == 2 && !cUsername.empty()) listGames();
        else if (selection == 3 && !cUsername.empty()) createGame();
        else if (selection == 4 && !cUsername.empty()) joinGame();
        else if (selection == 5 && !cUsername.empty()) return exitGame();
        else if (selection == 6 && !cUsername.empty()) unregisterUser();
        else {
            Static::consoleOut("not a valid option...\n");
            Static::consoleOut("press enter to return to menu\n");
            Static::consoleIn();
        }
    }
	close(cSd);
	exit(0);
    return 0;
}
