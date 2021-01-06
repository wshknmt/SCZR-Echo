#include <iostream>
#include <fstream> 

int main(){
    std::cout << "proc_c: Starting..." << std::endl;
    std::ofstream outfile ("c_out.txt");
    outfile << "proc_c here, kisses!" << std::endl;
    outfile.close();

    return 0;
}