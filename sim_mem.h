

#ifndef C__EX_SEM_MEM_H
#define C__EX_SEM_MEM_H
#define MEMORY_SIZE 20
#include <queue>
extern char main_memory[MEMORY_SIZE]; //main memory initlization
using namespace std;
typedef struct page_descriptor
{
    int V; // valid
    int D; // dirty
    int P; // permission
    int frame; //the number of a frame if in case it is page-mapped
    int swap_index; // where the page is located in the swap file.
} page_descriptor;


class sim_mem {
    int swapfile_fd; //swap file fd
    int program_fd[2]; //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    queue<int> fifo;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    int num_of_proc;
    int SI;
    page_descriptor **page_table; //pointer to page table
public:

    sim_mem(char exe_file_name1[],char exe_file_name2[], char swap_file_name2[], int text_size,
            int data_size, int bss_size, int heap_stack_size,
            int num_of_pages, int page_size, int num_of_process);
    ~sim_mem();
    char load(int process_id, int address);
    void store(int process_id, int address, char value);
    void print_memory();
    void print_swap ();
    void buff(int id,int page,int offset,char *buffer);
    void print_page_table();
    bool isMemoryFull();
    void swap_out(int address,char* main_buff);
};





#endif //C__EX_SEM_MEM_H