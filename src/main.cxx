#include <iostream>

#include <hash.hxx>
#include <tuple>

int main(){
    std::cout << "Hello World" << std::endl;

    std::cout << "Hash of (1,2,3) is " << trustc::hasher<>{}(std::make_tuple(1,2,3)) << std::endl;
}