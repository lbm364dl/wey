#include <stdio.h>
#ifndef MOVESTACK
#include "moveStack.h"
#define MOVESTACK
#endif
#ifndef MOVEGENERATOR 
#include "moveGenerator.h"
#define MOVEGENERATOR 
#endif
#ifndef BOARDREPRESENTATION 
#include "boardRepresentation.h"
#define BOARDREPRESENTATION
#endif
#include <stdlib.h>

char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

int castlingRights = 0;
char enPassantSquare = 10;
int fiftyMoveCounter = 0;

int turn = WHITE;
const char *sideToAscii[] = {"white", "black"};

//represent pieces with characters
const char ascii[] = ".PRNBQKprnbqko";

// converts the square into understandable coordinates
const char *squareToCoords[] = {
	[0]   = "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
	[16]  = "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	[32]  = "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	[48]  = "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	[64]  = "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	[80]  = "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	[96]  = "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	[112] = "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

// converts the ascii characters into castling rights
const int asciiToCastling[] = {
	['-'] = 0, //no castling
	['K'] = KC,
	['Q'] = QC,
	['k'] = kc,
	['q'] = qc,
};

// converts the ascii characters into pieces
const int asciiToPieces[] = {
	['P'] = P,
	['R'] = R,
	['N'] = N,
	['B'] = B,
	['K'] = K,
	['Q'] = Q,
	['p'] = p,
	['r'] = r,
	['n'] = n,
	['b'] = b,
	['k'] = k,
	['q'] = q,
};

//delta[0] corresponds to delta = -119
//checks who could move there
int delta[240] = {
	//horizontal moves

	//Right
	[120] = rook+queen+king, rook+queen, rook+queen, rook+queen, rook+queen, rook+queen, rook+queen,
	//Left
	[112] = rook+queen, rook+queen, rook+queen, rook+queen, rook+queen, rook+queen, rook+queen+king,
	
	//vertical moves
	
	//Up
	[135] = rook+queen+king, [151] = rook+queen, [167] = rook+queen, [183] = rook+queen, 
	[199] = rook+queen, [215] = rook+queen, [231] = rook+queen,
	//Down
	[7] = rook+queen, [23] = rook+queen, [39] = rook+queen, [55] = rook+queen, 
	[71] = rook+queen, [87] = rook+queen, [103] = rook+queen+king,
	
	//diagonal moves
	
	//Up-right
	[136] = bishop+queen+whitePawn+king, [153] = bishop+queen, [170] = bishop+queen, 
	[187] = bishop+queen, [204] = bishop+queen, [221] = bishop+queen, [238] = bishop+queen,
	//Up-left
	[134] = bishop+queen+whitePawn+king, [149] = bishop+queen, [164] = bishop+queen, [179] = bishop+queen, 
	[194] = bishop+queen, [209] = bishop+queen, [224] = bishop+queen,
	//Down-left
	[0] = bishop+queen, [17] = bishop+queen, [34] = bishop+queen, 
	[51] = bishop+queen, [68] = bishop+queen, [85] = bishop+queen, [102] = bishop+queen+blackPawn+king,
	//Down-right
	[14] = bishop+queen, [29] = bishop+queen, [44] = bishop+queen, [59] = bishop+queen, 
	[74] = bishop+queen, [89] = bishop+queen, [104] = bishop+queen+blackPawn+king,

	//knight moves

	[137] = knight, [133] = knight, [152] = knight, [150] = knight, 
	[101] = knight, [105] = knight, [86] = knight, [88] = knight
};


//for sliding pieces, get the delta which would get you to square
int slidingDelta[240] = {
	[120] = 1, 1, 1, 1, 1, 1, 1,
	[112] = -1, -1, -1, -1, -1, -1, -1,
	[135] = 16, [151] = 16, [167] = 16, [183] = 16, [199] = 16, [215] = 16, [231] = 16,
	[7] = -16, [23] = -16, [39] = -16, [55] = -16, [71] = -16, [87] = -16, [103] = -16,
	[136] = 17, [153] = 17, [170] = 17, [187] = 17, [204] = 17, [221] = 17, [238] = 17,
	[134] = 15, [149] = 15, [164] = 15, [179] = 15, [194] = 15, [209] = 15, [224] = 15,
	[0] = -17, [17] = -17, [34] = -17, [51] = -17, [68] = -17, [85] = -17, [102] = -17,
	[14] = -15, [29] = -15, [44] = -15, [59] = -15, [74] = -15, [89] = -15, [104] = -15,
};

int pieceValuesToDeltaPieces[] = {
	[p] = blackPawn, [P] = whitePawn,
	[R] = rook, [r] = rook,
	[N] = knight, [n] = knight,
	[B] = bishop, [b] = bishop, 
	[Q] = queen, [q] = queen,
	[K] = king, [k] = king
};

Piece *whitePieces[17] = {&(Piece){-1, -1, -1}, &(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},
&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},
&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},
&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-2, 0, 0}};

