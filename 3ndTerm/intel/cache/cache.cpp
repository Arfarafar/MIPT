#include <iostream>
#include <stdlib.h>
#include <list>
#include <cstdint>
#include "types.h"



int Do_Ideal_cache(Cache_t<char>& cache, Requestbuf_t& requestbuf){

    int hit = 0;
    size_t bufsize = requestbuf.getSize();

    for (size_t i = 0; i < requestbuf.getSize(); i++){
        if(cache.Find_in_cache(requestbuf[i]) + 1){
            hit++;
        }
        else if (cache.getSize() < cache.getMaxsize()){
            cache.Add_in_cache(requestbuf[i]);
        } else {

            size_t maxpos = 0;
            size_t index = -1;

            for (size_t j = cache.getSize() ; j > 0; j--){

                size_t tmp = requestbuf.Find_in_requestbuf(cache[j-1], i);

                if (tmp > maxpos){
                    maxpos = tmp;
                    index = j-1;
                    if(maxpos == bufsize - 1)
                        break;
                }

            }

            cache.Replace_in_cache(requestbuf[i], index);
        }

    }

    return hit;
}


template <typename T>
int Do_LFU_cache(Cache_t<T>& cache, Requestbuf_t& requestbuf, int capacity_of_hashtable){

    int hit = 0;
    Hashtable_t<T> hashtable(capacity_of_hashtable);

    for (size_t i = 0; i < requestbuf.getSize(); i++){
        Cell<T>* cell = hashtable.Find_in_lists(requestbuf[i]);
        if(cell && cell -> index_ + 1){
            hit++;
            cell -> frequency_++;
        }
        else if (cache.getSize() < cache.getMaxsize()){
            cache.Add_in_cache(requestbuf[i]);
            hashtable.Add_new_element(requestbuf[i], cache.getSize() - 1);
        }
        else if (cell) {
            Cell<T>* cell_toreplace = hashtable.Find_the_leastfreaquent();
            cell -> index_ =  cell_toreplace -> index_;
            cell_toreplace -> index_ = -1;

            cache.Replace_in_cache(requestbuf[i], cell -> index_);
        }
        else {
            cell = hashtable.Find_the_leastfreaquent();

            size_t index = cell -> index_;
            cell -> index_ = -1;

            cache.Replace_in_cache(requestbuf[i], index);
            hashtable.Add_new_element(requestbuf[i], index);
        }

    }

    return hit;
}





int main(int argc, char* argv[]){

    /*if (argc < 2){
        FILE* flin = fopen("test.txt", "rb");
    }
    else */
    FILE* flin;
    if ((flin = fopen(argv[1], "rb")) == NULL){
        std::cout << "cannot open this file" ;
        return 1;
    }


    char* strend;
    long cachesize = strtol(argv[2], &strend, 10);

    Cache_t<char> cache (cachesize);
    Requestbuf_t requestbuf(flin);

    std::cout << "Ideal cache: " << Do_Ideal_cache(cache, requestbuf) << std::endl;
    cache.Errase();
    std::cout << "LFU cache: " <<  Do_LFU_cache<char>(cache, requestbuf, 4) << std::endl;

    return 0;
}