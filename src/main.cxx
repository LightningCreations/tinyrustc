#include <iostream>
#include <iomanip>

#include <hash.hxx>
#include <tuple>
#include <lex.hxx>

int main(){
    std::cout << "Hello World" << std::endl;

    std::cout << "Hash of (1,2,3) is 0x" << std::hex << trustc::hasher<>{}(std::make_tuple(1,2,3)) << std::endl;
}