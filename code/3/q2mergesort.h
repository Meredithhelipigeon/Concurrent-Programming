#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif

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
        public:
            T * data;
            T * copy;
            int start;
            int end;
            int depth;
            StartMsg(T * data, T * copy, int start, int end, int depth): 
                data{data}, copy{copy}, start{start}, end{end}, depth{depth} {}
            StartMsg() {}
	};

    _Actor

    #elif defined( TASK )

    _Task

    #endif
            Mergesort {
                #if defined( CFOR )
                public: 
                    static 
                #endif 
                void sort(T data[], T copy[], unsigned int left, unsigned int right, unsigned int mid){
                        int leftIndex = 0;
                        int rightIndex = 0;
                        int mergeIndex = left;
                        int leftSize = mid - left + 1;
                        int rightSize = right - mid;
                    
                        // Copy data to temp arrays data
                        for(auto i = left; i <= right; i++){
                            copy[i] = data[i];
                        }
                    
                        // Merge 2 arrays(from copy now) into data
                        while (leftIndex < leftSize && rightIndex < rightSize) {
                            // Always choose the smaller one to copy
                            if (copy[left+leftIndex] <= copy[mid+1+rightIndex]) {
                                data[mergeIndex]  = copy[left+leftIndex];
                                leftIndex++;
                            } else {
                                data[mergeIndex] = copy[mid+1+rightIndex];
                                rightIndex++;
                            }
                            mergeIndex++;
                        }
                        
                        // If right array has finished
                        if (rightIndex==rightSize){
                            while (leftIndex < leftSize) {
                                data[mergeIndex]
                                    = copy[leftIndex+left];
                                leftIndex++;
                                mergeIndex++;
                            }
                        } else {
                            while (rightIndex < rightSize) {
                                data[mergeIndex] = copy[mid+1+rightIndex];
                                rightIndex++;
                                mergeIndex++;
                            }

                        }
                    }
                #if defined (ACTOR)
                uActor * parent;
                int liveNum = 2; // live children
                bool ifLive = true;
                StartMsg leftMsg;
                StartMsg rightMsg;
                T * data;
                T * copy;
                int start;
                int end;
                int depth;
                #elif defined( TASK )
                T *data;
                T *copy;
                int start;
                int end;
                int depth;
                void main() {
                    merge(data, copy, start, end, depth);
                }
                public: 
                    Mergesort(T *data, T *copy, int start, int end, int depth) : data{data}, copy{copy}, start{start}, end{end}, depth{depth} {}
                #endif

                #if defined( CFOR )
                static 
                #endif

                void merge( T data[], T copy[], unsigned int start, unsigned int end, int depth ) {
                    unsigned int mid = (start+end)/2;
                    #if defined (CFOR)
                    if (start >= end){
                        return;
                    }
                    if (depth<=0){
                        merge(data, copy, start, mid, depth-1);
                        merge(data, copy, mid+1, end, depth-1);
                    } else {
                        COBEGIN
                            BEGIN merge(data, copy, start, mid, depth-1); END
                            BEGIN merge(data, copy, mid+1, end, depth-1); END
                        COEND // wait for return of recursive call
                    }
                    sort(data, copy, start, end, mid);
                    #elif defined( ACTOR )
                    if (start >= end){
                        ifLive = false;
                        if ((parent!=NULL) && (depth>0)){
                            *parent | uActor::stopMsg;
                        }
                        return;
                    }
                    if (depth<=0){
                        merge(data, copy, start, mid, depth-1);
                        merge(data, copy, mid+1, end, depth-1);
                        sort(data, copy, start, end, mid);
                    } else {
                        // create left and right, and send messages
                        leftMsg = StartMsg(data, copy, start, mid, depth-1);
                        rightMsg = StartMsg(data, copy, mid+1, end, depth-1);
                        *new Mergesort() | leftMsg;
                        *new Mergesort() | rightMsg;
                    }
                    #elif ( TASK )
                    if (start >= end){
                        return;
                    }
                    if (depth<=0) {
                        merge(data, copy, start, mid, depth-1);
                        merge(data, copy, mid+1, end, depth-1);
                    } else {
                        Mergesort leftMergeSort(data, copy, start, mid, depth-1);
                        Mergesort rightMergeSort(data, copy, mid+1, end, depth-1);
                    } // the magic bracket will help to run the main and destroy in the end
                    sort(data, copy, start, end, mid);
                    #endif
                }

                #if defined( ACTOR )
                uActor::Allocation receive( uActor::Message & message){
                    Case (StartMsg, message) {
                        // initialize the data information
                        parent = message_d->sender();
                        data = message_d->data;
                        copy = message_d->copy;
                        start = message_d->start;
                        end = message_d->end;
                        depth = message_d->depth;
                        merge(data, copy, start, end, depth);
                        if ((message_d->depth<=0) || (!ifLive)){
                            if ((parent!=NULL) && (message_d->depth<=0)){
                                *parent | uActor::stopMsg;
                            }
                            return uActor::Delete;
                        }
                    } else Case(uActor::StopMsg, message){
                        liveNum -= 1;
                        // when there is no children
                        if (liveNum==0){
                            sort(data, copy, start, end, (start+end)/2);
                            if (parent!=NULL){
                                *parent | uActor::stopMsg;
                            }
                            return uActor::Delete;
                        }
                    }
                    return uActor::Nodelete;
                }
                #endif
            };
    
    if (size>0){
        #if defined( CFOR )
        Mergesort::merge(data, copy, 0, size-1, depth);
        #elif ( ACTOR )
        uActor::start();
        StartMsg start(data, copy, 0, size-1, depth);
        *new Mergesort() | start;
        uActor::stop();
        #elif defined( TASK ) 
        {
            Mergesort root(data, copy, 0, size-1, depth);
        }
        #endif
    }

    delete [] copy;
} // mergesort
