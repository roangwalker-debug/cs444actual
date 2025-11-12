#include <iostream>
#include <vector>
#include "CircularDynamicArray.cpp"

using namespace std;

template<typename keytype>
class BHeap {
    private:
        struct Node {
            keytype key;
            int degree;
            bool isRoot;
            Node* left;
            Node* right;
            Node* parent;
            CircularDynamicArray<Node*> child;
            //CircularDynamicArray<Node> *child;

            Node(keytype k) : key(k), degree(0), isRoot(true), left(nullptr), right(nullptr), parent(nullptr) {}
        };
        Node* min;
        //size_t minPos;
        //vector<Node*> rootList;
        int size;
        int rootSize;

        void consolidate() {
            //vector<Node*> check(size, nullptr);
            Node* check[size];
            for (int i = 0; i < size; i++) {
                check[i] = nullptr;
            }
            int maxDeg = 0;
            bool printedMin = false;
            Node* curr = min;
            Node* next = nullptr;
            while ((curr != min || !(printedMin)) && curr != nullptr) {
                if (curr == min) {
                    printedMin = true;
                }
                next = curr->right;
                int deg = curr->degree;
                while (check[deg] != nullptr) {
                    Node* degree = check[deg];
                    if (curr->key > degree->key) {
                        Node* temp = curr;
                        curr = degree;
                        degree = temp;
                    }
                    combine(curr, degree);
                    check[deg] = nullptr;
                    deg++;
                }
                check[deg] = curr;
                if (curr->degree > maxDeg) {
                    maxDeg = curr->degree;
                }
                curr = next;
            }
            min->right = nullptr;
            min->left = nullptr;
            min = nullptr;
            int count = 0;
            Node* first = nullptr;
            Node* itt = nullptr;
            //for (Node* node : check)
            for(int i = 0; i <= maxDeg; i++) {
                if(check[i] != nullptr) {
                    Node* node = check[i];
                    if (count == 0) {
                        min = node;
                        first = node;
                        itt = node;
                        node->parent = nullptr;
                        node->left = node;
                        node->right = node;
                    } else {
                        itt->right = node;
                        node->left = itt;
                        itt = node;
                        node->parent = nullptr;
                        if (node->key < min->key) {
                            min = node;
                        }
                            //Node* ind = first;
                            //for (i = 0; i < count - 1; i++) {
                            //ind = ind->left
                            //}
                    }
                    count++;
                }
            }
            itt->right = first;
            first->left = itt;
            //for(Node* node : rootList) {
                /*Node* curr = node;
                int deg = curr->degree;
                track++;
                while (check[deg] != nullptr) {
                    Node* degree = check[deg];
                    if(curr->key > degree->key) {
                        Node* temp = curr;
                        curr = degree;
                        degree = temp;
                    }
                    combine(curr, degree);
                    check[deg] = nullptr;
                    deg++;
                    track--;
                }
                check[deg] = curr;
            }
            rootList.clear();
            rootList.resize(track, nullptr);
            //cout << "this is track" << track << endl;
            min = nullptr;
            int count = 0;
            for (Node* node : check) {
                if (node != nullptr) {
                    //cout << "isnot null" << endl;
                    if (count == 0) {
                        min = node;
                    }
                    node->parent = nullptr;
                    if (count > 0) {
                        node->left = rootList[count - 1];
                        if (node->left != nullptr) {
                            node->left->right = node;
                        }
                    }
                    if (min->key > node->key) {
                        min = node;
                        minPos = count;
                    }
                    rootList[count] = node;
                    count++;
                }
                //cout << "this is count" << count << endl;
            }
            rootList[0]->left = rootList[rootList.size() - 1];
            rootList[rootList.size() - 1]->right = rootList[0];
            int calc = 0;
            for (int i = 0; i < rootList.size(); i++) {
                if (i == 0) {
                    min = rootList[i];
                }
                if (min->key >= rootList[i]->key) {
                    min = rootList[i];
                    minPos = calc;
                }
                calc++;
            }*/
            
        }
        void combine(Node* par, Node* c) {
            c->parent = par;
            c->left = nullptr;
            c->right = nullptr;
            par->child.addEnd(c);
            c->isRoot = false;
            par->degree++;
            rootSize--;
        }
        void nodeInsert(Node* node) {
            keytype k = node->key;
            Node* newNode = copyNode(node);
            if (min == nullptr) {
                min = newNode;
                min->right = min;
                min->left = min;
                //rootList.push_back(node);
            } else {
                newNode->left = min->left;
                newNode->right = min;
                min->left = newNode;
                newNode->left->right = newNode;
            }
        }
        void recursivePrint(Node* node) {
            if (node == nullptr) return;
            cout << node->key << " ";
            for (int i = 0; i < node->child.length(); i++) {
                if (node->child[i] != nullptr) {
                    recursivePrint(node->child[i]);
                }
            }
        }
        Node* copyNode(Node* node) {
            Node* temp = new Node(node->key);
            temp->degree = node->degree;
            temp->isRoot = node->isRoot;
            temp->parent = node->parent;
            for (int i = 0; i < node->child.length(); i++) {
                temp->child.addEnd(copyNode(node->child[i]));
            }
            return temp;
        }
        public:
        BHeap(const BHeap& old) {
            min = new Node(old.min->key);
            size = old.size;
            rootSize = old.rootSize;
            bool printedMin = false;
            Node* curr = old.min;
            Node* other = copyNode(curr);
            other->right = copyNode(curr->right);
            other->right->left = other;
            other = other->right;
            curr = curr->right;
            while (other->right != min) {
                other->right = copyNode(curr->right);
                other->right->left = other;
                curr = curr->right;
                other = other->right;
            }
            /*minPos = old.minPos;
            rootList.clear();
            min = new Node(old.min->key);
            size = old.size;

            for (int i = 0; i < old->rootList.size(); i++) {
                Node* other = old.rootList[i];
                Node* newNode = copyNode(other);
                rootList.push_back(newNode);
            }
            min = rootList[0]->key;
            for (int i = 0; i < rootList.size(); i++) {
                if (rootList[i]->key < min->key) {
                    min = rootList[i];
                }
                if (i == 0) {
                    rootList[0]->right = rootList[1];
                    rootList[0]->left = rootList[rootList.size() - 1];
                } else if (i == rootList.size() - 1) {
                    rootList[rootList.size() - 1]->right = rootList[0];
                    rootList[rootList.size() - 1]->left = rootList[rootList.size() - 2];
                } else {
                    rootList[i]->left = rootList[i - 1];
                    rootList[i]->right = rootList[i + 1];
                }
            }*/

        }
        BHeap& operator=(const BHeap& rhs) {
            if (this != &rhs) {
                /*for (Node* node : rootList) {
                    recursiveDelete(node);
                }*/
                Node* del = min;
                Node* next = del->right;
                while (del != nullptr) {
                    recursiveDelete(del);
                    del = next;
                }
                //minPos = rhs.minPos;
                //rootList.clear();
                min = new Node(rhs.min->key);
                size = rhs.size;
                rootSize = rhs.rootSize;

                bool printedMin = false;
                Node* curr = rhs.min;
                Node* other = copyNode(curr);
                other->right = copyNode(curr->right);
                other->right->left = other;
                other = other->right;
                curr = curr->right;
                while (other->right != min) {
                    other->right = copyNode(curr->right);
                    other->right->left = other;
                    curr = curr->right;
                    other = other->right;
                }
                /*for (int i = 0; i < rhs->rootList.size(); i++) {
                    Node* other = rhs.rootList[i];
                    Node* newNode = copyNode(other);
                    rootList.push_back(newNode);
                }*/
                /*min = rootList[0]->key;
                for (int i = 0; i < rootList.size(); i++) {
                    if (rootList[i]->key < min->key) {
                        min = rootList[i];
                    }
                    if (i == 0) {
                        rootList[0]->right = rootList[1];
                        rootList[0]->left = rootList[rootList.size() - 1];
                    } else if (i == rootList.size() - 1) {
                        rootList[rootList.size() - 1]->right = rootList[0];
                        rootList[rootList.size() - 1]->left = rootList[rootList.size() - 2];
                    } else {
                        rootList[i]->left = rootList[i - 1];
                        rootList[i]->right = rootList[i + 1];
                    }
                }*/
            }
            return *this;
        }

