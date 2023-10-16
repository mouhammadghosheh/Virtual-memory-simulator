#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include "sim_mem.h"

char main_memory[MEMORY_SIZE];


sim_mem::sim_mem(char exe_file_name1[], char exe_file_name2[], char swap_file_name[], int text_size,
                 int data_size, int bss_size, int heap_stack_size,
                 int num_of_pages, int page_size, int num_of_process) {
    int text_pages = text_size / page_size; //constructor variables initialization
    this->page_size = page_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_proc = num_of_process;
    this->data_size = data_size;
    this->num_of_pages = num_of_pages;
    this->text_size = text_size;
    char swap[text_size * 2];
    SI = 0;

    for (int i = 0; i < MEMORY_SIZE; ++i) {  //zeroing values in the main memory
        main_memory[i] = '0';
    }

    //opening files according to how many processes there are
    if (num_of_proc == 1) {
        this->program_fd[0] = open(exe_file_name1, O_RDONLY);
    } else {
        this->program_fd[0] = open(exe_file_name1, O_RDONLY);
        this->program_fd[1] = open(exe_file_name2, O_RDONLY);
    }
    this->swapfile_fd = open(swap_file_name, O_CREAT | O_RDWR | O_TRUNC, 0666);

    //swap text zeroing
    for (int i = 0; i < text_size; ++i) {
        swap[i] = '0';
    }
    //swap open
    for (int i = 0; i < page_size * (num_of_pages - text_pages); ++i) {
        if (write(this->swapfile_fd, swap, page_size) == -1) {
            perror("swap 0 failed");
            exit(1);
        }
    }

    //page table initializing
    this->page_table = (page_descriptor **) calloc(num_of_process, sizeof(page_descriptor *));
    for (int j = 0; j < num_of_proc; j++) {
        page_table[j] = (page_descriptor *) calloc(num_of_pages, sizeof(page_descriptor));

    }
    for (int j = 0; j < num_of_proc; j++) {
        for (int i = 0; i < num_of_pages; i++) {
            page_table[j][i].V = 0;
            page_table[j][i].D = 0;
            page_table[j][i].frame = -1;
            page_table[j][i].swap_index = -1;
            if(i<text_pages){
                page_table[j][i].P = 0;
            }
            if(i>=text_pages){
                page_table[j][i].P = 1;
            }

        }
    }
}

char sim_mem::load(int process_id, int address) { //load function from memory

    int id = process_id - 1;
    int page = address / page_size;
    int offset = address % page_size;
    char buffer[page_size];
    int p_frame;
    int phys;
    if (page_table[id][page].V == 1) { //if valid is 1
        p_frame = page_table[id][page].frame;
        phys = (p_frame * page_size) + offset;
        return main_memory[phys];
    } else {
        if (page_table[id][page].P == 0) { //if permission is 0
            lseek(program_fd[id], page * page_size, SEEK_SET); //go to this place in fd
            if (read(program_fd[id], buffer, page_size) == -1) { //read
                perror("READ FAILED2");
                exit(1);
            }
            buff(id, page, offset, buffer);
            p_frame = page_table[id][page].frame;
            phys = (p_frame * page_size) + offset; //physical address
            return main_memory[phys];
        } else if (page_table[id][page].P == 1 && page_table[id][page].D == 1) {  // if the page is dirty
            lseek(swapfile_fd, page_table[id][page].swap_index * page_size, SEEK_SET); //go to swap
            if (read(swapfile_fd, buffer, page_size) == -1) {
                perror("READING FAILED3");
                exit(1);
            }
            buff(id, page, offset, buffer);
            p_frame = page_table[id][page].frame;
            phys = (p_frame * page_size) + offset;
            return main_memory[phys];
        } else if (page_table[id][page].P == 1 && page_table[id][page].D == 0) {
            if (((page >= text_size/page_size) && page < ((text_size/page_size) + (data_size/page_size)))) { //if page is data
                page_table[id][page].D = 1;
                lseek(program_fd[id], page * page_size, SEEK_SET);
                if (read(program_fd[id], buffer, page_size) == -1) {
                    perror("READ FAILED2");
                    exit(1);
                }
                buff(id, page, offset, buffer);
                p_frame = page_table[id][page].frame;
                phys = (p_frame * page_size) + offset;
                return main_memory[phys];
            }
            else if(page >= ((text_size/page_size) + (data_size/page_size) )&& page <  ((text_size/page_size) + (data_size/page_size)  + (bss_size/page_size))){
                for (int i = 0; i < page_size; ++i) { //if page is bss
                    buffer[i] = '0';
                }
                buff(id,page,offset,buffer);
                p_frame = page_table[id][page].frame;
                phys = (p_frame * page_size) + offset;
                return main_memory[phys];
            }
            else {
                page_table[id][page].D = 1;
                perror("LOADING FROM HEAP STACK OR BSS IS NOT PERMITTED HERE"); //table already visited and written on
                exit(1);
            }
        }

    }

}

