
int GrWidth;  //���o�͂��悤�Ƃ���摜�̕�


#define BUFFSIZE 0x10000

unsigned char BinBuffer[BUFFSIZE];
int FileSize=0;

//pce�̉摜�̓f�[�^�����[�h�A�N�Z�X�̂���
unsigned short *rom_ptr;

//����������������������������������������������������������������
//�o�C�i���ǂݍ��݊֐�
//����������������������������������������������������������������

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
//	rom_ptr     = (unsigned short *)&romdata[rom_datacnt];     //���[�h�|�C���^
//
//	return(0);
//}


//����������������������������������������������������������������
// �X�v���C�g�̕\��
//rom�\���A�h���X����0x2000�o�C�g�i8x8�X�v���C�g���j�\��
//����������������������������������������������������������������
//�\���J�n������W
#define DISP_RU_X 365
#define DISP_RU_Y 40

//�\���ʒu�e�[�u��
//2x4 8�X�v���C�g��xy�ʒu�A1�`�����N��
int sprt_chank[8][2]={
	{  0,  0},		{ 16,  0},
	{  0, 16},		{ 16, 16},
	{  0, 32},		{ 16, 32},
	{  0, 48},		{ 16, 48}};


//�X�v���C�g�����̏��ԂɂȂ�悤�ɕ\��
// 00 01  10 11  20 21  30 31
// 02 03  12 13  22 23  32 33
// 04 05  14 15  24 25  34 35
// 06 07  16 17  26 27  36 37
// 08 09  18 19  28 29  38 39
// 0A 0B  1A 1B  2A 2B  3A 3B
// 0C 0D  1C 1D  2C 2D  3C 3D
// 0E 0F  1E 1F  2E 2F  3E 3F

//16x16�h�b�g�̍�Ɨ̈�

unsigned short  dot_wok[4][16];  //�X�v���C�g1�������[�N�G���A
//unsigned short  dot_wok[64];  //�X�v���C�g1�������[�N�G���A

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
		dot_wok[0][wok] = 0;   // ���ŃN���A
		dot_wok[1][wok] = 0;   // ���ŃN���A
		dot_wok[2][wok] = 0;   // ���ŃN���A
		dot_wok[3][wok] = 0;   // ���ŃN���A
	}

	// 16x16�h�b�g������
	// BMP��1�h�b�g1�o�C�g
	// pce��16x16��128�o�C�g
	// BMP��1�h�b�g1�o�C�g�܂�8�r�b�g��256�̒l
	// ���̂���0-15��16���g�p����B4�r�b�g�ł��B���̃r�b�g�͎̂Ă�
	// 16x16�h�b�gx4�r�b�g�� 256x4�r�b�g 128x8�r�b�g�A128�o�C�g�ł��B

	//--------
	//�c16���C���̃��[�v
	for( lp=0 ; lp<16 ; lp++ )  //yloop
	{
		plane0 = 0;
		plane1 = 0;
		plane2 = 0;
		plane3 = 0;

		bit0_15 = 0x8000;

		//--------
		//��16�h�b�g

		ptrsave = src;

		for( xx=0 ; xx<8 ; xx++ )  //x loop
		{
			wok = *src;  //1byte low
			src++;
			//16�F�ɃR���o�[�g
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
			//16�F�ɃR���o�[�g
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
		src += BmpWidth; // �\�[�X�Anext���C��

	}
	//�c16dot(��=8byte) = 16x8byte = 128�o�C�g(1�X�v���C�g)
}








	//BmpWidth  =  BitmapHedder.biWidth;
	//BmpHeight =  BitmapHedder.biHeight;
	//ReadSize  =  BmpWidth* BmpHeight;

void BMP_PCE(
	int srcX, int srcY,        //bmp������xy
	int srcLenX, int srcLenY ) //�����Axy
{
	int lpx,lpy,i,j;
	unsigned char *srcptr;
	unsigned short int *dstptr;
	unsigned char *ptrsave;
	unsigned char *orgsave;

	srcptr = (unsigned char *)BmpBuff;  // bmp�摜�o�b�t�@�A��o�C�g1�r�b�g
	srcptr += srcY*BmpWidth + srcX;     // ������ʂ������I�t�Z�b�g

	dstptr = (unsigned short *)&BinBuffer[0];              //�]����APCE�̉摜�t�H�[�}�b�g


	for( lpx=0 ;   ;lpx+=2 )            // ����E�ɂ���Ă���
	{
		orgsave = srcptr;

		if( lpx*16 >= srcLenX )break;

		//�W�O�U�O�ɏc�����ɕۑ����Ă���
		for( lpy=0 ;  ;lpy++ )
		{
			ptrsave = srcptr;
			if( lpy*16 >= srcLenY )break;

			//���ɓ����
			make_1SPRITE( srcptr );  //short dot_wok[256];
			for( i=0; i<4; i++ ){
				for( j=0; j<16; j++ ){
					*dstptr = dot_wok[i][j];
					dstptr++;
				}
			}

			srcptr += 16;         //�E��16�h�b�g�ړ�
			make_1SPRITE( srcptr );  //short dot_wok[256];
			for( i=0; i<4; i++ ){
				for( j=0; j<16; j++ ){
					*dstptr = dot_wok[i][j];
					dstptr++;
				}
			}

			srcptr = ptrsave;
			srcptr += 16*BmpWidth;  //16���C�����������܂���
		}
		srcptr = orgsave;
		srcptr += 32;  //��32�h�b�g���������܂���
	}

}
	//�\�[�X�A�h���X�v�Z
	//���ォ��A���A�����̗ʂ���������K�v������܂��B
	//y�̃��[�v�ł�+16���Ă����āA�����ɒB������A�ŏ���y�ɂ��ǂ�A+32���Ď��̏���
	//x�̃��[�v�ł́A+16���Ă����ē�񏈗������玟��y�̏����ƂȂ�܂��B
	//�����A�Ō��x�̃��C���ł͍ŏI�����̂��ƁA�Ăяo�����ɕԂ�܂��B



void write_PCE(
	char *binfname,
	int srcX, int srcY,        // bmp������xy
	int srcLenX, int srcLenY ) // �����Axy

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

	xx = srcLenX/32;  // ��32���Ƃɏ������Ă��邽��

	size = xx*2 * srcLenY/16;  //�X�v���C�g��

	//1�}�C�̃X�v���C�g��128�o�C�g
	fwrite( BinBuffer, 128, size, fp );

	fclose(fp);
}
