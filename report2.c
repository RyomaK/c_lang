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

//hword = unsigned short (unsigned int)

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


//走査時方向
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
//初期化
void init() {
	hword *ptr;
	ptr = (hword*)0x04000000;
	*ptr = 0x0F03;

	ptr = (hword*)VRAM;
	*ptr = WHITE;

	player.x = 1;
	player.y = 1;	


}
//ブロック表示
void block(hword color) {
	hword tx, ty;

	for (tx = 0; tx < 16; tx++) {
		for (ty = 0; ty < 16; ty++) {
			draw_point((p.x + tx), (p.y + ty), color);
		}
	}
}


//8*8
void locate(hword cx, hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 3;				/* x座標の指定 */
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	/* xの最大値を設定 */
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 3;				/* y座標の指定 */
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	/* yの最大値を設定 */
	}
}

//16*16
void locate_board(hword cx , hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 4;				/* x座標の指定 */
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	/* xの最大値を設定 */
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 4;				/* y座標の指定 */
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	/* yの最大値を設定 */
	}
}





//数字表示
void printn(hword val) {

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;
	
	tmp = val;
	for (i = 0; i < 5; i++) {
		if (tmp >= 10){
			tmp = div(tmp, 10);
		} else {
			break;
		}
	}

	
	buf[i+1] = 0;
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}
	
	prints(buf);
	
	return;
}

//割り算
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

//余り
hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			
	
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

//文字列表示
void prints(byte *str) {

	while(*str) {
		print_ascii(*str++);
		p.x = p.x + CHAR_WIDTH;			
	}	
}


/* locateで場所決め,8*8aschii.h*/
void print_ascii(byte ascii_num) {
	
	hword tx, ty;							
	byte cbit;								

	for(ty = 0; ty < 8; ty++) {				
		
		cbit = 0x80;						
		for(tx = 0; tx < 8; tx++) {							
			if((char8x8[ascii_num][ty] & cbit) == cbit){	
				draw_point((p.x + tx), (p.y + ty), BGR(0x1F, 0x1F, 0x1F));	
			}else {
				draw_point((p.x + tx), (p.y + ty), BGR(0x00, 0x00, 0x00));	
			}
			cbit = cbit >> 1;	
		}
	}
}

//座標に点を打つ
void draw_point(hword x, hword y, hword color) {

	hword *ptr;
	ptr = (hword *) VRAM;
	
	ptr = ptr + x + y * LCD_WIDTH;
	*ptr = color;

}

//key入力
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
//盤面表示
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
//コマ表示
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
//カーソル設定
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


//vecの向きにひっくり返るコマがあるか確認
int check_flip(int x,int y,int turn,int vec)
{	int flag = 0;
	while(1){
		y += vec_y[vec];
		x += vec_x[vec];
		
		if( x < 0 || y < 0 || x > 7 || y > 7) return 0;
		
		if(board[y][x] == 0) return 0;
		
		if(board[y][x] == (turn*-1)){
			flag = 1;
			continue;
		}
		
		if(flag == 1) break;
		return 0;
	}
	return 1;
}

//コマを置けるか確認する関数
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

//コマ裏返す
void flip(int x,int y,int turn,int vec){
	while(1){
		y += vec_y[vec];
		x += vec_x[vec];
		
		if(board[y][x] == turn) {
			break;
		}
		
		board[y][x] = turn;
	}
}

//コマを置く
int put(int x,int y,int turn){
	int vec,flag=0;
	
	if(board[y][x] != 0) return 0;

	for(vec=0 ; vec < 8 ; ++vec){
		if(check_flip(x,y,turn,vec) == 1){
			flip(x,y,turn,vec);
			flag = 1;
		}
	}
	if(flag == 1){
		board[y][x] = turn;
		return 1;
	}
	
	return 0;
}

//コマが置けるか
int check_end(int turn)
{
	hword i,j;
	
	for(i = 0 ; i < 8; ++i){
		for(j = 0 ; j<8 ; ++j){
			if(board[j][i] == 0 && check(i,j,turn) == 1) return 0;
		}
	}
	//パスしてターン変更した場合
	turn = turn * -1;
	for(i = 0 ; i < 8 ; ++i){
		for(j = 0 ; j < 8; ++j){
			if(board[j][i] == 0 && check(i,j,turn) == 1) return 1;
		}
	}
	
	
	return 2;
}

//cpuの手
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

//コマを数える
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

