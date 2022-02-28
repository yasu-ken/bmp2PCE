
int GrWidth;  //入出力しようとする画像の幅


#define BUFFSIZE 0x10000

unsigned char BinBuffer[BUFFSIZE];
int FileSize=0;

//pceの画像はデータをワードアクセスのため
unsigned short *rom_ptr;

//────────────────────────────────
//バイナリ読み込み関数
//────────────────────────────────

//int pce_Read(char *fname)
//{
//	int lp;
//	FILE *fp;
//
//	for( lp=0 ; lp<BUFFSIZE ; lp++ )
//	{
//		BinBuffer[lp] = 0;
//	}
//
//	if (( fp = fopen( fname, "rb" )) == NULL )
//	{
//		printf( "Bin file not found\n");
//		return( -1 );
//	}
//
//	FileSize = fread( BinBuffer , sizeof(char) , BUFFSIZE , fp );
//	fclose( fp );
//
//	rom_datacnt = 0x000;
//	rom_ptr     = (unsigned short *)&romdata[rom_datacnt];     //ワードポインタ
//
//	return(0);
//}


//────────────────────────────────
// スプライトの表示
//rom表示アドレスから0x2000バイト（8x8スプライト分）表示
//────────────────────────────────
//表示開始左上座標
#define DISP_RU_X 365
#define DISP_RU_Y 40

//表示位置テーブル
//2x4 8スプライトのxy位置、1チャンク内
int sprt_chank[8][2]={
	{  0,  0},		{ 16,  0},
	{  0, 16},		{ 16, 16},
	{  0, 32},		{ 16, 32},
	{  0, 48},		{ 16, 48}};


//スプライトがこの順番になるように表示
// 00 01  10 11  20 21  30 31
// 02 03  12 13  22 23  32 33
// 04 05  14 15  24 25  34 35
// 06 07  16 17  26 27  36 37
// 08 09  18 19  28 29  38 39
// 0A 0B  1A 1B  2A 2B  3A 3B
// 0C 0D  1C 1D  2C 2D  3C 3D
// 0E 0F  1E 1F  2E 2F  3E 3F

//16x16ドットの作業領域

unsigned short  dot_wok[4][16];  //スプライト1枚分ワークエリア
//unsigned short  dot_wok[64];  //スプライト1枚分ワークエリア

void make_1SPRITE( 
	unsigned char  *src
	)
{
	int wok;
	int lp,xx;
	unsigned int bit0_15;
	unsigned short int plane0;
	unsigned short int plane1;
	unsigned short int plane2;
	unsigned short int plane3;
	unsigned char  *ptrsave;

	for(wok=0;wok<16;wok++)
	{
		dot_wok[0][wok] = 0;   // 黒でクリア
		dot_wok[1][wok] = 0;   // 黒でクリア
		dot_wok[2][wok] = 0;   // 黒でクリア
		dot_wok[3][wok] = 0;   // 黒でクリア
	}

	// 16x16ドットを処理
	// BMPは1ドット1バイト
	// pceは16x16で128バイト
	// BMPの1ドット1バイトつまり8ビットで256の値
	// このうち0-15の16を使用する。4ビットです。他のビットは捨てる
	// 16x16ドットx4ビットで 256x4ビット 128x8ビット、128バイトです。

	//--------
	//縦16ラインのループ
	for( lp=0 ; lp<16 ; lp++ )  //yloop
	{
		plane0 = 0;
		plane1 = 0;
		plane2 = 0;
		plane3 = 0;

		bit0_15 = 0x8000;

		//--------
		//横16ドット

		ptrsave = src;

		for( xx=0 ; xx<8 ; xx++ )  //x loop
		{
			wok = *src;  //1byte low
			src++;
			//16色にコンバート
			if(wok & 0x08) plane0 |= bit0_15;
			if(wok & 0x04) plane1 |= bit0_15;
			if(wok & 0x02) plane2 |= bit0_15;
			if(wok & 0x01) plane3 |= bit0_15;
			bit0_15 = bit0_15>>1;
		}

		//bit0_15 = 0x10;
		for( xx=0 ; xx<8 ; xx++ )  //x loop
		{
			wok = *src;  //1byte high
			src++;
			//16色にコンバート
			if(wok & 0x08) plane0 |= bit0_15;
			if(wok & 0x04) plane1 |= bit0_15;
			if(wok & 0x02) plane2 |= bit0_15;
			if(wok & 0x01) plane3 |= bit0_15;
			bit0_15 = bit0_15>>1;
		}
		//(src16byte) = (pce)16dot = 4pln 2word = 4word = 8byte
		dot_wok[0][lp] = plane3; //16bit
		dot_wok[1][lp] = plane2; //16bit
		dot_wok[2][lp] = plane1; //16bit
		dot_wok[3][lp] = plane0; //16bit

		src = ptrsave;   // pop
		src += BmpWidth; // ソース、nextライン

	}
	//縦16dot(横=8byte) = 16x8byte = 128バイト(1スプライト)
}








	//BmpWidth  =  BitmapHedder.biWidth;
	//BmpHeight =  BitmapHedder.biHeight;
	//ReadSize  =  BmpWidth* BmpHeight;

