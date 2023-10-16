//id: 322310509
//name: mohammed ghosheh
#include "sim_mem.h"
#include <iostream>

using namespace std;

int main(){
    char exec1[20] = "exec_file";
    char exec2[20] = "exec_file2";//exec_file2 if used
    char swap[20] = "swap_file";
    sim_mem mem_sim(exec1,exec2,swap,25,50,25,25,25,5,2);

cout<< mem_sim.load(1,0);
cout<< mem_sim.load(1,5);
cout<< mem_sim.load(1,10);
cout<< mem_sim.load(1,15);
cout<< mem_sim.load(1,20);
cout<< mem_sim.load(1,25);
cout<< mem_sim.load(1,30);
cout<< mem_sim.load(1,35);
cout<< mem_sim.load(1,40);
cout<< mem_sim.load(2,45);
cout<< mem_sim.load(2,50);

 

    mem_sim.store(1,40,'E');
    mem_sim.store(1,41,'X');
    mem_sim.store(1,42,'5');
  

    mem_sim.print_memory();
    mem_sim.print_swap();
    mem_sim.print_page_table();
    return 0;
}
