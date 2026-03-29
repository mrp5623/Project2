#pragma once

#include <iostream>
#include <string>
#include "Fragrance.h"

enum Color {red, black};

class Set
{ 
    struct Node {
        Fragrance* frag; //fragrance
        Color color;     //red/black
        Node* parent;
        Node* left;
        Node* right;

        Node(Fragrance* f) : frag(f), color(red), parent(nullptr), left(nullptr), right(nullptr) {};
    };  
    
    Node* root;
    int size;

    //getters
    Color getColor(Node* n) {return (n == nullptr) ? black : n->color;}
    int getSize() const {return size;}

    //helpers
    //changes colors to maintain red-black properties after insertion
    void insertHelper(Node* c) {
        while (c->parent && c->parent->color == red) {
            if (c->parent == c->parent->parent->left) {
                Node* uncle = c->parent->parent->right;
                
                //uncle red
                if (uncle && uncle->color == red) {
                    c->parent->color = black;
                    uncle->color = black;
                    c->parent->parent->color = red;
                    c = c->parent->parent;
                } 
                else { //uncle black 
                    
                    if (c == c->parent->right) {
                        c = c->parent;
                        rotateLeft(c);
                    }
                    
                    //outer chiuld
                    c->parent->color = black;
                    c->parent->parent->color = red;
                    rotateRight(c->parent->parent);
                }
            } 
            else { //other side
                Node* uncle = c->parent->parent->left;
                
                if (uncle && uncle->color == red) {
                    c->parent->color = black;
                    uncle->color = black;
                    c->parent->parent->color = red;
                    c = c->parent->parent;
                } 
                else {
                    if (c == c->parent->left) {
                        c = c->parent;
                        rotateRight(c);
                    }
                    c->parent->color = black;
                    c->parent->parent->color = red;
                    rotateLeft(c->parent->parent);
                }
            }
        }
        root->color = black;
    }

    //replaces c with r
    void switchErase(Node* c, Node* r) {
        if (c->parent == nullptr) {root = r;}
        else if (c == c->parent->left) {c->parent->left = r;}
        else {c->parent->right = r;}

        if (r != nullptr) {r->parent = c->parent;}
    }

    //rotations to maintain red-black properties after deletion
    void rotateLeft(Node* a) {
        Node* b = a->right;
        a->right = b->left;
        
        if (b->left) b->left->parent = a;
        b->parent = a->parent;
        
        if (!a->parent) {
            root = b;
        } else if (a == a->parent->left) {
            a->parent->left = b;
        } else {
            a->parent->right = b;
        }
        
        b->left = a;
        a->parent = b;
    }

    void rotateRight(Node* b) {
        Node* a = b->left;
        b->left = a->right;
        
        if (a->right) a->right->parent = b;
        a->parent = b->parent;
        
        if (!b->parent) {
            root = a;
        } else if (b == b->parent->left) {
            b->parent->left = a;
        } else {
            b->parent->right = a;
        }
        
        a->right = b;
        b->parent = a;
    }

    //keeps red-black properties after deletion
    void removeBlack(Node* c, Node* cParent) {
          while (c != root && getColor(c) == black) {
            if (c == cParent->left) {
                Node* w = cParent->right;
                if (getColor(w) == red) {                   
                    w->color = black;
                    cParent->color = red;
                    rotateLeft(cParent);
                    w = cParent->right;
                }
                if (getColor(w->left) == black && getColor(w->right) == black) {
                    w->color = red;                       
                    c = cParent;
                    cParent = c->parent;
                } else {
                    if (getColor(w->right) == black) {         
                        if (w->left) w->left->color = black;
                        w->color = red;
                        rotateRight(w);
                        w = cParent->right;
                    }
                    w->color = cParent->color;               
                    cParent->color = black;
                    if (w->right) w->right->color = black;
                    rotateLeft(cParent);
                    c = root;
                }
            } else {                                     
                Node* w = cParent->left;
                if (getColor(w) == red) {
                    w->color = black;
                    cParent->color = red;
                    rotateRight(cParent);
                    w = cParent->left;
                }
                if (getColor(w->right) == black && getColor(w->left) == black) {
                    w->color = red;
                    c = cParent;
                    cParent = c->parent;
                } else {
                    if (getColor(w->left) == black) {
                        if (w->right) w->right->color = black;
                        w->color = red;
                        rotateLeft(w);
                        w = cParent->left;
                    }
                    w->color = cParent->color;
                    cParent->color = black;
                    if (w->left) w->left->color = black;
                    rotateRight(cParent);
                    c = root;
                }
            }
        }
        if (c) c->color = black;
    }

