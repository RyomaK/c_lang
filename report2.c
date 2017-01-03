#include "gba1.h"
#include "8x8ascii.h"

#define RED 0x001F
#define BLUE 0x7C00
#define CYAN 0x7FE0
#define WHITE 0x7FFF
#define BLACK 0x0000
#define YELLOW 0x03FF
#define GREEN 0x0320

#define PLAYER 1
#define COM -1 



void init();
void block(hword);
void locate(hword, hword);
void locate_board(hword,hword);
void printh(hword);
void printn(hword);
void prints(byte*);
void print_ascii(byte);
void draw_point(hword, hword, hword);
void display();
void circle(hword);
void p_cursor(hword);
int check_flip(int,int,int,int);
int check(int,int,int);
void input_key();
void flip(int,int,int,int);
int put(int,int,int);
int check_end(int);
void com_ans();
int counter();  

hword mod(hword dividened, hword divisor);
hword div(hword dividened, hword divisor);

point p;
//初期設定
point player;
point com;


//ターン交代
int next_turn;

//map
int board[8][8] = {
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,1,-1,0,0,0 },
	{ 0,0,0,-1,1,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 }

};


//走査時
int vec_y[] = {-1,-1,0,1,1,1,0,-1};
int vec_x[] = {0,1,1,1,0,-1,-1,-1};
//player BLACK = 1
//com WHITE = -1
int main(void) {
	init();
	next_turn = PLAYER;
	locate(17,1);
	prints("Othello");
	locate(17,3);
	prints("TURN:");
	locate(1,17);
	prints("if you put black piece");
	locate(1,19);
	prints("PUT A BUTTON");


	while(next_turn<2){
		input_key();
		display();
		counter();
		com_ans();

		switch(check_end(next_turn)){
			case 1:
			//pass
			next_turn = next_turn * -1;
			break;
			case 2:
			    switch(counter()){
				  case 0:
				  locate(20,15);
				    prints("COM WIN");
				  break;
				  case 1:
				  locate(20,15);
				    prints("PLAYER WIN");
				    break;
				  case 2:
				  locate(20,15);
				    prints("DRAW");
				  break;
				  default:
				  break;
				}
			  next_turn = 2;
			break;
			default:

			break;
		}

	}
	return 0;

}

void init() {
	hword *ptr;
	ptr = (hword*)0x04000000;
	*ptr = 0x0F03;

	ptr = (hword*)VRAM;
	*ptr = WHITE;

	player.x = 1;
	player.y = 1;	


}

void block(hword color) {
	hword tx, ty;

	for (tx = 0; tx < 16; tx++) {
		for (ty = 0; ty < 16; ty++) {
			draw_point((p.x + tx), (p.y + ty), color);
		}
	}
}



/* •¶ŽšÀ•W(0, 0)`(29,19)‚ÌˆÊ’u(cx, cy)‚ð‰æ–ÊÀ•W‚É•ÏŠ· */
void locate(hword cx, hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 3;				/* xÀ•W‚ÌŽw’è */
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	/* x‚ÌÅ‘å’l‚ðÝ’è */
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 3;				/* yÀ•W‚ÌŽw’è */
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	/* y‚ÌÅ‘å’l‚ðÝ’è */
	}
}

void locate_board(hword cx , hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 4;				/* xÀ•W‚ÌŽw’è */
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	/* x‚ÌÅ‘å’l‚ðÝ’è */
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 4;				/* yÀ•W‚ÌŽw’è */
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	/* y‚ÌÅ‘å’l‚ðÝ’è */
	}
}





/* ˆø”‚ð10i”‚Æ‚µ‚Ä•\Ž¦‚·‚éŠÖ”iÅ‘å’l16ƒrƒbƒgj*/
void printn(hword val) {

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;
	
	/* “ü—Í’lval‚ÌŒ…””»’è */
	tmp = val;
	for (i = 0; i < 5; i++) {
		if (tmp >= 10){
			tmp = div(tmp, 10);
		} else {
			break;
		}
	}

	/* •¶Žš—ñ‚ÌÅŒã‚ÉNULLƒR[ƒh‘}“ü */
	buf[i+1] = 0;
	
	/* Å‰ºˆÊŒ…‚Ì•¶Žši10‚ÅŠ„‚Á‚½—]‚èj‚ð‘}“ü‚µC“ü—Í’l‚ð10‚ÅŠ„‚é */
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}
	
	/* •¶Žš—ñ‘S‘Ì‚ð•\Ž¦ */
	prints(buf);
	
	return;
}


