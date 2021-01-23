#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> /* for open/close .. */
#include <fcntl.h> /* for O_RDONLY */
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h> 
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <wiringPi.h> 
#include "font.h" 

#define FBDEVFILE "/dev/fb2"
#define abc 23 
#define def 27 
#define ghi 22 
#define jkl 21 
#define mno 26 
#define prs 7 
#define tuv 6 
#define wxy 11 
#define qz 24 

#define Left 1 
#define Right 5 
#define DEL 4 
int clickcheck[9] = { 0,0,0,0,0,0,0,0,0 };
int alphanum[9] = { 0,0,0,0,0,0,0,0,0 };
int size;
int deletech;


void set_btns()
{
	pinMode(abc, INPUT);   pullUpDnControl(abc, PUD_UP);
	pinMode(def, INPUT);   pullUpDnControl(def, PUD_UP);
	pinMode(ghi, INPUT);   pullUpDnControl(ghi, PUD_UP);
	pinMode(jkl, INPUT);   pullUpDnControl(jkl, PUD_UP);
	pinMode(mno, INPUT);   pullUpDnControl(mno, PUD_UP);
	pinMode(prs, INPUT);   pullUpDnControl(prs, PUD_UP);
	pinMode(tuv, INPUT);   pullUpDnControl(tuv, PUD_UP);
	pinMode(wxy, INPUT);   pullUpDnControl(wxy, PUD_UP);
	pinMode(qz, INPUT);      pullUpDnControl(qz, PUD_UP);
	pinMode(Left, INPUT);   pullUpDnControl(Left, PUD_UP);
	pinMode(Right, INPUT);   pullUpDnControl(Right, PUD_UP);
	pinMode(DEL, INPUT);   pullUpDnControl(DEL, PUD_UP);
}
void kursormaker(int* col, int* row, unsigned short* pfbdata, struct fb_var_screeninfo fbvar)
{
	int k = 0;
	int offset;
	int pixel = 65535;
	for (k = 22 + (*col) * 28; k < 46 + (*col) * 28; k++) {
		offset = (34 + (*row) * 28) * fbvar.xres + k;
		*(pfbdata + offset) = pixel;
	}
}

int alpaclick(int i, const unsigned char* one, const unsigned char* two, const unsigned char* three, char* res) {
	int removecheck = 0;
	int p = 0;
	int q = 0;
	int past[9] = { 0,0,0,0,0,0,0,0,0 };
	for (p = 0; p < 9; p++)
		if (alphanum[p] || clickcheck[p]) //  ��  ��   ��  ��  ��   ��  ��  ��     ��    �� 
		{
			past[p] = 1; clickcheck[p] = 0;
		}
	for (q = 0; q < 9; q++) {
		if (past[q] == 1 && q == i) //  ��  ��   ��  ��   ��  ��  ��  >  ��  ��  ��  ��  ��   ��  ��  ��
			removecheck = 1;//  ��   ��  ��
		else if (past[q] == 1 && q != i) {//
			removecheck = 0;
			alphanum[q] = 0;
		}
	}
	if (alphanum[i] == 0) {//  ��  ��  ��  ��  ��
		memcpy(res, one, 576);//  ��  ��   ��  �� //
		alphanum[i] = 1;//  ��  ��  ��  ��   ü  ũ

	}
	else if (alphanum[i] == 1) {
		memcpy(res, two, 576);
		alphanum[i] = 2;

	}
	else if (alphanum[i] == 2) {
		memcpy(res, three, 576);
		alphanum[i] = 0;
		clickcheck[i] = 1;

	}
	if (removecheck) return 0;
	else return 1;
}

