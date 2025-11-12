#include <iostream>
using namespace std;
#include "BHeap.cpp"

/*int main(){
	char K[6] = {'a','b','c','d','e','f'};
	
	BHeap<char> H1, H2;
	for(int i=0; i<6; i++) H1.insert(K[i]);
	//H1.printKey();
	cout << H1.extractMin() << endl; //Should output a
	
	H1.printKey();
	//Should output "B2:\n b c d e\n B0:\n f \n"
	
	H1.insert('g'); H1.insert('h'); H1.insert('a'); H1.insert('i');
	
	H1.printKey();

	//Should output "B0:\n a\n B2:\n b c d e\n B0:\n f\n B0:\n g\n B0:\n h\n B0:\n i\n"
	//cout << " this is min pos" << H1.rootList[H1.minPos] << endl;
	cout << H1.extractMin() << endl; 	//Should output a
	H1.printKey();	
	//Should output "B3: b c d e f g h i\n"
	//
	cout << "min Pos" << H1.minPos << endl;
	cout << "key at min Pos" << H1.rootList[H1.minPos]->key << endl;
	cout << "root list" << H1.rootList.size() << endl;
	//
	H1.insert('j');
	//cout << "inserted j" << endl;
	H1.insert('k');
	//cout << " inserted k" << endl;
	H1.insert('l');
	//cout << "success" << endl;
	H1.printKey();	
	//
	cout << "min Pos" << H1.minPos << endl;
	cout << "key at min Pos" << H1.rootList[H1.minPos]->key << endl;
	cout << "min key" << H1.min->key << endl;
	cout << "root list" << H1.rootList.size() << endl;
    //
	//cout << "minKey" << H1.min->key << endl;
	cout << H1.extractMin() << endl;	//Should output b

	H1.printKey();
	//Should output	B3:\n c j d e f g h i\n B1:\n k l\n"
	
	H2.insert('A'); H2.insert('B'); H2.insert('C'); H2.insert('D');
	cout<< H2.extractMin() << endl;	//Should output A

	H2.printKey();
	//Should output "B1:\n B C\n B0:\n D\n"
	
	H1.merge(H2); H1.printKey();
	//Should output "B1: B C\n B0:\n D\n B3:\n c j d e f g h i\n B1:\n k l\n"
	
	cout << H1.extractMin() << endl;	//Should output B

	H1.printKey();
	//Should output "B2:\n C D k l\n B3:\n c j d e f g h i\n"
	
	return 0;
}*/
void printErrors(string errors, int numOfErrors){
    if(numOfErrors < 5){
        cout << errors << " passed (Key:imeidj)" << endl;
	}else if(numOfErrors < 100){
        cout << errors << " caused " << numOfErrors << " of errors ADD+1(Key:kdjoum)" << endl;
        cout << errors << " caused " << numOfErrors << " of errors ADD+1(Key:kdjoum)" << endl;
        cout << errors << " caused " << numOfErrors << " of errors ADD+1(Key:kdjoum)" << endl;
    }else if (numOfErrors < 1000){
        cout << errors << " caused " << numOfErrors << " of errors ADD+1(Key:kdjoum)" << endl;
        cout << errors << " caused " << numOfErrors << " of errors ADD+1(Key:kdjoum)" << endl;
    }else if (numOfErrors < 10000){
        cout << errors << " caused " << numOfErrors << " of errors ADD+1(Key:kdjoum)" << endl;
    }else{
        cout << errors << " caused " << numOfErrors << " of errors " << endl;
    }
}

int main(){
    string A[20] = {"AD","AB","AA","A","B","C","D","E","F","H","I","J","K","L","M","O","Q","R","S","T"};
        
    BHeap<string> Z(A,20), Y;

    Z.printKey();
    //B2: A AA AB AD\n B4: B C D E F H I J K L M O Q R S T 

    cout << Z.extractMin() << endl;       //A

    Z.printKey();
    //B0:AA\n B1:AB AD\n B4:B C D E F H I J K L M O Q R S T
        
    Y.insert("p"); 
    Y.insert("h");
    Y.insert("a");
    Y.insert("s");
    Y.insert("e");
    Y.insert("3");
    Y.printKey();
    //B0: 3\n B0: a\n B0: p\n B0: h\n B0: s\n B0: e\n
        
    Y.merge(Z);
    
    Y.printKey();
    //B0: AA\n B1:AB AD\n B4:B C D E F H I J K L M O Q R S T\n B0: 3\n B0: a\n B0: p\n B0: h\n B0: s\n B0: e\n
    
    cout << Y.peekKey() << endl;            //AA
    cout << Y.extractMin() << endl;       //AA
    Y.printKey();
    //B2: AB AD a p\n B4:B C D E F H I J K L M O Q R S T\n B0: e\n  B1: h s\n
        
    BHeap<int> X;
    for(int i = 0; i < 100000; i++){
        X.insert(i);
    }
    int peakKey=0;
    int extractMin=0;
    for(int i = 0; i < 100000; i++){
        if(X.peekKey() != i) peakKey++;
        if(X.extractMin() != i) extractMin++;
    }
    //peakkey passed (Key:imeidj);
    printErrors("peakKey",peakKey);
    //extractMin passed (Key:imeidj);
    printErrors("extractMin",extractMin);           
        
    BHeap<int> C,D;
    for(int i = 0; i < 100000; i++){
        C.insert(i);
        D.insert(100000+i);
    }
    C.merge(D);
    peakKey=0;
    extractMin=0;
    for(int i = 0; i < 199999; i++){
        if(C.peekKey() != i) peakKey++;
        if(C.extractMin() != i) extractMin++;
    }
    //peakkey passed (Key:imeidj)
    printErrors("peakKey",peakKey);
    //extractMin passed (Key:imeidj)
    printErrors("extractMin",extractMin);
    return 0;
}

