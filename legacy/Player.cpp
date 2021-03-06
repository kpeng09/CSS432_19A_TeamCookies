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

#include "Board.h"
#include "Static.h"

#define DEFAULT_IPORT 6932
#define DEFAULT_IADDRESS "localhost"
#define DEFAULT_UPORT 6933 // port and address cannot both be the same
#define DEFAULT_UADDRESS "localhost"
#define DEFAULT_SERVERSD -1
#define DEFAULT_NEWSD -1
#define DEFAULT_CLIENTSD -1

int iPort = DEFAULT_IPORT;
std::string iAddress = DEFAULT_IADDRESS;
int uPort = DEFAULT_UPORT;
std::string uAddress = DEFAULT_UADDRESS;
int serverSd = DEFAULT_SERVERSD;
int newSd = DEFAULT_NEWSD;
int clientSd = DEFAULT_CLIENTSD;

int getIPort( ) { return iPort; }
std::string getIAddress( ) { return iAddress; }
int getUPort( ) { return uPort; }
std::string getUAddress( ) { return uAddress; }
int getServerSd( ) { return serverSd; }
int getNewSd( ) { return newSd; }
int gStaticlientSd( ) { return clientSd; }

void setIPort( int i ) { iPort = i; }
void setIAddress( std::string i ) { iAddress = i; }
void setUPort( int u ) { uPort = u; }
void setUAddress( std::string u ) { uAddress = u; }
void setServerSd( int s ) { serverSd = s; }
void setNewSd( int n ) { newSd = n; }
void sStaticlientSd( int c ) { clientSd = c; }

