# Virtual-memory-simulator
Mouhammed ghosheh

Description
The program is a simulation memory (RAM), We use paging
The virtual memory is divided into pages ,these pages are brought to the main memory (RAM)
the program can handle two proccesses to load and store from and to memory



functions:
two main functions:
1.load- this method receive a proccess id and an address to load from the memory

2.store- this method is very similar to "load" function,receive an address, proccess id and a value,
to store the address in the RAM the method need to insert the value in the RAM.

3-ismemoryfull(): this function checks if the memory is full

4-buff(int id,int page,int id,char* buffer): this function updates the memory according to the buffer and the page table and also it updates the page table according to the case we are in (data,heap ,bss) and also it does the fifo algorithm using a queue to swap pages between the main memory and the swap

5-print_memory(): this function prints the main memory

6-print_swap(): this function prints the contents of the swap file (in pages)

7-print_page_table(): this function prints the values of the page table
       

==Program Files==
sim_mem.cpp- the file contain only functions
sim_mem.h- a header file ,contains declerations of functions.
main.cpp- contain the main only.
Makefile-to compile the program.
==How to compile?==
compile: g++ sim_mem.cpp main.cpp -o ex5
run: ./ex5

==Input:==
no input

==Output:==
main memory (RAM)
swap file
page table