void BMP_PCE(
	int srcX, int srcY,        //bmp内左上xy
	int srcLenX, int srcLenY ) //長さ、xy
{
	int lpx,lpy,i,j;
	unsigned char *srcptr;
	unsigned short int *dstptr;
	unsigned char *ptrsave;
	unsigned char *orgsave;

	srcptr = (unsigned char *)BmpBuff;  // bmp画像バッファ、一バイト1ビット
	srcptr += srcY*BmpWidth + srcX;     // 左上座礁を示すオフセット

	dstptr = (unsigned short *)&BinBuffer[0];              //転送先、PCEの画像フォーマット


	for( lpx=0 ;   ;lpx+=2 )            // 二個ずつ右にずれていく
	{
		orgsave = srcptr;

		if( lpx*16 >= srcLenX )break;

		//ジグザグに縦方向に保存していく
		for( lpy=0 ;  ;lpy++ )
		{
			ptrsave = srcptr;
			if( lpy*16 >= srcLenY )break;

			//横に二個処理
			make_1SPRITE( srcptr );  //short dot_wok[256];
			for( i=0; i<4; i++ ){
				for( j=0; j<16; j++ ){
					*dstptr = dot_wok[i][j];
					dstptr++;
				}
			}

			srcptr += 16;         //右に16ドット移動
			make_1SPRITE( srcptr );  //short dot_wok[256];
			for( i=0; i<4; i++ ){
				for( j=0; j<16; j++ ){
					*dstptr = dot_wok[i][j];
					dstptr++;
				}
			}

			srcptr = ptrsave;
			srcptr += 16*BmpWidth;  //16ライン分処理しました
		}
		srcptr = orgsave;
		srcptr += 32;  //横32ドット分処理しました
	}

}
	//ソースアドレス計算
	//左上から、幅、高さの量を処理する必要があります。
	//yのループでは+16していって、高さに達したら、最初のyにもどり、+32して次の処理
	//xのループでは、+16していって二回処理したら次のyの処理となります。
	//ただ、最後のxのラインでは最終処理のあと、呼び出し元に返ります。



void write_PCE(
	char *binfname,
	int srcX, int srcY,        // bmp内左上xy
	int srcLenX, int srcLenY ) // 長さ、xy

{
	FILE *fp;
	int rts;
	int size,xx;

	fp = fopen( binfname, "wb" );
	if( fp == NULL )
	{
		printf("binFile write error \n");
		exit(1);
	}

	//BmpWidth  =  BitmapHedder.biWidth;
	//BmpHeight =  BitmapHedder.biHeight;
	//ReadSize  =  BmpWidth* BmpHeight;

	xx = srcLenX/32;  // 横32ごとに処理しているため

	size = xx*2 * srcLenY/16;  //スプライト個数

	//1マイのスプライトは128バイト
	fwrite( BinBuffer, 128, size, fp );

	fclose(fp);
}