    //inorder recursion to find intersection of two sets
    void inorderIntersectHelper(Node* node, const Set& other, Set& result) const {
        if (!node) return;

        inorderIntersectHelper(node->left, other, result);

        // Check for intersection
        if (other.contains(node->frag->name)) {
            result.insert(node->frag);
        }

        inorderIntersectHelper(node->right, other, result);
    }

    //for copy constructor and assignment operator
    void copyHelper(Node* node) {
        if (!node) return;

        copyHelper(node->left);
        insert(node->frag);
        copyHelper(node->right);
    }

    //inorder print
    void printHelper(Node* node, int& count) {
            if (!node) return;

            //inorder
            printHelper(node->left, count);
            std::cout << count++ << ". " << node->frag->name << ": " << node->frag->url << std::endl;
            printHelper(node->right, count);
    }

    //recursive destructor helper
    void dest(Node* n) {
        if (n) {
            dest(n->left);
            dest(n->right);
            delete n;
        }
    }

    public:
        Set() : root(nullptr), size(0) {};
        ~Set() {dest(root);}

        bool isEmpty() const {
            return root == nullptr;
        }

        //copy constructor and assignment operator
        Set(const Set& other) : root(nullptr), size(0) {
            copyHelper(other.root);
        }

        Set& operator=(const Set& other) {
            if (this != &other) {
                clear();
                copyHelper(other.root);
            }
            return *this;
        }

        //insert

        bool insert(Fragrance* frag) {
            Node* newNode = new Node(frag);

            if (root == nullptr) {
                root = newNode;
                root->color = black;
                size++;
                return true;
            }
            
                Node* a = nullptr; 
                Node* b = root;

                while(b) {
                    a = b;
                    if (newNode->frag->name < b->frag->name) {
                        b = b->left;
                    }
                    else if (newNode->frag->name > b->frag->name) {
                        b = b->right;
                    }
                    else {
                        delete newNode;
                        return false;
                    }
                }
                
                newNode->parent = a;
                if (newNode->frag->name < a->frag->name) {
                    a->left = newNode;
                } else {
                    a->right = newNode;
                }
            
            
                size++;
                insertHelper(newNode);
                return true;
        }

        //remove
        bool erase(std::string name) {
            Node* current = root;
            while (current) {
                if (name < current->frag->name) {
                    current = current->left;
                } else if (name > current->frag->name) {
                    current = current->right;
                } else {
                    break;
                }
            }

            if (!current) return false;
    
            Node* c = current;
            Node* replacer = nullptr;
            Node* replacerParent;
            Color c_color = c->color;
    
            //no left child
            if(!current->left) {
                replacer = current->right;
                replacerParent = current->parent;
                switchErase(current, current->right);
            }

            //no right child
            else if (!current->right) {
                replacer = current->left;
                replacerParent = current->parent;
                switchErase(current, current->left);
            }

            //two kids
            else {
                c = current->right;
                while (c->left != nullptr) c = c->left;
                c_color = c->color;
                replacer = c->right;
                replacerParent = c;
                if (c->parent != current) {
                    replacerParent = c->parent;
                    switchErase(c, c->right);
                    c->right = current->right;
                    c->right->parent = c;
                }
                switchErase(current, c);
                c->left = current->left;
                c->left->parent = c;
                c->color = current->color;
            }

            delete current;
            if (c_color == black) {
                removeBlack(replacer, replacerParent);
            }
            size--;
            return true;
        } 
        void clear() {
            dest(root);
            root = nullptr;
            size = 0;
        }


        //has?
        bool contains(const std::string name) const {
            Node* current = root;
            while (current) {
                if(name < current->frag->name) {
                    current = current->left;
                }
                else if (name > current->frag->name) {
                    current = current->right;
                }
                else {
                    return true;
                }
            }

                return false;
        }

        //returns fragrance not iterator
        Fragrance* find(const std::string name) const {
            Node* current = root;
            while (current) {
               if(name < current->frag->name) {
                   current = current->left;
               }
               else if (name > current->frag->name) {
                   current = current->right;
               }
               else {
                   return current->frag;
               }
            }

            return nullptr;
        }

        //move const
        Set(Set&& other) noexcept : root(other.root), size(other.size) {
            other.root = nullptr;
            other.size = 0;
        }

        Set intersect(const Set& other) const {
            Set both;
            inorderIntersectHelper(root, other, both);
            return both;
        }

        void printResults() {
            int num = 1;
            printHelper(root, num);
        }
};
