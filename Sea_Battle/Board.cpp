//#include "board.h"
#define BOARD_HEIGHT 6
#define BOARD_WIDTH 6
#define SHIP 'O' //○
#define HIT 'X' //●
#define SEA '-' //□
#define MISS '+' //■
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
#define RANDOM true
#define SHIPCOUNT 10

using namespace std;

//Board::Board(){
//}

//Board::~Board(){
//}

int Board::errChk(int errVal, string errMsg){
	//errVals under 0 will result in printing errMsg and exiting the program.
	//errVals over 0 will return the errVal.
	if (errVal < 0) {
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}

char Board::getBoard(int x, int y){
	return board[x][y];
}

void Board::setBoard(int x, int y, char c){
	board[x][y] = c;
}

void Board::printBoard(){
	//TODO: Is it even possible to print two boards side-by-side????
	std::cout << "  ";
	for (int x = 1; x <= BOARD_WIDTH; x++) {
		cout << std::to_string(x) << " ";
	}
	std::cout << std::endl;
	
	for (int x = 0; x < BOARD_WIDTH; x++) {
		std::cout << ALPHABET[x % BOARD_WIDTH] << " ";
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			std::cout << getBoard(x, y) << " ";
		};
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int Board::requestTranslator(char c){
	//TODO: Must be a better way of doing this.
	//Maybe switch statement? Maybe key-value dict map?
	//Also, should support the full alphabet, not just six.
	
	if (c == 'a'){
		return 0;
	}
	else if (c == 'b'){
		return 1;
	}
	else if (c == 'c'){
		return 2;
	}
	else if (c == 'd'){
		return 3;
	}
	else if (c == 'e'){
		return 4;
	}
	else if (c == 'f'){
		return 5;
	}
	else if (c == 'b'){
		return 6;
	}
	else if (c == '1'){
		return 0;
	}
	else if (c == '2'){
		return 1;
	}
	else if (c == '3'){
		return 2;
	}
	else if (c == '4'){
		return 3;
	}
	else if (c == '5'){
		return 4;
	}
	else if (c == '6'){
		return 5;
	}
	else if (c == '7'){
		return 6;
	}
	else{
		errChk(-1, "invalid input: " + to_string(c)); 
	}
}

char Board::responseTranslator(char c){
	//Same comments for the method above
	if (c == 'h'){
		return HIT;
	}
	else if (c == 'm'){
		return MISS;
	}
	else{
		errChk(-1, "invalid input: " + to_string(c));
	}
}

std::string Board::attackBoard(int x, int y){
	//TODO: error handling for chars as input, 3 ints etc.
	if (getBoard(x, y) == SHIP){
		setBoard(x, y, HIT);
		return "hit!\n";
	}
	else if (getBoard(x, y) == HIT){
		setBoard(x, y, HIT);
		return "hey, you already hit this spot...\n";
	}
	else if (getBoard(x, y) == SEA){
		setBoard(x, y, MISS);
		return "miss!\n";
	}
	else if (getBoard(x, y) == MISS){
		setBoard(x, y, MISS);
		return "my, you already missed this spot...\n";
	}
}

void Board::initBoardSea(){
	for (int x = 0; x < BOARD_HEIGHT; x++) {
		for (int y = 0; y < BOARD_WIDTH; y++) {
			setBoard(x, y, SEA);
		};
	}
}

void Board::initBoardShips(){//TODO: FLESH OUT ship implementation before improving this function!!!!!!!!
	if (RANDOM){
		int x, y;
		srand (time(NULL));
		for (int i = 0; i < SHIPCOUNT; i++){
			x = rand() % BOARD_WIDTH;
			y = rand() % BOARD_HEIGHT;
			if (getBoard(x, y) == SHIP) i--;
			else setBoard(x, y, SHIP);
		}
	}
	else{
		setBoard(1, 0, SHIP);
		setBoard(1, 1, SHIP);
		setBoard(1, 2, SHIP);
		setBoard(1, 3, SHIP);
		setBoard(3, 4, SHIP);
		setBoard(4, 0, SHIP);
		setBoard(4, 1, SHIP);
		setBoard(4, 4, SHIP);
		setBoard(5, 0, SHIP);
		setBoard(5, 1, SHIP);
		setBoard(5, 4, SHIP);
	}
}