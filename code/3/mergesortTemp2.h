#include <iostream>
using namespace std;

template< typename T > void mergesort( T data[], unsigned int size, unsigned int depth ) {
    _Task Mergesort {
        T *data;
        T *copy;
        int start;
        int end;
        int depth;
        void sort(unsigned int left, unsigned int right, unsigned int mid){
            int subArrayOne = mid - left + 1;
            int subArrayTwo = right - mid;
        
            // Copy data to temp arrays data
            for(auto i = left; i <= right; i++){
                copy[i] = data[i];
            }
        
            auto indexOfSubArrayOne
                = 0, // Initial index of first sub-array
                indexOfSubArrayTwo
                = 0; // Initial index of second sub-array
            int indexOfMergedArray
                = left; // Initial index of merged array
        
            // Merge the temp arrays back into array[left..right]
            while (indexOfSubArrayOne < subArrayOne
                && indexOfSubArrayTwo < subArrayTwo) {
                if (copy[left+indexOfSubArrayOne]
                    <= copy[mid+1+indexOfSubArrayTwo]) {
                    data[indexOfMergedArray]
                        = copy[left+indexOfSubArrayOne];
                    indexOfSubArrayOne++;
                }
                else {
                    data[indexOfMergedArray]
                        = copy[mid+1+indexOfSubArrayTwo];
                    indexOfSubArrayTwo++;
                }
                indexOfMergedArray++;
            }
            // Copy the remaining elements of
            // left[], if there are any
            while (indexOfSubArrayOne < subArrayOne) {
                data[indexOfMergedArray]
                    = copy[indexOfSubArrayOne+left];
                indexOfSubArrayOne++;
                indexOfMergedArray++;
            }
            // Copy the remaining elements of
            // right[], if there are any
            while (indexOfSubArrayTwo < subArrayTwo) {
                data[indexOfMergedArray]
                    = copy[mid+1+indexOfSubArrayTwo];
                indexOfSubArrayTwo++;
                indexOfMergedArray++;
            }
        }

        void merge( unsigned int start, unsigned int end, int depth ) {
            cout << "start here " << start  << " " << end << " " << depth << endl;
            if (start >= end){
                return;
            }
            unsigned int mid = (start+end)/2;
            merge(start, mid, depth-1);
            merge(mid+1, end, depth-1);
            sort(start, end, mid);
            cout << "end here " << start << " " << mid << " " << end << " " << depth << endl;
        }
        void main() {
            cout << start << " " << end << " " << depth << endl;
            merge(start, end, depth);
            cout << "finish main function" << endl;
            for(int i = start; i <= end; i++){
                cout << data[i] << " ";
            }
            cout << endl;
        }
        public: 
            Mergesort(T data [], T copy [], int start, int end, int depth) : data{data}, copy{copy}, start{start}, end{end}, depth{depth} {}
            ~Mergesort(){
            }
    };
    // copy the original array to a copy arrary for temporary copying use in mergesort
    // T* copy = new T(size);
    T *copy{ new T[size]{} };
    for( unsigned int i = 0; i < size; i += 1){
        copy[i] = data[i];
    }
    
    Mergesort * root = new Mergesort(data, copy, 0, size-1, 0);
    cout << "finish task" << endl;
    delete root;
    delete [] copy;
} // mergesort
