#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif
#include <iostream>
using namespace std;

template< typename T > void mergesort( T data[], unsigned int size, unsigned int depth ) {
    // copy the original array to a copy arrary for temporary copying use in mergesort
    T *copy{ new T[size]{} };
    for( unsigned int i = 0; i < size; i += 1){
        copy[i] = data[i];
    }

    #if defined( CFOR )

    class

    #elif defined( ACTOR )

	struct StartMsg : public uActor::SenderMsg {
	}; // StartMsg

    _Actor

    #elif defined( TASK ) // TASK
    cout << "start creating task function" << endl;

    _Task

    #endif

        Mergesort {
            #if defined( CFOR )
            public: 
            #endif
                #if defined( CFOR )
                static 
                #endif 
                void sort(T data[], T copy[], unsigned int left, unsigned int right, unsigned int mid){
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
                
                #if defined( CFOR )
                static 
                #endif

                void merge( T data[], T copy[], unsigned int start, unsigned int end, int depth ) {
                    if (start >= end){
                        return;
                    }
                    cout << start << " " << end << " " << depth << endl;
                    unsigned int mid = (start+end)/2;
                    #if defined( CFOR )
                    cout << "cfor beigns" << endl;
                    if (depth<=0){
                        merge(data, copy, start, mid, depth-1);
                        merge(data, copy, mid+1, end, depth-1);
                    } else {
                        COBEGIN
                            BEGIN merge(data, copy, start, mid, depth-1); END
                            BEGIN merge(data, copy, mid+1, end, depth-1); END
                        COEND // wait for return of recursive call
                    }
                    #elif defined( ACTOR )
                    #elif defined( TASK ) // TASK
                    if (depth<=0) {
                        merge(data, copy, start, mid, depth-1);
                        merge(data, copy, mid+1, end, depth-1);
                    } else {
                        Mergesort leftMergeSort(data, copy, start, mid, depth-1);
                        Mergesort rightMergeSort(data, copy, mid+1, end, depth-1);
                    }
                    #endif
                    sort(data, copy, start, end, mid);
                    cout << "end here " << start << " " << end << " " << depth << endl;
                }

                #if defined( TASK )
                T *data;
                T *copy;
                int start;
                int end;
                int depth;
                void main() {
                    cout << start << " " << end << " " << depth << endl;
                    merge(data, copy, start, end, depth);
                    cout << "finish main function" << endl;
                }
                public: 
                    Mergesort(T *data, T *copy, int start, int end, int depth) : data{data}, copy{copy}, start{start}, end{end}, depth{depth} { 

                    }
                #endif
        };
    #if defined( CFOR )
    Mergesort::merge(data, copy, 0, size-1, depth);
    #elif defined( ACTOR )
    #elif defined( TASK ) // TASK
    cout << size <<  " " << depth << endl;
    {
        Mergesort root(data, copy, 0, size-1, depth);
    }
    #endif

    delete [] copy;
} // mergesort
