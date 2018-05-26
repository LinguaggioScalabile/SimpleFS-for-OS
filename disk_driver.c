#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> 
#include <fcntl.h>


#include "disk_driver.h"

#define ERROR -1

int fp=-1;
int PageSize; 

static DiskDriver* createMMAP(void* addr,size_t size, int mfd){
	
	
	int protections=PROT_READ | PROT_WRITE;
	int flags = MAP_SHARED | MAP_ANONYMOUS;
	off_t offset=0;

	DiskDriver* state = mmap(addr,size,protections,
		flags, mfd, offset);

	if(state==MAP_FAILED){
		perror("errore in mmap");
		exit(1);
	}

	return state;
}

static void deleteMMAP(void* addr){

	if (ERROR == munmap(addr,sizeof(DiskDriver) )){
		perror("errore in unmap di deleteMMAP");
		exit(1);
	}
}

				//LAVORO PER SINGOLO FILE (un file creato-scritto/letto-chiuso)TODO DA AMPLIARE

// opens the file (creating it if necessary_
// allocates the necessary space on the disk
// calculates how big the bitmap should be

// if the file was new
// compiles a disk header, and fills in the bitmap of appropriate size
// with all 0 (to denote the free space);
void DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks){

	// 2 diversi casi: il file è creato / il file è aperto
	char file_existance=0;
	if( access( filename, F_OK ) != -1 ) {
    // file exists
		file_existance=1;
	} else {
    // file doesn't exist
		file_existance=0;
	}

	PageSize= (int)sysconf(_SC_PAGESIZE);	//serve per la mmap
    if ( PageSize < 0) {
    	perror("sysconf() error");
    }


	//TODO possibile evitare controllo interno su esistenza file in fopen
	fp=open(filename, O_RDWR | O_CREAT|O_APPEND,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fp==-1){
		perror("errore in apertura file");
		exit(1);
	}

	DiskHeader* dh=(DiskHeader*) malloc(sizeof(DiskHeader));
	DiskDriver* dd=(DiskDriver*) malloc(sizeof(DiskDriver));
	DiskDriver* dd_ptr;
	if(!file_existance){//if the file was new...
		//init di diskHeader e diskdriver 
	  dh->bitmap_blocks=0;   // numero di blocchi mappati sulla bitmap, per ora 0
	  dh->bitmap_entries=1+num_blocks/8;  //  grandezza in byte della bitmap: ogni blocco= 1/8 di byte
	  // Per l'inodemap
	  dh->inodemap_blocks=-1;  // Numero di inode nella inodemap
	  dh->inodemap_entries=-1;   // Numero di byte necessari per memorizzare la inodemap
	  
	  dh->dataFree_blocks=-1;     // free blocks of data, dim della bitmap in blocchi - num_blocks ???
	  dh->dataFirst_free_block=-1;// first block index data 

	  dh->inodeFree_blocks=-1;     // free blocks of inode <- (necessario ????)
	  dh->inodeFirst_free_block=-1;// first block index
		
	  dd->header=dh; // mmapped
	  BitMap bm= BitMap_init();	//tutte a 0

  	  dd->bitmap_data_values=bm.entries;  // mmapped (data bitmap)
  	  dd->bitmap_inode_values=NULL;  //mmaped <- (necessario introdurlo???)  
  	  dd->fd=fp; // for us

  	  dd_ptr = createMMAP(NULL,sizeof(DiskDriver),-1);	// dubbio su mmap di diskheader
  	  
	}

	//TODO FILLARE LA bitmap e altro


	if( close(fp)){
		perror("errore in fclose");
		exit(1);
	}

	disk=dd_ptr;	//side effect e mi do 
	//dd_ptr non freeable

}

// reads the block in position block_num
// returns -1 if the block is free accrding to the bitmap
// 0 otherwise
int DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num) {
	/*
	void* blockRead=(void*) malloc(BLOCK_SIZE);
	fseek(fp, block_num*BLOCK_SIZE, SEEK_SET);
	fread(blockRead, BLOCK_SIZE, 1, fp);
	int j;
	for (j=0; j<BLOCK_SIZE; j++) {
		memcpy(dest, blockRead, BLOCK_SIZE);
	}
	free(blockRead);
	return 0;
	*/
	//TODO eseguire il controllo per vedere se il block è free secondo la bitmap
}

// writes a block in position block_num, and alters the bitmap accordingly
// returns -1 if operation not possible
int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num) {
	/*
	void* blockWrite = (void*) malloc(BLOCK_SIZE);
	fseek(fp, block_num*BLOCK_SIZE, SEEK_SET);
	memcpy(blockWrite, src, BLOCK_SIZE);
	fwrite(blockWrite, BLOCK_SIZE, 1, fp);
	fflush(fp);
	free(blockWrite);
	return 0;
	*/
	//TODO sempre il problema della bitmap non ancora implementata

}

// frees a block in position block_num, and alters the bitmap accordingly
// returns -1 if operation not possible
int DiskDriver_freeBlock(DiskDriver* disk, int block_num);

// returns the first free blockin the disk from position (checking the bitmap)
int DiskDriver_getFreeBlock(DiskDriver* disk, int start);

// writes the data (flushing the mmaps)
int DiskDriver_flush(DiskDriver* disk);