Piece *blackPieces[17] = {&(Piece){-1, -1, -1}, &(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},
&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},
&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},
&(Piece){-1, -1, -1},&(Piece){-1, -1, -1},&(Piece){-2, 0, 0}};

Piece *pNoPiece = &((Piece){-1, -1, -1});

Piece *board[128];

int coordsToSquare(char *coord){
	return (*coord - 'a') + (*(coord+1) - '1')*16 ;	
}

// FEN to board

void setBoardFromFEN(char *fen){
	int iw = 0, ib = 0;
	int square = 112; //a8 square
	while(*fen != ' '){
		if(*fen >= '1' && *fen <= '8'){	//if it's a number from 1 to 8
			int nullSquares = (*fen - '0');
			for( ; nullSquares > 0 ; nullSquares--){
				board[square] = NULL;
				square++;
			}					 		//advance these squares
		}
		else if(*fen == '/')			//if it's a bar, go to next rank
			square -= 24; 				//lower one rank
		else{	//it's a piece
			int pieceValue = asciiToPieces[*fen];
			if(pieceValue >= 1 && pieceValue <= 6){
				if(pieceValue == K){
					kingsLocation[WHITE] = square;
				}
				Piece *pPiece = malloc(sizeof(Piece));
				*pPiece = (Piece) {pieceValue, square, WHITE};
				whitePieces[iw] = pPiece;
				board[square] = pPiece;
				iw++;
			}else if(pieceValue >= 7 && pieceValue <= 12){
				if(pieceValue == k){
					kingsLocation[BLACK] = square;
				}
				Piece *pPiece = malloc(sizeof(Piece));
				*pPiece = (Piece) {pieceValue, square, BLACK};
				blackPieces[ib] = pPiece;
				board[square] = pPiece;
				ib++;
			}
			square++;
		}
		fen++;
	}
	fen++;
	turn = *fen == 'w' ? WHITE : BLACK; //storing side to move
	fen+=2;
	while(*fen != ' '){
		castlingRights |= asciiToCastling[*fen];
		fen++;
	}
	fen++;
	if(*fen == '-'){
		enPassantSquare = 10;
		fen+=2;
	} else{
		enPassantSquare = coordsToSquare(fen);
		fen+=3;
	}
	
	int lenFiftyMoves = 0;
	while(*fen != ' '){ //Getting size of fiftyMovesCounter from chars
		lenFiftyMoves++;
		fen++;
	}
	int i;
	unsigned int tenMultiples = 1;
	for(i = 0 ; i < lenFiftyMoves ; i++){
		fen--;
		fiftyMoveCounter += (*fen - '0')*tenMultiples;
		tenMultiples *= 10;
	}
	fen+= lenFiftyMoves + 1;
}

// clean board

void resetBoard(void){
	castlingRights = 0;
	int i, j;
	for(i = 0 ; i < ROWS ; i++)
		for(j = 0 ; j < COLS ; j++)
			board[16*i + j] = NULL;
}

void printPieces(int side){
	Piece **p = (side == WHITE) ? whitePieces : blackPieces;
	printf("%s pieces:\n", sideToAscii[side]);
	for( ; ((*p) -> pieceValue) != -2; p++){
		if((*p)->pieceValue != -1){
			printf("Piece: %c, square: %s\n", ascii[(*p) -> pieceValue], squareToCoords[(*p) -> pieceLocation]);
		}
	}
	printf("\n");
}

void printBoard(void){

	printf("Castling rights: %c%c%c%c   Value: %d\n", 	(castlingRights & KC)? 'K' : '-',
														(castlingRights & QC)? 'Q' : '-', 
														(castlingRights & kc)? 'k' : '-', 
														(castlingRights & qc)? 'q' : '-', castlingRights);
	
	printf("En passant square: %s\n", (enPassantSquare == 10) ? "-" : squareToCoords[enPassantSquare]);

	printf("Fifty moves counter: %d\n", fiftyMoveCounter);

	int i, j;
	for(i = 0 ; i < ROWS ; i++){
		printf("%d  ",ROWS - i); // prints rank numbers
		for(j = 0 ; j < COLS ; j++)
			printf("%c ",(board[16*(7- i) + j]) != NULL ? ascii[board[16*(7-i) + j] -> pieceValue] : '.'); // print the piece in the square converted to ascii char
		printf("\n");
	}
	printf("\n   A B C D E F G H\n\n"); // prints file letters
}