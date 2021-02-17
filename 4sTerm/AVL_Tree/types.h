

enum errors{
    NORMAL_STATE = 0,
    IMPOSSIBLE_TO_INSERT,
    INVALID_TREE,
    TREE_IS_FULL,
    NO_SUCH_ELEMENT,
    NO_TREE,
    NO_FUNC,
    NO_DATA
};

typedef int Elem_t;
#define Elem_t_print "%d"

extern const int MAX_AVL_height;
extern const int MAX_AVL_SIZE;

typedef struct Node{
    Elem_t value;
    struct Node* link[2];
    char balance;
} Node;

typedef struct {
    Node* root;
    unsigned int size;
    int err;
} AVL_Tree;

