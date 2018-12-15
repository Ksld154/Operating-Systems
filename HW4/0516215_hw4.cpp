// Student ID: 0516215
// Name      : 林亮穎
// Date      : 2018.12.
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
#define SECTION_NUM 200

int imgWidth, imgHeight;
int MEAN_FILTER_SIZE = 9;
int SOBEL_FILTER_SIZE;
int FILTER_SCALE;
int *filter_gx, *filter_gy;

int thread_cnt = 0;    // at the moment,  # of threads that has completed it's computation in that stage.
int global_clk = 0;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutexes[SECTION_NUM];
pthread_mutex_t mutex_sync[SECTION_NUM];
pthread_cond_t  synch[SECTION_NUM];

struct sync{
    pthread_mutex_t mutex;
    pthread_cond_t  other_thread;
}sync_thread;

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

// int height_split = imgHeight / THREAD_NUM;

unsigned char *pic_in, *pic_grey, *pic_mean, *pic_gx, *pic_gy, *pic_sobel,*pic_final;

unsigned char RGB2grey(int w, int h)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char MeanFilter(int w, int h)
{
	int tmp = 0;
	int a, b , window[9],k=0,sum=0;
	int ws = (int)sqrt((float)MEAN_FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
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

unsigned char gx_sobelFilter(int w, int h)
{
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)SOBEL_FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_gx[j*ws + i] * pic_mean[b*imgWidth + a];
	};
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char gy_sobelFilter(int w, int h)
{
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)SOBEL_FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_gy[j*ws + i] * pic_mean[b*imgWidth + a];
	};
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char sobelFilter(int w, int h)
{
	int tmp = 0;
	tmp = sqrt(pic_gx[h*imgWidth + w]*pic_gx[h*imgWidth + w] + pic_gy[h*imgWidth + w]*pic_gy[h*imgWidth + w]);
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

void *SingleMean(void *arg){
    long int current_section = (long int)arg;

    int height_split = imgHeight / SECTION_NUM;
    int height_limit = (current_section+1)*height_split;

    // if not the last section
    if(current_section != SECTION_NUM-1){
        for(int i = current_section*height_split; i < height_limit; i++){
            for(int j = 0; j < imgWidth; j++){
                pic_mean[i*imgWidth + j] = MeanFilter(j, i);		
            }
        }
    }

    // last section
    else{
        for(int i = current_section*height_split; i < imgHeight; i++){
            for(int j = 0; j < imgWidth; j++){
                pic_mean[i*imgWidth + j] = MeanFilter(j, i);		
            }
        }
    }
    pthread_exit(0);
}

void *SingleSobel(void *arg){
    long int current_section = (long int)arg;
    // cout << current_section << endl;

    int height_split = imgHeight / SECTION_NUM;
    if(current_section != SECTION_NUM-1){
        for(int i = current_section*height_split; i < (current_section+1)*height_split; i++){
            for(int j = 0; j < imgWidth; j++){
                pic_gx[i*imgWidth + j] = gx_sobelFilter(j, i);
                pic_gy[i*imgWidth + j] = gy_sobelFilter(j, i);
                pic_sobel[i*imgWidth + j] = sobelFilter(j, i);
            }
        }
    }
    else{
        for(int i = current_section*height_split; i < imgHeight; i++){
            for(int j = 0; j < imgWidth; j++){
                pic_gx[i*imgWidth + j] = gx_sobelFilter(j, i);
                pic_gy[i*imgWidth + j] = gy_sobelFilter(j, i);
                pic_sobel[i*imgWidth + j] = sobelFilter(j, i);
            }
        }
    }
    pthread_exit(0);
}

void *MeanRunner(void *arg);
void *SobelRunner(void *arg);

int main()
{
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
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_grey[j*imgWidth + i] = RGB2grey(i, j);
			}
		}


        // pthread_mutex_init(&mutex1, NULL);
        // pthread_mutex_init(&mutex2, NULL);

        
        for(int i = 0; i < SECTION_NUM; i++){
            pthread_mutex_init(&mutexes[i], NULL);
            pthread_mutex_init(&mutex_sync[i], NULL);
            pthread_mutex_init(&sync_thread.mutex, NULL);
            pthread_cond_init(&sync_thread.other_thread, NULL);
            pthread_cond_init(&synch[i], NULL);

        }
        


        pthread_t tid[2];
        pthread_create(&tid[0], NULL, MeanRunner,  NULL);
        pthread_create(&tid[1], NULL, SobelRunner, NULL);
        
        global_clk = 0;
        int finish_cnt = 0;
        while(finish_cnt <= SECTION_NUM){

            if(thread_cnt == THREAD_NUM){
                global_clk++;
                finish_cnt++;
                thread_cnt = 0;
                // printf("finish_cnt: %d\n", finish_cnt);
                // printf("clk: %d\n", global_clk);
                while(1){
                    if(pthread_mutex_trylock(&sync_thread.mutex) == 0){
                        pthread_mutex_unlock(&sync_thread.mutex);
                        pthread_cond_broadcast(&sync_thread.other_thread);
                        break;
                    }
                }        
            }
            // }
        }

        pthread_join(tid[0], NULL);        
        pthread_join(tid[1], NULL);  

		//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
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

