#include"ThreadPool.h"
#include<iostream>

int main(){
    ThreadPool pool(4);

    for(int i=0;i<10 ; i++){
        pool.enqueue([]{
            std::cout << "Task executed\n" << std::endl;
        });
    }

    return 0;
}