int main( int argc, char *argv[] ) {
	Static::consoleOut("\npreparing local resources...");
	
	if (argc == 0){
		Static::errChk(-1, "Usage: ./player.out [iport] [uport] [uaddress]");
    }
	else if (argc == 3){ // two args: first player waits for a challenger
		setIPort(std::stoi(argv[1]));
		setIAddress(argv[2]);
	}
	else if (argc == 4){ // three args: players know each other
		setIPort(std::stoi(argv[1]));
		setUPort(std::stoi(argv[2]));
		setUAddress(argv[3]);
	}
    else if (argc == 5){ // four args: second player sends its connection info to challenger
		setIPort(std::stoi(argv[1]));
		setIAddress(argv[2]);
		setUPort(std::stoi(argv[3]));
		setUAddress(argv[4]);
	}
	else {
		Static::errChk(-1, "usage: ./player.out [iport] [uport] [uaddress]");
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	Static::consoleOut("\nlocal resources prepared!");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	Static::consoleOut("\nestablishing remote connection...");
	
	sockaddr_in newSockAddr;
    Static::serverSetup(getIPort(), serverSd);
	if (argc == 3) {
        pthread_t t;
        Static::startDots(t);
        
		newSd = Static::portAccept(serverSd, newSockAddr);

		setUPort(std::stoi(Static::portIn(newSd))); /// three-way handshake
		Static::portOut(newSd, std::to_string(getUPort()));
		std::string sPort = Static::portIn(newSd); 

		setUAddress(Static::portIn(newSd));
		Static::portOut(newSd, getUAddress());
		std::string sAddress = Static::portIn(newSd);
		
		Static::errChk(-1 + (sPort == std::to_string(getUPort()) && sAddress == getUAddress()), "Something has gone terribly wrong! " + sPort + " != " + std::to_string(getUPort()) + " || " + sAddress + " != " + getUAddress());
        
        Static::stopDots(t);
	}
	
    sockaddr_in sendSockAddr;
    Static::clientSetup(getUPort(), getUAddress(), clientSd, sendSockAddr);
	if (argc == 5) { /// send iPort iAddress
    //TODO: If the port connection fails (ex. desired other player is not running), this should fail gracefully.
		Static::portConnect(clientSd, sendSockAddr);
		
		Static::portOut(clientSd, std::to_string(getIPort())); /// three-way handshake
		std::string sPort = Static::portIn(clientSd);
		Static::portOut(clientSd, sPort);
		
		Static::portOut(clientSd, getIAddress());
		std::string sAddress = Static::portIn(clientSd);
		Static::portOut(clientSd, sAddress);
		
		Static::errChk(-1 + (sPort == std::to_string(getIPort()) && sAddress == getIAddress()), "Something has gone terribly wrong! " + sPort + " != " + std::to_string(getIPort()) + " || " + sAddress + " != " + getIAddress());
	}
	
	/* Credit to GitHub user vmrob for simple nonblocking read from std::cin
	https://gist.github.com/vmrob/ff20420a20c59b5a98a1 */
	std::future<void> portConnect = std::async(std::launch::async, Static::portConnect, clientSd, sendSockAddr); // TODO: Redundant initialization,
	std::future<int> portAccept = std::async(std::launch::async, Static::portAccept, serverSd, newSockAddr); // since this is repeated in the loop?
	
	while (newSd == -1) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
		
		if (portConnect.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			portConnect = std::async(std::launch::async, Static::portConnect, clientSd, sendSockAddr);
		}
	
		if (portAccept.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			newSd = portAccept.get();
			portAccept = std::async(std::launch::async, Static::portAccept, serverSd, newSockAddr);
		}
	}

	Static::consoleOut("\nRemote connection established!\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "iport: " << getIPort() << " iaddress: " << getIAddress() << std::endl;
	std::cout << "uport: " << getUPort() << " uaddress: " << getUAddress() << std::endl;
    std::cout << "clientSd: " << gStaticlientSd() << " serverSd: " << getServerSd() << " newSd: " << getNewSd() << std::endl;
    std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
	Static::consoleOut("welcome to Sea Battle, a game by Team Cookies\n");

    Static::consoleOut("field: \n");
	Board field; // Player's board
	field.initBoardSea();
	field.initBoardShips(); // Field knows player ship locations
	field.printBoard();

    Static::consoleOut("map: \n");
	Board map; // Enemy's board
	map.initBoardSea();
	//map.initBoardShips(); // Map does not know enemy ship locations
	map.printBoard();

    int turn = 0;
    while (true) {
        turn++;
        std::string mapTargStaticoords;
        std::string fieldTargStaticoords;

        Static::consoleOut("[char a-f][int 1-6] launch an attack on coordinate: "); //cout-cin combo creates a GUI facade
        std::future<std::string> consoleIn = std::async(std::launch::async, Static::consoleIn); // TODO: Redundant async initialization,
        std::future<std::string> fieldTargStaticoordsIn = std::async(std::launch::async, Static::portIn, newSd); // when updated in the loop?
        std::future<void> mapTargStaticoordsOut = std::async(std::launch::async, Static::portOut, clientSd, mapTargStaticoords);

        bool warning = false;
        while (mapTargStaticoords.empty() || fieldTargStaticoords.empty()) {
            if (!mapTargStaticoords.empty()) {
                if (warning == false) {
                    Static::consoleOut("waiting for opponent to make a move.");
                    warning = true;
                }
                Static::consoleOut(".");
            }
            else if (!fieldTargStaticoords.empty()) {
                if (warning == false) {
                    Static::consoleOut("\nopponent is waiting for you to make a move!\nlaunch an attack on coordinate: ");
                    warning = true;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second

            if (consoleIn.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargStaticoords = consoleIn.get();
                Static::validateCoord(mapTargStaticoords);
                consoleIn = std::async(std::launch::async, Static::consoleIn);
                mapTargStaticoordsOut = std::async(std::launch::async, Static::portOut, clientSd, mapTargStaticoords); // Is using future on this line necessary if not blocking program?
            }
            if (fieldTargStaticoordsIn.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                fieldTargStaticoords = fieldTargStaticoordsIn.get();
                fieldTargStaticoordsIn = std::async(std::launch::async, Static::portIn, newSd);
            }
        }

        Static::consoleOut("\nincoming transmission...\n");

        std::string fieldTargetStatus = field.attackBoard(field.requestTranslator(fieldTargStaticoords[0]), field.requestTranslator(fieldTargStaticoords[1])); // Assumes coords are at pos 0 and 1
        std::string mapTargetStatus;
        std::future<void> fieldTargetStatusOut = std::async(std::launch::async, Static::portOut, clientSd, fieldTargetStatus); // TODO: Redundant initialization if updated in loop?
        std::future<std::string> mapTargetStatusOut = std::async(std::launch::async, Static::portIn, newSd);
        
        while (mapTargetStatus.empty()) {
            //Alternatively to threading, we could implement style of Aloha which waits for a random amount of time to send, and listen for the rest of the time.
            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second
            //std::cout << "send: " << send << "recieve: " << recieve << std::endl;
            
            if (fieldTargetStatusOut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) { // TODO: is this necessary, since write does not block the program?
                fieldTargetStatusOut = std::async(std::launch::async, Static::portOut, clientSd, fieldTargetStatus);
            }
            if (mapTargetStatusOut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargetStatus = mapTargetStatusOut.get();
                mapTargetStatusOut = std::async(std::launch::async, Static::portIn, newSd);
            }
        }

        Static::consoleOut(fieldTargStaticoords + " is being attacked... " + fieldTargetStatus);
        Static::consoleOut("launching attack on " + mapTargStaticoords + "... " + mapTargetStatus + "\n");
	    map.setBoard(map.requestTranslator(mapTargStaticoords[0]), map.requestTranslator(mapTargStaticoords[1]), map.responseTranslator(mapTargetStatus[0])); // Assumes coords and status are at pos 0 and 1
        Static::consoleOut("key: -sea     Oship     +miss     Xhit\n\n");
        Static::consoleOut("Field: \n");
        field.printBoard(); // TODO merge printBoard statements to print side by side
        Static::consoleOut("Map: \n");
        map.printBoard();

        //std::cout << std::flush << std::endl; // Program hangs right here and I have no idea what is wrong with it. Bth players mst press enter to proceed.
        Static::consoleOut("press enter to continue"); //I htink it may have to do with the lingering Static::consoleIn async function.
        //Symptoms so far: Sometimes the "turn sequence" will begin immediately upon pressing enter. It does this for each player, individually.
        //Other times, it will only begin , and for both players at the same time, when BOTH players have pressed enter.
        //Again, this happens imediately, without waiting for 1 second. (perhaps the peers are actuall sending a write??)
    }
    // TODO: win conditions
    std::cerr << "Until updated, this program should never reach here.";

    close(getServerSd());
    close(getNewSd());
    close(gStaticlientSd());
    exit(0);
    return 0;
}
