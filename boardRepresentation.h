#define ROWS 8
#define COLS 8
#define WHITE 0
#define BLACK 1

//testing fen
extern char fen[];

extern int castlingRights;
extern char enPassantSquare; //impossible to have en passant square 0 (a8), use it as no en passant
extern int fiftyMoveCounter;

extern int turn;
extern const char *sideToAscii[];

//represent pieces with characters
extern const char ascii[];

// converts the square into understandable coordinates
extern const char *squareToCoords[];

//needed types, e:empty, o:outer square
enum pieces{e, P, R, N, B, Q, K, p, r, n, b, q, k, o};
enum castling{KC = 8, QC = 4, kc = 2, qc = 1}; //every castling right is a different bit, ie 1000, 0100, 0010, 0001
enum squares{
	a1 = 0, b1, c1, d1, e1, f1, g1, h1,
	a2 = 16, b2, c2, d2, e2, f2, g2, h2,
	a3 = 32, b3, c3, d3, e3, f3, g3, h3,
	a4 = 48, b4, c4, d4, e4, f4, g4, h4,
	a5 = 64, b5, c5, d5, e5, f5, g5, h5,
	a6 = 80, b6, c6, d6, e6, f6, g6, h6,
	a7 = 96, b7, c7, d7, e7, f7, g7, h7,
	a8 = 112, b8, c8, d8, e8, f8, g8, h8,
};
enum deltaPieces{whitePawn = 64, blackPawn = 32, king = 16, knight = 8, queen = 4, rook = 2, bishop = 1};

extern int pieceValuesToDeltaPieces[];

extern int delta[];
extern int slidingDelta[];

extern const int asciiToCastling[];
extern const int asciiToPieces[];
extern struct Piece *board[];

//updated struct for pieces
typedef struct Piece{
	char pieceValue;
	char pieceLocation;
	char pieceSide;
}Piece;

extern Piece *whitePieces[];
extern Piece *blackPieces[];
extern Piece *pNoPiece;

int coordsToSquare(char *);
void setBoardFromFEN(char *);
void resetBoard(void);
void printBoard(void);
void printPieces(int);