/* Š„‚èŽZ‚Ì¤‚ð‹‚ß‚éŠÖ”*/
hword div(hword dividened, hword divisor){
	
	hword quotient = 0;			
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return quotient;
}


/* Š„‚èŽZ‚Ì—]‚è‚ð‹‚ß‚éŠÖ”*/
hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			/* ¤ */
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return dividened;
}


/* •¶Žš—ñ•\Ž¦ŠÖ” */
void prints(byte *str) {

	while(*str) {
		print_ascii(*str++);
		p.x = p.x + CHAR_WIDTH;			
	}	
}


/* locate‚ÅÝ’è‚³‚ê‚½ˆÊ’u‚ÉASCIIƒR[ƒhascii_num‚É‘Š“–‚·‚é•¶Žš‚ð•\Ž¦‚·‚éŠÖ” */
void print_ascii(byte ascii_num) {
	
	hword tx, ty;							/* •¶Žšƒpƒ^[ƒ“‚ÌÀ•WŽw’è */
	byte cbit;								/* ƒrƒbƒg”»’è */

	for(ty = 0; ty < 8; ty++) {				/* •¶Žšƒpƒ^[ƒ“‚ÌyÀ•WŽw’è */
	
		cbit = 0x80;						/* ”»’èƒrƒbƒg‚ð8ƒrƒbƒg‚ÌÅãˆÊ‚ÉƒZƒbƒg */

		for(tx = 0; tx < 8; tx++) {							/* •¶Žšƒpƒ^[ƒ“‚ÌxÀ•WŽw’è */
			if((char8x8[ascii_num][ty] & cbit) == cbit){			/* ƒrƒbƒgˆÊ’u‚Ì”»’è */
				draw_point((p.x + tx), (p.y + ty), BGR(0x1F, 0x1F, 0x1F));	// 1‚È‚ç”’
			}else {
				draw_point((p.x + tx), (p.y + ty), BGR(0x00, 0x00, 0x00));	// 0‚È‚ç•
			}
			cbit = cbit >> 1;					/* ”»’èƒrƒbƒg‚ð1ƒrƒbƒg‰E‚ÖƒVƒtƒg */
		}
	}
}


void draw_point(hword x, hword y, hword color) {

	hword *ptr;
	ptr = (hword *) VRAM;
	
	ptr = ptr + x + y * LCD_WIDTH;
	*ptr = color;

}

void input_key(){
	hword temp;
	hword *key;
	key = (hword*)0x04000130;
	temp = ~(*key)&0x003FF;
	switch(temp){
		case (KEY_UP):
		if(player.y>0){
			player.y -- ;
		}
		break;
		case (KEY_DOWN):
		if(player.y<7){
			player.y ++ ;
		}
		break;

		case (KEY_RIGHT):
		if(player.x <7){
			player.x ++ ;
		}
		break;
		case (KEY_LEFT):
		if(player.x > 0){
			player.x -- ;
		}
		break;
		case (KEY_A):
		if(next_turn = PLAYER){
			if(check(player.x,player.y,PLAYER) == 1){
				put(player.x,player.y,PLAYER);
				next_turn = COM;
			}
		}
		break;
		case (KEY_B):


		break;
		default:


		break;
	}
}

void display(){
	hword tx, ty;
	for (tx = 0; tx < 8; tx++) {
		for (ty = 0; ty < 8; ty++) {
			locate_board(tx,ty);
			if(board[ty][tx] == PLAYER){
				circle(BLACK);
			}else if(board[ty][tx] == COM){
				circle(WHITE);
			}else{
				block(GREEN);
			}
			if((tx == player.x) && (ty == player.y)){
				if(check(tx,ty,PLAYER)==1){
					p_cursor(BLUE);
				}else{
				p_cursor(RED);
				}
			}
		}
	}
	if(next_turn == COM){
		locate(17,4);
		prints("COM   ");
	}else{
		locate(17,4);
		prints("PLAYER");
	}
	
}