void removealp(int* row, int* col, unsigned short* pfbdata, struct fb_var_screeninfo fbvar)
{
	int j = 0;
	int k = 0;
	int offset, pixel = 0;
	if ((*col) == 0 && (*row) != 0) {
		(*col) = 9; (*row)--;
	}
	else (*col)--;
	for (j = 10 + (*row) * 28; j < 34 + (*row) * 28; j++) {
		for (k = 22 + (*col) * 28; k < 46 + (*col) * 28; k++) {
			offset = j * fbvar.xres + k;
			*(pfbdata + offset) = pixel;
		}
	}
	size--;
}
void del_alp(int* col, int* row, unsigned short* pfbdata, struct fb_var_screeninfo fbvar)
{
	int offset;
	int pixel = 0;
	int j = 0;
	int k = 0;
	memset(alphanum, 0, 9);//0  ��  ��   ��  ��  ȭ
	memset(clickcheck, 0, 9);
	for (j = 10 + (*row) * 28; j < 34 + (*row) * 28; j++) {
		for (k = 22 + (*col) * 28; k < 46 + (*col) * 28; k++) {
			offset = j * fbvar.xres + k;
			*(pfbdata + offset) = pixel;
		}
	}
	kursormaker(col, row, pfbdata, fbvar); //  Ŀ  ��  ��   ��  ��  ��  ��        
	deletech = 1;
	size--;

}
void alphaselect(int i, char res[], int* row, int* col, unsigned short* pfbdata, struct fb_var_screeninfo fbvar)
{
	int flag = 0;
	switch (i) {
	case 0:
		flag = alpaclick(i, dot, Q, Z, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 1:
		flag = alpaclick(i, A, B, C, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 2:
		flag = alpaclick(i, D, E, F, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 3:
		flag = alpaclick(i, G, H, I, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 4:
		flag = alpaclick(i, J, K, L, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 5:
		flag = alpaclick(i, M, N, O, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 6:
		flag = alpaclick(i, P, R, S, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 7:
		flag = alpaclick(i, T, U, V, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	case 8:
		flag = alpaclick(i, W, X, Y, res);
		if (flag != 1) removealp(row, col, pfbdata, fbvar);
		break;
	}
}


void kursormover(int way, int* col, int* row, unsigned short* pfbdata, struct fb_var_screeninfo fbvar)  //  Ŀ  ��    ��  ��  ��   ��  ��
{
	int offset;
	int pixel = 65535;
	int k = 0;
	for (k = 22 + (*col) * 28; k < 46 + (*col) * 28; k++) {//�����
		offset = k + (34 + (*row) * 28) * fbvar.xres;
		*(pfbdata + offset) = pixel - 65535;
	}

	if (way == 1) {//  ��  ��  ��               
		memset(alphanum, 0, 9);//  ��  ư  ��  ��  ��  �� 0  ��  ��                
		memset(clickcheck, 0, 9);//  ��  ư   ��  ��  ��  �� 0  ��  ��   ��               
		if ((*col) == 9) { (*col) = 0; (*row)++; }
		else  (*col)++;
		kursormaker(col, row, pfbdata, fbvar);
	}
	else if (way == 2) {//  ��  ��  ��  ��   ��  ��  ��  ��
		memset(alphanum, 0, 9);//  ��  ư  ��  ��  ��  �� 0  ��  ��                  
		memset(clickcheck, 0, 9);//  ��  ư   ��  ��  ��  �� 0  ��  ��   ��               
		if ((*col) == 0 && (*row) != 0) {//  ��  ��  °  ��  ��   ��                           
			(*col) = 9; (*row)--; //  ��  ��  ��   ��  ��  ��  ��   
		}
		else if (((*col) == 0 && (*row) == 0)) {
			(*col) = 0; (*row) = 0;
		}
		else (*col)--;//  ��  ��  ��   ��  ��   ��  ��  ��  ��
		kursormaker(col, row, pfbdata, fbvar);
	}

}
void kursorcleaner(int* col, int* row, unsigned short* pfbdata, struct fb_var_screeninfo fbvar)//
{
	int offset;
	int pixel = 0;
	int rmcol = 0, rmrow = 0;
	int k = 0;
	if ((*col) == 0 && (*row) != 0) {
		rmcol = 9; rmrow = (*row) - 1;
	}
	else {
		rmcol = (*col) - 1;
		rmrow = (*row);
	}
	for (k = 22 + rmcol * 28; k < 46 + rmcol * 28; k++) {
		offset = k + (34 + rmrow * 28) * fbvar.xres;
		*(pfbdata + offset) = pixel;
	}
}





int main(int argc, char** argv)
{
	int button[] = { qz, abc, def, ghi, jkl, mno, prs, tuv, wxy, Left, DEL,Right };
	wiringPiSetup();
	set_btns();

	int fbfd;
	int ret;
	struct fb_var_screeninfo fbvar;
	unsigned short pixel;
	unsigned short* pfbdata;

	fbfd = open(FBDEVFILE, O_RDWR);
	if (fbfd < 0) {
		perror("fbdev open");
		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if (ret < 0) {
		perror("fbdev ioctl");
		exit(1);
	}

	if (fbvar.bits_per_pixel != 16) {
		fprintf(stderr, "bpp is not 16\n");
		exit(1);
	}

	pfbdata = (unsigned short*)mmap(0, fbvar.xres * fbvar.yres * 16 / 8, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((unsigned)pfbdata == (unsigned)-1) {
		perror("fbdev mmap");
		exit(1);
	}
	int i = 0;
	int j = 0;
	int k = 0;
	int flag = 0, x = 1, offset;
	int rpixel = 65535, col = 0, row = 0;
	kursormaker(&col, &row, pfbdata, fbvar);
	while (1) {
		x = 1;
		for (i = 0; i < 12; i++) {
			if ((x &= digitalRead(button[i])) == LOW && !flag && i
				< 9) {
				//if ( row == 7 && col==9){
				   //  size=0;
					//  row=0; col=0;                         
	//kursormaker(&col, &row, pfbdata, fbvar);
	//continue;}; 
				int count = 0;


				char alp[576];

				kursorcleaner(&col, &row, pfbdata, fbvar);
				alphaselect(i, alp, &row, &col, pfbdata, fbvar);
				if (deletech) {
					for (j = 10 + row * 28; j < 34 + row * 28; j++) {
						for (k = 22 + col * 28; k < 46 + col * 28; k++) {
							if (alp[count++]) {
								offset = j * fbvar.xres + k;
								*(pfbdata + offset) = rpixel;
							}
						}
					}
					kursormaker(&col, &row, pfbdata, fbvar);
					deletech = 0;
					flag = 1;

					continue;
				}
				for (j = 10 + row * 28; j < 34 + row * 28; j++) {
					for (k = 22 + col * 28; k < 46 + col * 28; k++) {
						if (alp[count++]) {
							offset = j * fbvar.xres + k;
							*(pfbdata + offset) = rpixel;
						}
					}
				}
				flag = 1;
				kursormaker(&col, &row, pfbdata, fbvar);
				if (col == 9 && row != 7) { col = 0; row++; }
				else if (col == 9 && row == 7) { col = 0; row = 0; }
				else  col++;
				size++;
			}
			else if ((x &= digitalRead(Left)) == LOW && !flag) {      
				int l = 2;
				kursormover(l, &col, &row, pfbdata, fbvar);
				flag = 1;
			}
			else if ((x &= digitalRead(DEL)) == LOW && !flag) {
				del_alp(&col, &row, pfbdata, fbvar);
				flag = 1;
			}
			else if ((x &= digitalRead(Right)) == LOW && !flag && size <= 80) {
				int r = 1;
				if (row == 7 && col > 8) {
					row = 0;
					col = 0;
				}
				kursormover(r, &col, &row, pfbdata, fbvar);
				flag = 1;
			}
			else if ((x &= digitalRead(Right)) == LOW && !flag && size > 80) {
				col = 0; row = 0;
				kursormaker(&col, &row, pfbdata, fbvar);
				flag = 1;
			}
			else if ((x &= digitalRead(Right)) == LOW && !flag && row == 7 && col > 9) {
				col = 0; row = 0;
				kursormaker(&col, &row, pfbdata, fbvar);
				flag = 1;
				size = 0;
			}
			delay(20);
			if (x && flag)flag = 0;
		}

	}
	munmap(pfbdata, fbvar.xres * fbvar.yres * 16 / 8);
	close(fbfd);
	return 0;
}