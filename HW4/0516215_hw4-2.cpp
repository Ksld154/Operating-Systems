// Student ID: 0516215
// Name      : 林亮穎
// Date      : 2018.12.22
#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0
#define THREAD_NUM 2

int SECTION_NUM = 100;
int imgWidth, imgHeight;
int MEAN_FILTER_SIZE = 9;
int SOBEL_FILTER_SIZE;
int FILTER_SCALE;
int *filter_gx, *filter_gy;

int thread_cnt = 0;    // at the moment,  # of threads that has completed it's computation in that stage.
int global_clk = 0;

struct sync{
    pthread_mutex_t mutex;
    pthread_cond_t  other_thread;
}sync_thread;

sem_t sem1;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputMed_name[5] = {
	"output1.bmp",
	"output2.bmp",
	"output3.bmp",
	"output4.bmp",
	"output5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_mean, *pic_gx, *pic_gy, *pic_sobel,*pic_final;

inline unsigned char RGB2grey(int w, int h);//__attribute__((optimize("-O3")));
inline unsigned char RGB2grey(int w, int h){
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

inline unsigned char MeanFilter(int w, int h);
inline unsigned char MeanFilter(int w, int h){
	int tmp = 0;
	int a, b , window[9],k=0,sum=0;
	int ws = (int)sqrt((float)MEAN_FILTER_SIZE);
	for (register int j = 0; j<ws; j++)
	for (register int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight)continue;

		sum=sum+pic_grey[b*imgWidth + a];
	};

	tmp=sum/MEAN_FILTER_SIZE;
	
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}



inline unsigned char sobelFilter(int w, int h);//__attribute__((optimize("-O3")));
inline unsigned char sobelFilter(int w, int h){
	
    for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			int tmp_x = 0;
			int tmp_y = 0;
			int a, b;
			int ws = (int)sqrt((float)SOBEL_FILTER_SIZE);
			for (int y = 0; y<ws; y++)
			for (int x = 0; x<ws; x++)
			{
				a = i + x - (ws / 2);
				b = j + y - (ws / 2);

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

				tmp_x += filter_gx[y*ws + x] * pic_mean[b*imgWidth + a];
				tmp_y += filter_gy[y*ws + x] * pic_mean[b*imgWidth + a];
			};
			if (tmp_x < 0) tmp_x = 0;
			if (tmp_x > 255) tmp_x = 255;
			if (tmp_y < 0) tmp_y = 0;
			if (tmp_y > 255) tmp_y = 255;

			pic_gx[j*imgWidth + i] = (unsigned char)tmp_x;
			pic_gy[j*imgWidth + i] = (unsigned char)tmp_y;

	    }
		for (int i = 0; i < imgWidth; i++){
			int tmp = 0;
			int w = i, h = j;
			tmp = sqrt(pic_gx[h*imgWidth + w]*pic_gx[h*imgWidth + w] + pic_gy[h*imgWidth + w]*pic_gy[h*imgWidth + w]);
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;
			pic_sobel[j*imgWidth + i] = (unsigned char)tmp;
		}
    }	
}


inline void *MeanRunner(void *arg);//__attribute__((optimize("-O3")));
inline void *SobelRunner(void *arg);//__attribute__((optimize("-O3")));

int main(){
	// read mask file
	FILE* mask;

	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &SOBEL_FILTER_SIZE);

	filter_gx = new int[SOBEL_FILTER_SIZE];
	filter_gy = new int[SOBEL_FILTER_SIZE];

	for (int i = 0; i<SOBEL_FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gx[i]);
	for (int i = 0; i<SOBEL_FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gy[i]);
	fclose(mask);
	
	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k < 5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey  = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_mean  = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_gx    = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_gy    = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_sobel = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
	
		//convert RGB image to grey image
		for (register int j = 0; j<imgHeight; j++) {
			for (register int i = 0; i<imgWidth; i++){
				pic_grey[j*imgWidth + i] = RGB2grey(i, j);
			}
		}


        // if(imgHeight < SECTION_NUM)
        //     SECTION_NUM = 1;
        SECTION_NUM = imgHeight;

        pthread_mutex_init(&sync_thread.mutex, NULL);
        pthread_cond_init(&sync_thread.other_thread, NULL);
		sem_init(&sem1,0,0);

        pthread_t tid[2];
        pthread_create(&tid[0], NULL, MeanRunner,  NULL);
        pthread_create(&tid[1], NULL, SobelRunner, NULL);
        


        pthread_join(tid[0], NULL);        
        pthread_join(tid[1], NULL);  

		//extend the size form WxHx1 to WxHx3
		for (register int j = 0; j<imgHeight; j++) {
			for (register int i = 0; i<imgWidth; i++){
				pic_final[3 * (j*imgWidth + i) + MYRED]   = pic_sobel[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_sobel[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYBLUE]  = pic_sobel[j*imgWidth + i];
			}
		}
        // printf("pic %d is finished\n", k);
		bmpReader->WriteBMP(outputMed_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_mean);
		free(pic_final);
		free(pic_sobel);
		free(pic_gx);
		free(pic_gy);
	}
	return 0;
}

inline void *MeanRunner(void *arg){
    int height_split = imgHeight / SECTION_NUM;
    while(1){
        for(register int k = 0; k < SECTION_NUM; k++){
            int height_limit = (k+1)*height_split;

            // pthread_mutex_lock(&mutexes[k]);
            // printf("Mean start %d\n", k);

            // other section
            if(k != SECTION_NUM-1){
                for(register int j = k*height_split; j < height_limit; j++){
                    for(register int i = 0; i < imgWidth; i++){
                        pic_mean[j*imgWidth + i] = MeanFilter(i, j);			
                    }
                }
            }    

            // last section (deal with border)
            else{
                for(register int j = k*height_split; j < imgHeight; j++){
                    for(register int i = 0; i < imgWidth; i++){
                        pic_mean[j*imgWidth + i] = MeanFilter(i, j);		
                    }
                }
            }

            if(k >= 2)
                sem_post(&sem1);

            // do first two row's meanfilter first
            sem_post(&sem1);
	        sem_post(&sem1);
        }
        pthread_exit(0);
    } 
}

inline void *SobelRunner(void *arg){
    int height_split = imgHeight/SECTION_NUM;

    while(1){
        for(register int k = 0; k < SECTION_NUM; k++){
		    sem_wait(&sem1);
            
            
            if(k != SECTION_NUM-1){
                for(register int j = k*height_split; j < (k+1)*height_split; j++){
                    for(register int i = 0; i < imgWidth; i++){
                        sobelFilter(i, j);	
                    }
                }
            }
            // last section
            else{
                for(register int j = k*height_split; j < imgHeight; j++){
                    for(register int i = 0; i < imgWidth; i++){
                        sobelFilter(i, j);	
                    }
                }
            }
        }
        pthread_exit(0);
    }
}