void sim_mem::store(int process_id, int address, char value) { //the store function
    char temp[page_size]; //usefull variables init
    int id = process_id - 1;
    int page = address / page_size;
    int offset = address % page_size;
    char buffer[page_size];
    int p_frame;
    int phys;
    if (page_table[id][page].V == 1) {
        p_frame = page_table[id][page].frame;
        page_table[id][page].D = 1;
        phys = (p_frame * page_size) + offset;
        main_memory[phys] = value;
    }
    else {
        if(page_table[id][page].P == 0){
            perror("No PERMISSION TO WRITE");
            exit(1);
        }
        else if(page_table[id][page].P == 1 && page_table[id][page].D == 0 && page >= text_size/page && page < (text_size/page)+(data_size/page)){
            lseek(program_fd[id], page * page_size, SEEK_SET);
            if (read(program_fd[id], buffer, page_size) == -1) {
                perror("READ FAILED2");
                exit(1);
            }
            page_table[id][page].D = 1;
            buff(id, page, offset, buffer);
            p_frame = page_table[id][page].frame;
            phys = (p_frame * page_size) + offset;
            main_memory[phys] = value;
        }
        else if(page_table[id][page].D == 0 && page >= (text_size/page)+(data_size/page)){
            for (int i = 0; i < page_size; ++i) {
                temp[i] = '0';
            }
            buff(id,page,offset,temp);
            p_frame = page_table[id][page].frame;
            page_table[id][page].D = 1;
            phys = (p_frame * page_size) + offset;
            main_memory[phys] = value;
        }
        else if(page_table[id][page].D == 1 && page >= (text_size/page)+(data_size/page)){
            lseek(swapfile_fd, page_table[id][page].swap_index * page_size, SEEK_SET);
            if (read(swapfile_fd, buffer, page_size) == -1) {
                perror("READING FAILED3");
                exit(1);
            }
            buff(id, page, offset, buffer);
            p_frame = page_table[id][page].frame;
            page_table[id][page].D = 1;
            phys = (p_frame * page_size) + offset;
            main_memory[phys]= value;
        }



    }


}

void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
    for (i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", main_memory[i]);
    }
}

void sim_mem::print_swap() {
    char *str = (char *) (malloc(this->page_size * sizeof(char)));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while (read(swapfile_fd, str, this->page_size) == this->page_size) {
        for (i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
    free(str);
}

void sim_mem::print_page_table() {
    int i;
    for (int j = 0; j < num_of_proc; j++) {
        printf("\n page table of process: %d \n", j);
        printf("Valid  Dirty  Permission  Frame  Swap index\n");
        for (i = 0; i < num_of_pages; i++) {
            printf("[%d] \t[%d] \t[%d] \t  [%d]   \t [%d] \n",
                   page_table[j][i].V,
                   page_table[j][i].D,
                   page_table[j][i].P,
                   page_table[j][i].frame,
                   page_table[j][i].swap_index);
        }
        printf("\n");
    }
}


sim_mem::~sim_mem() { //destructor
    if (num_of_proc == 2) {
        close(this->swapfile_fd);
        close(this->program_fd[0]);
        close(this->program_fd[1]);
    }
    if (num_of_proc == 1) {
        close(this->swapfile_fd);
        close(this->program_fd[0]);
    }

    for (int i = 0; i < num_of_proc; ++i) {
        free(page_table[i]);
    }
    free(page_table);

}

//memory check
bool sim_mem::isMemoryFull() {
    int counter ;
    for (int i = 0; i < MEMORY_SIZE; i += page_size) { //nested loop to check if there is an empty place in the memory
        counter = 0;
        for (int j = 0; j < page_size; ++j) {
            if (main_memory[j + i] == '0') {
                counter++;
            } else
                break;

        }
        if (counter == page_size){
            fifo.push(i/page_size);
            return false;
        }}

    return true;
}


//this function is responsible for updating the page table and for swapping out pages with the swap
void sim_mem::buff(int id,int page,int offset,char* buffer) {
    int a, b, counter=0;
    int ind;

    char temp[page_size];


    if (!isMemoryFull()) {

        for (a = 0; a < MEMORY_SIZE; a += page_size) {

            for (b = 0; b < page_size; b++) {
                if (main_memory[a+b] == '0') { //checking for the first empty place in memory
                    counter++;
                }

            }
            if (counter == page_size) {
                break;
            }
            else
                counter = 0;
        }
        ind = a / page_size;
        for (int y = 0; y < page_size; y++) {
            main_memory[y+ (ind*page_size)] = buffer[y]; // putting the page in the empty place that was found
            a++;
        }
        this->page_table[id][page].V = 1; //page table update
        this->page_table[id][page].frame = ind;

    } else {

        int front = fifo.front(); //using the queue to pop the first page to push it and swap it in swap file
        int front2 = SI;
        fifo.pop();
        int j2;
        int t;
        for (int j = 0; j < num_of_proc; ++j) { //page table updating
            for (int i = 0; i < num_of_pages; ++i) {
                if (page_table[j][i].frame == front) {
                    page_table[j][i].V = 0;
                    page_table[j][i].frame = -1;
                    t = i;
                    j2 = j;
                    break;
                }
            }

        }

        for (int i = front * page_size, k = 0; i < (front + 1) * page_size; ++i, k++) {
            temp[k] = main_memory[i];
            main_memory[i] = '0';
        }


        lseek(swapfile_fd, front2 * page_size, SEEK_SET); //go to swap to fill the frame we pushed from the queue
        SI++;
        if (write(swapfile_fd, temp, page_size) == -1) {
            perror("COULD NOT OPEN FILE"); //error handle
            exit(EXIT_FAILURE);

        }
        page_table[j2][t].swap_index = front2;


        for (int i = 0; i < page_size; ++i) {
            main_memory[(front * page_size) + i] = buffer[i]; //filling the place in the main memory that was pushed

        }
        page_table[id][page].frame = front;
        fifo.push(front);//pushing the queue to get the next value which we entered
                        //filling according to queue
        page_table[id][page].V = 1;
    }

}