        BHeap() : min(nullptr), size(0), rootSize(0) {}

        BHeap(keytype k[], int s) {
            size = 0;
            min = nullptr;
            rootSize = 0;
            //rootList.clear();
            for (int i = 0; i < s; i++) {
                insert(k[i]);
            }
            consolidate();
        }
        ~BHeap() {
            /*for (Node* node : rootList) {
                recursiveDelete(node);
            }
            //rootList.clear();*/
            Node* del = min;
            Node* next = nullptr;
            for (int i = 0; i < rootSize; i++) {
                next = del->right;
                recursiveDelete(del);
                del = nullptr;
                del = next;
            }
        }
        void recursiveDelete(Node* node) {
            if (node == nullptr) return;
            for (int i = 0; i < node->child.length(); i++) {
                recursiveDelete(node->child[i]);
            }
            //node->child.clear();
            node = nullptr;
            delete node;
            //node = nullptr;
        }
        keytype peekKey() const {
            //cout << endl;
            keytype key = min->key;
            key += "\n";
            //return min->key;
            return key;
        }
        keytype extractMin() {
            if (min == nullptr) {
                return NULL;
            }
            int track = min->child.length();
            keytype key = min->key;
            Node* temp = min;
            Node* newMin = min->left;
            if (size == 1) {
                delete min;
                min = nullptr;
                //rootList.clear();
            } else if (min->child.length() > 0) {
                min = min->child[0];
                Node* c = nullptr;
                
                for (int i = 0; i < temp->child.length(); i++) {
                    c = temp->child[i];
                    //rootList.insert(rootList.begin() + minPos + i, c);
                    c->parent = nullptr;
                    c->left = temp->left;
                    c->left->right = c;
                    c->right = temp->right;
                    c->right->left = c;
                    temp->left = c;
                    c->isRoot = true;
                }
            } else {
                min->left->right = min->right;
                min->right->left = min->left;
                min = min->left;
            }
            //rootList.erase(rootList.begin() + minPos + track);
            if (min != nullptr) {
                Node* curr = min;
                Node* first = min;
                bool printedMin = false;
                while (curr != first || !(printedMin)) {
                    if (curr == first) {
                        printedMin = true;
                    }
                    if (curr->key < min->key) {
                        min = curr;
                    }
                    curr = curr->right;
                }
            }
            temp->child.clear();
            delete temp;
            min = newMin;
            consolidate();
            size--;
            rootSize--;
            key += "\n";
            return key;
        }
        void insert(keytype k) {
            Node* newNode = new Node(k);
            if (min == nullptr) {
                min = newNode;
                //rootList.push_back(newNode);
                newNode->left = newNode;
                newNode->right = newNode;
            } else if (k < min->key) {
                newNode->left = min->left;
                newNode->right = min;
                min->left = newNode;
                //if (newNode->left == nullptr) {
                    //newNode->left = rootList[rootList.size() - 1];
                    //rootList[rootList.size() - 1]->right = newNode;
                //} else {
                newNode->left->right = newNode;
                min = newNode;
                //rootList.insert(rootList.begin() + minPos, newNode);
            } else {
                //rootList.insert(rootList.begin() + minPos, newNode);
                newNode->left = min->left;
                newNode->right = min;
                min->left = newNode;
                //if (newNode->left == nullptr) {
                    //newNode->left = rootList[rootList.size() - 1];
                    //rootList[rootList.size() - 1]->right = newNode;
                //} else {
                newNode->left->right = newNode;
                
                //minPos++;
            }
            size++;
            rootSize++;
        }
        void merge(BHeap<keytype>& H2) {
            Node* curr = H2.min;
            Node* next = nullptr;
            bool printedMin = false;
            while (curr != H2.min || !(printedMin)) {
                next = curr->right;
                if (curr == H2.min) {
                    printedMin = true;
                }
                nodeInsert(curr);
                curr = next; 
            }
            Node* itt = min;
            Node* first = min;
            bool printed = false;
            while (itt != first || !(printed)) {
                if (itt == first) {
                    printed = true;
                }
                if (itt->key < min->key) {
                    min = itt;
                }
                itt = itt->right;
            }
            size += H2.size;
            rootSize += H2.rootSize;
            //H2.~BHeap();
        }
        void printKey() {
            bool printedMin = false;
            Node* curr = min;
            while (curr != min || !(printedMin)) {
                if (curr == min) {
                    printedMin = true;
                }
                //cout << "B" << curr->degree << endl;
                cout << "B" << curr->degree << ":" << endl;
                recursivePrint(curr);
                cout << endl;
                cout << endl;
                curr = curr->right;
            }
        }
};
