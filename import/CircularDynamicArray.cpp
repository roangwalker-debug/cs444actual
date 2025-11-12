#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

template <typename elmtype>
class CircularDynamicArray {
private:
	elmtype *array;
	int cap;
	int size;
	int front;
	elmtype error = NULL;
public:
	CircularDynamicArray() : cap(2), size(0), front(0) {
		array = new elmtype[cap];
	}

	CircularDynamicArray(int s) : cap(s), size(s), front(0) {
		array = new elmtype[cap];
	}

	~CircularDynamicArray() {
		delete[] array;
	}
	CircularDynamicArray(const CircularDynamicArray &old) {
		array = new elmtype[old.cap];
		cap = old.cap;
		size = old.size;
		front = old.front;
		for (int i = 0; i < cap; i++) {
			array[(front + i) % cap] = old.array[i];
		}
	}
	CircularDynamicArray& operator=(const CircularDynamicArray& rhs) {
		if(this != &rhs) {
			delete[] array;
			cap = rhs.cap;
			size = rhs.size;
			front = rhs.front;
			array = new elmtype[cap];
			
			for (int i = 0; i < cap; i++) {
				array[(front + i) % cap] = rhs.array[(rhs.front + i) % rhs.cap];
			}
		}
		return *this;
	}

	elmtype& operator[](int i) {
		if (i < 0 || i >= size) {
			cout << "Invalid Index: out of bounds." << endl;
			return error;
		}
		return array[(front + i) % cap];
	}

	void addEnd(elmtype v) {
		if (size == cap) {
			grow();
		}
		array[(front + size) % cap] = v;
		size++;
	}
	void addFront (elmtype v) {
		if (size == cap) {
			grow();
		}
		front = (front - 1 + cap) % cap;
		array[front] = v;
		size++;	
	
	}
	void delEnd() {
		size--;
		if (size <= cap / 4) {
			shrink();
		}
	
	}
	void delFront() {
		size--;
		if (size <= cap / 4) {
			shrink();
		}
		front = (front + 1) % cap;
	}
	
	int length() const {
		return size;
	}
	int capacity() const {
		return cap;
	}

	void clear() {
		delete[] array;
		cap = 2;
		size = 0;
		front = 0;
		array = new elmtype[cap];
	}

	void Sort() {
		elmtype* index = new elmtype[cap];
		mergeSort(array, index, 0, size - 1);
		delete[] index;
		
	}
	elmtype QSelect(int k) {
		return quick(0, size - 1, k);
	}
	int linearSearch(elmtype e) {
		for (int i = 0; i < size; i++) {
			if (array[(front + i) % cap] == e) {
				return i;
			}
		}
		return -1;
	}
	int binSearch(elmtype e) {
		int left = front;
		int currSize = size;
		while (currSize >= 1) {
			int mid = (left + (currSize / 2)) % cap;
			if (array[mid] == e) {
				return (((mid + cap) - front) % cap);
			}
			if (array[mid] < e) {
				left = (mid + 1) % cap;
			}
			
			currSize /= 2;
		}
		return -1;
	}
			
private:
	void grow() {
		cap *= 2;
		elmtype *grown = new elmtype[cap];

		for (int i = 0; i < size; i++) {
			grown[i] = array[(front + i) % (cap / 2)];
		}
		delete[] array;
		array = grown;
		front = 0;
	}
	void shrink() {
		cap /= 2;
		elmtype *shrunk = new elmtype[cap];

		for (int i = 0; i < size; i++) {
			shrunk[i] = array[i];
		}
		delete[] array;
		array = shrunk;
		front = 0;
	}
	
	void mergeSort(elmtype* arr, elmtype* index, int i, int k) {
		int j;
		if (i < k) {
			j = i + (k - i) / 2;
			mergeSort(arr, index, i, j);
			mergeSort(arr, index, j + 1, k);

			Merge(arr, index, i, j, k);
		}
	}
	void Merge(elmtype* arr, elmtype* index, int i, int j, int k) {
		int leftPos = i;
		int rightPos = j + 1;
		int pos = i;
	
		while (leftPos <= j && rightPos <= k) {
			if (arr[(front + leftPos) % cap] <= arr[(front + rightPos) % cap]) {
				index[pos++] = arr[(front + leftPos++) % cap];
			} else {
				index[pos++] = arr[(front + rightPos++) % cap];
			}
		}
		while (leftPos <= j) {
			index[pos] = arr[(front + leftPos) % cap];
			pos++;
			leftPos++;
		}
		while (rightPos <= k) {
			index[pos] = arr[(front + rightPos) % cap];
			pos++;
			rightPos++;
		}
		
		for (int x = i; x <= k; x++) {
			arr[(front + x) % cap] = index[x];
		}
	}
	elmtype quick(int left, int right, int k) {
		int pivot = partition(left, right);
		if (pivot == (k - 1)) {
			return array[(front + pivot) % cap];
		} else if (pivot < (k - 1)) {
			return quick(pivot + 1, right, k);
		} else {
			return quick(left, pivot - 1, k);
		}
		return array[(front + left) % cap];
	}
	int partition(int left, int right) {
		int pivot = rand() % (right - left + 1) + left;
		elmtype piv = array[(front + pivot) % cap];
		int x = left - 1;
		swap(array[(front + pivot) % cap], array[(front + right) % cap]);

		for (int y = left; y < right; y++) {
			if (array[(front + y) % cap] <= piv) {
				x++;
				swap(array[(front + x) % cap], array[(front + y) % cap]);
			}
		}
		swap(array[(front + x + 1) % cap], array[(front + right) % cap]);
		return x + 1;
	}
};