void *MeanRunner(void *arg){
    int height_split = imgHeight / SECTION_NUM;
    while(1){
        if(global_clk == SECTION_NUM){
            pthread_mutex_lock(&sync_thread.mutex);
            thread_cnt++;   //record the number of users that had finish stage1
            // printf("t1: %d\n", thread_cnt);
            pthread_cond_wait(&sync_thread.other_thread, &sync_thread.mutex);
            pthread_mutex_unlock(&sync_thread.mutex);
            pthread_exit(0);    

        }
        else{
            for(int k = 0; k < SECTION_NUM; k++){
                int height_limit = (k+1)*height_split;

                // pthread_mutex_lock(&mutexes[k]);
                
                // printf("Mean start %d\n", k);

                // other section
                if(k != SECTION_NUM-1){

                    for(int j = k*height_split; j < height_limit; j++){
                        for(int i = 0; i < imgWidth; i++){
                            pic_mean[j*imgWidth + i] = MeanFilter(i, j);			
                        }
                    }
                }    

                // last section (deal with border)
                else{
                    for(int j = k*height_split; j < imgHeight; j++){
                        for(int i = 0; i < imgWidth; i++){
                            pic_mean[j*imgWidth + i] = MeanFilter(i, j);		
                        }
                    }
                }


                // printf("Mean finish %d\n", k);
                // pthread_mutex_unlock(&mutexes[k]);
                // pthread_cond_signal(&synch[k]);


                pthread_mutex_lock(&sync_thread.mutex);
                thread_cnt++;   //record the number of users that had finish stage1
                // printf("t1: %d\n", thread_cnt);
                pthread_cond_wait(&sync_thread.other_thread, &sync_thread.mutex);
                pthread_mutex_unlock(&sync_thread.mutex);
            }
        }
    }
    // pthread_exit(0);    
}

void *SobelRunner(void *arg){
    int height_split = imgHeight/SECTION_NUM;
    while(1){
        if(global_clk == 0){
            pthread_mutex_lock(&sync_thread.mutex);
            thread_cnt++;   //record the number of users that had finish stage1
            // printf("t2: %d\n", thread_cnt);
            pthread_cond_wait(&sync_thread.other_thread, &sync_thread.mutex);
            pthread_mutex_unlock(&sync_thread.mutex);
        }
        else{
            for(int k = 0; k < SECTION_NUM; k++){

                // pthread_mutex_lock(&mutexes[k]);
                // pthread_cond_wait(&synch[k], &mutexes[k]);
                
                // pthread_mutex_lock(&mutexes[k]);
                // printf("Sobel start %d\n", k);
                
                if(k != SECTION_NUM){
                    for(int j = k*height_split; j < (k+1)*height_split; j++){
                        for(int i = 0; i < imgWidth; i++){
                            pic_gx[j*imgWidth + i] = gx_sobelFilter(i, j);
                            pic_gy[j*imgWidth + i] = gy_sobelFilter(i, j);
                            pic_sobel[j*imgWidth + i] = sobelFilter(i, j);	
                        }
                    }
                }
                // last section
                else{
                    for(int j = k*height_split; j < imgHeight; j++){
                        for(int i = 0; i < imgWidth; i++){
                            pic_gx[j*imgWidth + i] = gx_sobelFilter(i, j);
                            pic_gy[j*imgWidth + i] = gy_sobelFilter(i, j);
                            pic_sobel[j*imgWidth + i] = sobelFilter(i, j);	
                        }
                    }
                }

                // printf("Sobel finish %d\n", k);
                // pthread_mutex_unlock(&mutexes[k]);

                pthread_mutex_lock(&sync_thread.mutex);
                thread_cnt++;   //record the number of users that had finish stage1
                // printf("t2: %d\n", thread_cnt);
                pthread_cond_wait(&sync_thread.other_thread, &sync_thread.mutex);
                pthread_mutex_unlock(&sync_thread.mutex);
            }
            pthread_exit(0);    
        }
    }
}