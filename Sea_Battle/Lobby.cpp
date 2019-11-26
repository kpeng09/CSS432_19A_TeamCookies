#include "Lobby.h"
// #include "Server.cpp"

#include <iostream>
#include <vector>
#include <iterator> 
#include <string>

void Lobby::startGame() {
    std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies\n. Please select the following options." << std::endl;


    startMenu();
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v) 
{ 
    os << "["; 
    for (int i = 0; i < v.size(); ++i) { 
        os << v[i]; 
        if (i != v.size() - 1) 
            os << ", "; 
    } 
    os << "]\n"; 
    return os; 
}

void Lobby::startMenu() {
    std::cout << "Please select the following options by typing in the number:" << std::endl;
	std::cout << "[1] Register" << std::endl;
	std::cout << "[2] List Games" << std::endl;
	std::cout << "[3] Create Game" << std::endl;
	std::cout << "[4] Join Game" << std::endl;
	std::cout << "[5] Exit Game" << std::endl;
	std::cout << "[6] Unregister" << std::endl;
    std::cout << std::endl;

    int userResponse = 0;
    std::cin >> userResponse;

    switch(userResponse) {
        case 1:
            registerUser();
            break;
        case 2:
            listGames();
            break;
        case 3:
            createGame();
            break;
        case 4:
            joinGame();
            break;
        case 5:
            exitGame();
            break;
        case 6:
            unregisterUser();
            break;
        default:
            std::cout << "Not a valid option. Please reselect." << std::endl;
            break;
    }
}

void Lobby::registerUser() {
    std::cout << "running registerUser" << std::endl;

    std::cout << "Please enter a name for this user: " << std::endl;
    std::string username = "";
    std::cin >> username;

    this->listOfUsers.push_back(username);

    std::cout << listOfUsers;   

    std::cout << "User created. Returning to the start menu." << std::endl;
    std::cout << std::endl;

    startMenu();
}

void Lobby::unregisterUser() {
    std::cout << "running unregisterUser" << std::endl;
    
    std::cout << "Please enter a name of the user you want to remove: " << std::endl;
    std::string username = "";
    std::cin >> username;
    
    std::vector<int>::iterator iter;

    iter = std::find(this->listOfUsers.begin(), this->listOfUsers.end(), username);

    if (iter != this->listofUsers.end()) {
        std::cout << "User found." << endl;
        listOfUsers.erase(std::remove(listOfUsers.begin(), listOfUsers.end(), username), listOfUsers.end());
        std::cout << "User deleted. Returning to the start menu." << std::endl;
        std::cout << std::endl;
        
        startMenu();
    } else {
        std::cout << "User not found." << std::endl;
        std::cout << "Please select the following option by typing in the number: " << std::endl;
        std::cout << "[1] Re-enter username." << std::endl;
        std::cout << "[2] Return to menu." << std::endl;
        std::cout << "[3] Exit game." << std::endl;

        int userResponse = 0;
        std::cin >> userResponse;

        switch(userResponse) {
            case 1:
                unregisterUser();
            case 2:
                startGame();
            case 3:
                return 0;
        }
    }

    return 0;
}

void Lobby::listGames() {
    std::cout << "running listGames" << std::endl;
}

void Lobby::createGame() {
    std::cout << "running createGame" << std::endl;
}

void Lobby::joinGame() {
    std::cout << "running joinGame" << std::endl;
}

void Lobby::exitGame() {
    std::cout << "running exitGame" << std::endl;
}