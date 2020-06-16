/**
 * Team Members:
 * - Will Croteau
 * crote010@umn.edu
 * - Renda Zhang
 * zhan4067@umn.edu
 *
 * Project 2, CSci5103
 **/

/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// make it global for using inside handler
struct disk *disk;
char *virtmem;
char *physmem;
int npages, nframes;
// counter for page faults, disk reads and disk writes
int page_fault = 0;
int disk_reads = 0;
int disk_writes = 0;
// replacement policy
char *policy;

// The LEF page evictions array
int *page_evics;

// mapping of frame to page and bits
typedef struct {
	int page;
	int bits;
} frame_entry;
frame_entry* frame_table = NULL;

// global variable for FIFO array
int *fifo_array;
int front = 0;
int back = 0;


// function to remove the frame
void evict_page(struct page_table *pt, int frame)
{
	// write to the disk if WRITE bit is set
	if(frame_table[frame].bits & PROT_WRITE)
	{
		disk_write(disk, frame_table[frame].page, &physmem[frame*PAGE_SIZE]);
		disk_writes++;
	}
	// update the page table and frame table
	page_table_set_entry(pt, frame_table[frame].page, frame, 0);
	frame_table[frame].bits = 0;
}

// custom replacement policy - least evictions first
void custom_handler( struct page_table *pt, int page )
{
	int bits, frame;
	// set it to read
	bits = PROT_READ;

	// check for a free frame by looping through all the frame
	frame = -1;
  int i;
	for(i = 0; i < nframes; i++) {
		if(frame_table[i].bits == 0) {
			frame = i;
			break;
		}
	}

  // if no free frame
	if(frame < 0) {
		// find the page that has had the least evictions
    int min = INT_MAX;
    int i;
    for(i=0; i<nframes; i++){
      if(page_evics[frame_table[i].page] < min){
        frame = i;
        min = page_evics[frame_table[i].page];
      }
    }
    // evict page
    page_evics[frame_table[frame].page]++;
    evict_page(pt, frame);
	}

	// read from disk
	disk_read(disk, page, &physmem[frame*PAGE_SIZE]);
	disk_reads++;

	// update page table and frame table
	page_table_set_entry(pt, page, frame, bits);
	frame_table[frame].page = page;
	frame_table[frame].bits = bits;
}

// random replacement policy
void rand_handler( struct page_table *pt, int page )
{
	int bits, frame;
	// set it to read
	bits = PROT_READ;

	// check for a free frame by looping through all the frame
	frame = -1;
  int i;
	for(i = 0; i < nframes; i++) {
		if(frame_table[i].bits == 0) {
			frame = i;
			break;
		}
	}

	// if no free frame
	if(frame < 0) {
		// radomly select a frame and remove it
		frame = (int) lrand48() % nframes;
		evict_page(pt, frame);
	}

	// read from disk
	disk_read(disk, page, &physmem[frame*PAGE_SIZE]);
	disk_reads++;

	// update page table and frame table
	page_table_set_entry(pt, page, frame, bits);
	frame_table[frame].page = page;
	frame_table[frame].bits = bits;
}

// fifo replacement policy
void fifo_handler( struct page_table *pt, int page )
{
	int bits, frame;
	// set it to read
	bits = PROT_READ;

	// check for a free frame by looping through all the frame
	frame = -1;
  int i;
	for(i = 0; i < nframes; i++) {
		if(frame_table[i].bits == 0) {
			frame = i;
			break;
		}
	}

	// if no free frame
	if(frame < 0) {
		if(front == back) {
			// get frame from the front and update the front
			frame = fifo_array[front];
			evict_page(pt, frame);
			front = (front + 1) % nframes;
		} else {
			printf("error in fifo handler\n");
			return;
		}
	}

	// update the FIFO array and the back
	fifo_array[back] = frame;
	back = (back + 1) % nframes;

	// read from disk
	disk_read(disk, page, &physmem[frame*PAGE_SIZE]);
	disk_reads++;

	// update page table and frame table
	page_table_set_entry(pt, page, frame, bits);
	frame_table[frame].page = page;
	frame_table[frame].bits = bits;
}

void page_fault_handler( struct page_table *pt, int page )
{
	// count the page fault
	page_fault = page_fault + 1;

  // get the frame and bits that cause page fault
	int bits, frame;
	page_table_get_entry(pt, page, &frame, &bits);

  // check if the bits has been set
	if(!bits) {
		// use different policy based on user input
		if(!strcmp(policy, "rand")) {
			rand_handler(pt, page);
		}
		if(!strcmp(policy, "fifo")) {
			fifo_handler(pt, page);
		}
		if(!strcmp(policy, "custom")) {
			custom_handler(pt, page);
		}
	}
	// the bits is set with only read, add write to it
	else if(!(bits ^ PROT_READ)) {
		bits = PROT_READ | PROT_WRITE;
		// update page table and frame table
		page_table_set_entry(pt, page, frame, bits);
		frame_table[frame].page = page;
		frame_table[frame].bits = bits;
	}
	// error if come here
	else {
		printf("Error in page fault handler.");
		return;
	}
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
		return 1;
	}

	npages = atoi(argv[1]);
	nframes = atoi(argv[2]);
	const char *program = argv[4];
	policy = argv[3];

	disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	virtmem = page_table_get_virtmem(pt);

	physmem = page_table_get_physmem(pt);

	// check for incorrect policy type
	if(!((strcmp(policy, "rand") == 0) || (strcmp(policy, "fifo") == 0)
	    || (strcmp(policy, "custom") == 0))) {
		fprintf(stderr,"unknown policy: %s\n", policy);
		return 1;
	}

	// allocate memory for frame table
	frame_table = (frame_entry *)malloc(nframes*sizeof(frame_entry));
	if(frame_table == NULL) {
		printf("Error in malloc space for frame_table\n");
		return 1;
	}

	// allocate memory for the FIFO array
	fifo_array = (int *)malloc(nframes * sizeof(int));

	// allocate and initialize the LRU array
	page_evics = (int *)calloc(npages, sizeof(int));

	if(!strcmp(program,"sort")) {
		sort_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"scan")) {
		scan_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"focus")) {
		focus_program(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"unknown program: %s\n",argv[4]);
		return 1;
	}

	// clean up the process
	page_table_delete(pt);
	disk_close(disk);
	free(frame_table);
	free(fifo_array);
	free(page_evics);

	// summary of the page faults, disk reads, and disk writes
	printf("Page Faults: %d\n", page_fault);
	printf("Disk Reads: %d\n", disk_reads);
	printf("Disk Writes: %d\n", disk_writes);

	return 0;
}
