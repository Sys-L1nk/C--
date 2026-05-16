#include <iostream>
using namespace std;
struct Tree {
    int info;
    Tree* left;
    Tree* right;
};

Tree* Insert(Tree* Root, int X) {
    if (Root == NULL) {
        Tree* P = new Tree;
        P->info = X;
        P->left = NULL;
        P->right = NULL;
        return P;
    }

    if (X < Root->info) {
        Root->left = Insert(Root->left, X);
    }
    else if (X > Root->info) {
        Root->right = Insert(Root->right, X);
    }
    
    return Root;
}

void Inorder(Tree* Root) {
    if (Root != NULL) {
        Inorder(Root->left);
        cout << Root->info << " ";
        Inorder(Root->right);
    }
}

void Preorder(Tree* Root) {
    if (Root != NULL) {
        cout << Root->info << " ";
        Preorder(Root->left);
        Preorder(Root->right);
    }
}
void Postorder(Tree* Root) {
    if (Root != NULL) {
        Postorder(Root->left);
        Postorder(Root->right);
        cout << Root->info << " ";
    }
}

int main() {
    Tree* Root = NULL; 
    Root = Insert(Root, 5);
    Root = Insert(Root, 3);
    Root = Insert(Root, 8);
    Root = Insert(Root, 1);
    Root = Insert(Root, 4);
    Root = Insert(Root, 6);
    Root = Insert(Root, 9);

    cout << "--- Binary Search Tree Traversals ---" << endl;
    cout << "\nInorder (Left-Root-Right):  ";
    Inorder(Root);    

    cout << "\nPreorder (Root-Left-Right): ";
    Preorder(Root);   
    
    cout << "\nPostorder (Left-Right-Root): ";
    Postorder(Root);  
    
    cout << "\n" << endl;

    return 0;
}