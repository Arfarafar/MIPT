unsigned int MurMur_Hash(const void * key, int len, unsigned int seed = 0)
{

    const unsigned int m = 0xc6a4a793;
    const int r = 16;


    unsigned int h = seed ^ (len * m);


    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        h += *(unsigned int *)data;
        h *= m;
        h ^= h >> r;

        data += 4;
        len -= 4;
    }

    switch(len)
    {
        case 3:
            h += data[2] << 16;
        case 2:
            h += data[1] << 8;
        case 1:
            h += data[0];
            h *= m;
            h ^= h >> r;
    };

    h *= m;
    h ^= h >> 10;
    h *= m;
    h ^= h >> 17;

    return h;

}


template <typename T>
class Cell
{
public:
    T val_;
    size_t frequency_;
    size_t index_;

    Cell(T val, size_t frequency = 0, size_t index = -1) :
                                            val_(val),
                                            frequency_(frequency),
                                            index_(index)
                                            {}


};

template <typename T>
class Hashtable_t {
public:
    size_t capacity_of_hashtable_;
    std::list <Cell<T>*>** hashtable_;
    //list<Cell<T>*>*  all_cells_;

    Hashtable_t (size_t capacity) : capacity_of_hashtable_(capacity)
    {
        hashtable_ = new std::list <Cell<T>*>* [capacity_of_hashtable_];
        for (size_t i = 0; i < capacity_of_hashtable_; ++i)
        {
            hashtable_[i] = new std::list<Cell<T>*> ;
        }
       // all_cells_ = new list<Cell<T>*>;
    }

    ~Hashtable_t(){
        for (size_t i = 0; i < capacity_of_hashtable_; ++i)
        {
            delete hashtable_[i] ;
        }
        delete [] hashtable_;
       // delete all_cells_;
    }

    Cell<T>* Find_in_lists(T elem){

        unsigned int listnumber = MurMur_Hash(&elem, sizeof(T)) % capacity_of_hashtable_;
        typename std::list<Cell<T>*>::iterator it = hashtable_[listnumber] -> begin();

        for (size_t i = 0; i < hashtable_[listnumber] -> size(); ++i)
        {
             if( (*it) -> val_ == elem) {
                (*it) -> frequency_++;
                return *it;
             }
             advance(it, 1);
        }

        return NULL;

    }


    void Add_new_element(T elem, size_t ar_index) {

        unsigned int listnumber = MurMur_Hash(&elem, sizeof(T)) % capacity_of_hashtable_;
        Cell<T>* new_Cell = new Cell<T>(elem, 1, ar_index);

        hashtable_[listnumber] -> push_back(new_Cell);
        //all_cells_ -> push_front(new_Cell);
    }


    Cell<T>* Find_the_freaquest(){

        size_t minfreq = SIZE_MAX;
        Cell<T>* cell = nullptr;

        typename std::list<Cell<T>*>::iterator it;

        for (size_t j = 0; j < capacity_of_hashtable_; ++j){

            it = hashtable_[j] -> begin();

            for (size_t i = 0; i < hashtable_[j] -> size(); ++i)
            {
                 if( (*it) -> index_ + 1 && (*it) -> frequency_ < minfreq) {
                    minfreq = (*it) -> frequency_;
                    cell = *it;
                 }
                 advance(it, 1);
            }
        }

        return cell;
    }


};




template <typename T>
class Cache_t {
private:

    size_t maxsz_;
    size_t sz_;

public:

     T* cache_;

    Cache_t(size_t maxsz) : cache_((T*)calloc(maxsz, sizeof(T))),
                            maxsz_(maxsz),
                            sz_(0)
                            {}
    ~Cache_t() {
        free(cache_);
        maxsz_ = -1;
    }

    size_t getMaxsize() { return maxsz_; }
    size_t getSize() { return sz_; }

    T operator [](size_t index) {
        if (index < maxsz_)
            return cache_[index];
        else
            return cache_[maxsz_];
    }


    size_t Find_in_cache(T elem){
        for (size_t i = 0; i < sz_; i++){
            if (elem == cache_[i])
                return i;

        }
        return -1;
    }

    bool Replace_in_cache(T elem, size_t index){

        if (index < maxsz_){
            cache_[index] = elem;
            return true;
        }
        return false;
    }

    bool Add_in_cache(T elem){

        if(sz_ < maxsz_)
            return	cache_[sz_++] = elem;
        return false;
    }

    void Errase(){
        for (size_t i = 0; i < sz_; i++){
            cache_[i] = 0;
        }
        sz_ = 0;
    }


};


int getFileLength (FILE* flin){
    fseek(flin, 0, SEEK_END);
    int length = ftell(flin);
    fseek(flin, 0 , SEEK_SET);
    return  length;
}



///Template <typename T>
class Requestbuf_t {
private:
    size_t sz_;


public:

    char* buf_;

    Requestbuf_t(FILE* flin) : 	sz_(0)
                                {
                                        int len = getFileLength(flin);
                                        if (!len){
                                           // return nullptr;
                                        }
                                        buf_ = (char*)calloc(len, sizeof(char));
                                        sz_ = fread(buf_, sizeof(char), len, flin);
                                        fseek(flin, 0 , SEEK_SET);
                                }

    ~Requestbuf_t() {
        free(buf_);
        sz_ = -1;
    }

    size_t getSize() { return sz_; }

    char operator [](size_t index) {
        if (index < sz_)
            return buf_[index];
        else
            return buf_[sz_];
    }

    size_t Find_in_requestbuf(char elem, size_t start_pos = 0){
        for (size_t i = start_pos; i < sz_; i++){
            if (elem == buf_[i])
                return i;
        }
        return sz_;
    }

};
