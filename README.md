# Arena_Allocator 

Instructions: (Run the code):

make clean <br />
make <br />
./unit_test <br />

This program is written to allocate memory using malloc in a linked list. <br />

![image](https://user-images.githubusercontent.com/60457052/168406260-d5ed7101-3e28-40c2-83a5-5e584558be33.png)

While this implementation works fine for most applications with performance needs, <br />
e.g. games, aircraft, spacecraft, etc. but can not afford to make a system call every time an allocation is needed. <br /> 
In these cases a large memory pool or arena is allocated on application startup. <br />
The application then manages memory on its own by implementing its own allocators to handle requests. <br />

This memory arena allocator supports First Fit, Best Fit, Worst Fit, and Next Fit alogritms. <br />

Memory Allocator API: <br />

The four functions used in this API are <br />

int mavalloc_init( size_t size, enum ALGORITHM algorithm ) - This function will use malloc to allocate a pool of memory that is size bytes big. <br />
If the size parameter is less than zero then return -1. If allocation fails return -1. If the allocation succeeds return 0. <br />

Where size is 4 byte aligned and the second parameter will set which algorithm you will use to allocate memory from your pool. <br />
The enumerated value is: enum ALGORITHM <br />
{ <br />
FIRST_FIT = 0, <br />
NEXT_FIT, <br />
BEST_FIT, <br />
WORST_FIT <br />
}; <br />

void *mavalloc_alloc( size_t size ) <br />

This function will allocate size bytes from your preallocated memory arena using the heap allocation algorithm that was specified during mavalloc_init. <br /> 
This function returns a pointer to the memory on success and NULL on failure. <br />

void mavalloc_free( void * pointer ) <br />
This function will free the block pointed to by the pointer back to your preallocated memory arena. <br />
This function returns no value. If there are two consecutive blocks free then combine (coalesce) them.  <br />

void mavalloc_destroy() <br />
This function will free the allocated arena and empty the linked list. <br />

int mavalloc_size() <br />
This function will return the number of nodes in the memory area. <br />


Tracking Memory: <br />
To maintain a linked list of allocated and free memory segments of your memory pool, where a segment is either allocated to a process or is an empty hole <br />
between two allocations. Each entry in the list specifies a hole (H) or process allocation (P), the address at which it starts, <br /> 
the length, and a pointer to the next item. <br />

Figure 1: ![image](https://user-images.githubusercontent.com/60457052/168406245-ca608a5d-c2d5-401c-ba21-1f3a14182167.png) <br /> 
Figure 2: ![image](https://user-images.githubusercontent.com/60457052/168406363-cfc714a8-c1ed-4f06-9256-568108d80672.png) <br />

For example, after the user initializes your memory allocator with: <br />
mavalloc_init( 65535, FIRST_FIT ); <br />

Then you will have a single node in your linked list specifying a hole, at the starting address, a length of 65535, and a next pointer of NULL. <br />
If the user then requests a memory allocation as: <br />
mavalloc_alloc( 5000 ); <br />

you will then have a linked list with the first node specifying a process allocation (P), a pointer to the starting address, a length of 5000,and a next pointer <br /> pointing to the next node which is a hole (H), pointing to the starting address plus 5000, a length of 60535, and a next pointer of NULL. <br />

Figure 1: <br />
Your list will be kept sorted by address. Sorting this way has the advantage that when a allocation is released (terminated in figure 1 terminology), <br /> 
updating the list is straightforward. An allocated block of memory normally has two neighbors (except when it is at the very top or bottom of memory). <br /> 
These may be either allocations or holes, leading to the four combinations shown in Fig. 2. <br /> 

In Fig. 1(a) updating the list requires replacing a P by an H. 
In Fig. 2(b) and Fig. 2(c), two entries are coalesced into one, and the list becomes one entry shorter. <br />  
In Fig. 2(d), three entries are merged and two items are removed from the list. <br />

It may be more convenient to have the list as a double-linked list, rather than the single linked list. <br />
This structure makes it easier to find the previous entry and to see if a merge is possible. You may use either a single or doubly linked list. <br />