void circle(hword color){
	hword tx,ty;
	for(tx = 0;tx<16;tx++){
		for(ty = 0;ty<16;ty++){
			if((tx-8) * (tx-8) + (ty-8) * (ty-8) < 64){
				draw_point((p.x + tx),(p.y + ty),color);
			}else{
				draw_point((p.x + tx),(p.y + ty),GREEN);
			}
		}
	}

}

void p_cursor(hword color){
	hword tx,ty;
	for(tx = 0;tx<16;tx++){
		for(ty = 0;ty<16;ty++){
			if((ty < 2|| ty > 13) | (tx < 2 || tx > 13) ){
				draw_point((p.x + tx),(p.y + ty),color);
			}else{
				
			}
		}
	}
}


//vecで指定された向きについてひっくり返るコマがあるか確認する
int check_flip(int x,int y,int turn,int vec)
{	int flag = 0;
	while(1){
		y += vec_y[vec];
		x += vec_x[vec];
		
		//盤面の外に出ていたら終了
		if( x < 0 || y < 0 || x > 7 || y > 7) return 0;
		
		//空きマスだったら終了
		if(board[y][x] == 0) return 0;
		
		//相手のコマがあったらフラグを立てる
		if(board[y][x] == (turn*-1)){
			flag = 1;
			continue;
		}
		
		//もしフラグがたっていればループ脱出。いなければ終了
		if(flag == 1) break;
		return 0;
	}
	return 1;
}

//その場所に置くことができるかを確認する関数
int check(int x,int y,int turn)
{
	int vec;
	
	//どれか一方向でもひっくり返るか確認
	if(board[y][x]==0){
	for(vec = 0 ; vec < 8 ; vec++){
		if(check_flip(x,y,turn,vec) == 1) return 1;
	}
	}
	return 0;
}

//実際に裏返す関数
void flip(int x,int y,int turn,int vec){
	while(1){
		y += vec_y[vec];
		x += vec_x[vec];
		
		//自分のコマがあったら終了
		if(board[y][x] == turn) {

			break;
		}
		//それ以外なら自分のコマで塗りつぶす
		board[y][x] = turn;
	}
}

//入力を受けて裏返せるか確かめる関数
int put(int x,int y,int turn){
	int vec,flag=0;
	
	//空白でなければ終了
	if(board[y][x] != 0) return 0;
	
	//全方向について確認
	for(vec=0 ; vec < 8 ; ++vec){
		if(check_flip(x,y,turn,vec) == 1){
			//裏返す
			flip(x,y,turn,vec);
			flag = 1;
		}
	}
	if(flag == 1){
		//この場所にコマを置く
		board[y][x] = turn;
		return 1;
	}
	
	return 0;
}


int check_end(int turn)
{
	hword i,j;
	//置ける場所があるか確認
	for(i = 0 ; i < 8; ++i){
		for(j = 0 ; j<8 ; ++j){
			//あれば続行
			if(board[j][i] == 0 && check(i,j,turn) == 1) return 0;
		}
	}
	//場所が無かったので交替して探す
	turn = turn * -1;
	for(i = 0 ; i < 8 ; ++i){
		for(j = 0 ; j < 8; ++j){
			//あればpassして続行
			if(board[j][i] == 0 && check(i,j,turn) == 1) return 1;
		}
	}
	
	//なかったのでゲーム終了
	return 2;
}

void com_ans(){
	hword x,y;	
		for(x = 0;x< 8; x++){
			for(y = 0;y<8;y++){
				if(next_turn == COM ){
					if(check(x,y,COM)==1){
						put(x,y,COM);
						next_turn = PLAYER;
						break;
					}
				}
			}
		}
	
}

int counter(){
	hword x,y;
	int black =0;
	int white=0;
	for(x=0;x<8;x++){
		for(y=0;y<8;y++){
			if(board[y][x] == 1){
				black++;
			}else if(board[y][x]==-1){
				white ++;
			}
		}
	}
	locate(17,6);
	prints("PLAYER..BLACK");
	locate(17,7);
	printn(black);
	locate(17,9);
	prints("COM..WHITE");
	locate(17,10);
	printn(white);

	if(black>white){
		return 1;
	}else if(black == white){
		return 2;
	}else{
		return 0;
	}
}

