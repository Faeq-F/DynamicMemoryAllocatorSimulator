An implementation of a model of a dynamic memory allocator, i.e., a program that simulates a dynamic memory allocator.

### Compilation

All programs are compiled using `gcc -Wall -Werror -Wpedantic` and run without errors. Possible execution issues were spotted with Valgrind and fixed; the end of the output is
```
== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### What I learnt

- how to declare, define, and update variables and functions in C
- how to use pointer arithmetic
- how a memory allocator works
- how to have user interaction with dynamic memory
- how to read from `stdin` and write to `stdout` using `getchar` and `printf`
- how to allocate and free memory using `malloc`
- how to handle null pointers and memory leaks
- how to write readable, well-formatted and well-commented code in C

### Steps

<details>
<summary>
See steps for implementation
</summary>

#### Introduction

<details>
<summary>
See introduction
</summary>
The process address space is represented by an array of characters, `memory`.

Given a size in bytes, `nbytes`, the allocator scans the array to find a free memory block of size `nbytes` and returns the index of the start of the block.

For simplicity, the program maintains an integer array of the same size as memory to store the sizes of the allocated blocks. The address space is represented by

```C
struct space {
	char *memory;
	int *sizes;
	int len;
};
```

where `memory` and `sizes` are pointers to the first entry of the character and integer arrays and `len` is their length.

In the dynamic version of the program, the program stores an arbitrary number of arbitrary sentences entered by a user on standard input. The size of the memory space grows when needed.

At the beginning, the allocator stores all blocks one after the other. When some blocks are freed, allocating new blocks at the end becomes inefficient. The allocator should search the space for the first block compatible with the required size.

The implementation follows the steps described in the next sections.

**Example**. If the initial size of the memory array is 10 and `input.txt` is used as an input, i.e. a text file containing the following lines

```
Brian Kernighan
CS2850
Dennis Ritchie
and
The C Programming Language
```

a run of the final program should print

```
0000000000
______________________________________________
0000000000000000000000000000000000000000000000
Brian Kernighan++_____________________________
17///////////////00000000000000000000000000000
Brian Kernighan++CS2850++_____________________
17///////////////8///////000000000000000000000
Brian Kernighan++________Dennis Ritchie++_____
17///////////////0000000016//////////////00000
Brian Kernighan++and++___Dennis Ritchie++_____
17///////////////5////00016//////////////00000
______________________________________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
Brian Kernighan++and++___Dennis Ritchie++The C Programming Language++_________________________
17///////////////5////00016//////////////28//////////////////////////0000000000000000000000000
______________________________________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

on standard output.

Note that the program automatically frees the sentence `CS2500`. The memory grows when the user enters the sentences `Brian Kerninghan` and `C Programming Language`. The allocator saves the sentence `and` in the first suitable available space.

To print `memory` and `sizes`, `printMemory` and `printSizes` are provided.

</details>

#### Step 1: Initialization

<details>
<summary>
See initialization
</summary>

`BUSY` and `FREE` are used to mark the allocated and free entries of `memory` and `BUSYSIZE` and `FREESIZE` to mark the allocated (non-starting) and free entries of `sizes`. The first entry of an allocated block is marked with the length of the block in `sizes`.

In this step, 2 functions are implemented;

1. `initializeMemory`, which accepts an integer `memSize` and a pointer to the memory struct, `mem`, as inputs, allocates two blocks of size `memSize * sizeof(char)` and `memSize * sizeof(int)` using `malloc`, makes `memory` and `sizes` point to these blocks, sets `len` to `memSize`, initializes the arrays by marking all entries with `FREE` and `FREESIZE`, and print `memory` and `sizes` by calling `printMemory` and `printSizes`.

2. `cleanMemory`, which accepts a pointer to the memory struct, `mem`, as an input, replace all entries of `memory` and `sizes` with `FREE` and `FREESIZE`, print `memory` and `sizes` by calling `printMemory` and `printSizes`, and free the allocated memory by calling `free`.

The implementation can be tested by compiling and running `step1.c`. The output should be

```
__________________________________________________
00000000000000000000000000000000000000000000000000
__________________________________________________
00000000000000000000000000000000000000000000000000
__________________________________________________
00000000000000000000000000000000000000000000000000
```

Before proceeding, make sure Valgrind does not detect execution errors or memory leaks.

</details>

#### Step 2: Stack Allocator

<details>
<summary>
See writing a stack-like allocator
</summary>

In this section, a stack-like allocator and de-allocator is implemented.

This is different from the heap-like allocator implemented in the next section because a new memory block is always allocated at the end of the previous one.

The deallocator takes the index of the first entry of the block to be deallocated and removes its content by setting the block entries back to `FREE` (in `memory`) and `FREE` (in `FREESIZE`).

In this step, 2 functions are implemented;

1. `int stackAllocator`, which accepts an integer `nbytes` and a pointer to the memory struct, `mem`, as inputs, searches for the first entry of `sizes` marked with `FREESIZE`, check whether there are `nbytes` available entries after it, writes `nbytes` on that entry of `sizes`, writes `BUSYSIZE` on the `nbytes − 1` following entries of `sizes`, finds the corresponding `nbytes` entries in `memory` and mark them with `BUSY`, and returns the index of the first entry of the newly allocated block

2. `void deallocator`, which accepts an integers, `p`, and a pointer to the memory struct, `mem`, as inputs, find the location corresponding to `p` in `sizes`, read the block length, `nbytes`, from that entry, and sets the `nbytes` entries of `memory` starting at `p` to `FREE` and the corresponding `nbytes` entries of `sizes` to `FREESIZE`.

If the first entry marked with `FREESIZE` in `sizes` is too close to the end of the array, i.e. if there are not `nbytes` entries of `sizes` marked with `FREESIZE` after it, `stackAllocator` should return `mem->len`. This allows you to check from the program if the memory has been successfully allocated before writing on it.

The deallocator should not return anything.

The implementation can be tested by compiling and running `step2.c`. The output should be

```
__________________________________________________
00000000000000000000000000000000000000000000000000
Brian Kernighan+++++______________________________
20//////////////////000000000000000000000000000000
Brian Kernighan+++++CS2850+++++___________________
20//////////////////11/////////0000000000000000000
Brian Kernighan+++++___________Dennis Ritchie+++++
20//////////////////0000000000019/////////////////
__________________________________________________
00000000000000000000000000000000000000000000000000
```

Check that the program behaves correctly by running the binary with Valgrind.

If you change `stackAllocator(stringLen(s[i]) + 5, &space)` to `stackAllocator(stringLen(s[i]) + 7, &space);` in `step2.c`, the last sentence, `Dennis Ritchie`, will not be allocated because there is not enough space, i.e. the output becomes

```
__________________________________________________
00000000000000000000000000000000000000000000000000
Brian Kernighan+++++++____________________________
22////////////////////0000000000000000000000000000
Brian Kernighan+++++++CS2850+++++++_______________
22////////////////////13///////////000000000000000
Brian Kernighan+++++++____________________________
22////////////////////0000000000000000000000000000
__________________________________________________
00000000000000000000000000000000000000000000000000
```
</details>

#### Step 3: Heap Allocator

<details>
<summary>
See writing a heap-like allocator
</summary>

In this section, the allocator will be rewritten to make it a heap-like allocator, i.e. to let the program allocate memory in the free space left by previous frees.

In this step, 2 functions are implemented;

1. `int spaceScanner`, which takes an integer, `nbytes`, and a pointer to the memory structure, `mem`, as inputs, finds the first entry of `sizes` that is marked with `FREESIZE` and has `nbytes` entries also marked with `FREESIZE` after it, and returns the index of the corresponding entry of `memory` if the search is successful and `mem->len` otherwise

2. `int heapAllocatorQ3`, which has the same inputs as `stackAllocation`, calls `spaceScanner` to obtain the start index, `p`, of a suitable block, sets the `nbytes` entries of `memory` after `p` to `FREE` and the corresponding `nbytes` entries of `sizes` to `FREESIZE`, and returns `p`.

The implementation can be tested by compiling and running `step3.c`, with `memSize` set to `50`. The output should be

```
00000000000000000000000000000000000000000000000000
Brian Kernighan++_________________________________
17///////////////000000000000000000000000000000000
Brian Kernighan++CS2850++_________________________
17///////////////8///////0000000000000000000000000
Brian Kernighan++________Dennis Ritchie++_________
17///////////////0000000016//////////////000000000
Brian Kernighan++________Dennis Ritchie++_________
17///////////////0000000016//////////////000000000
Brian Kernighan++and++___Dennis Ritchie++_________
17///////////////5////00016//////////////000000000
__________________________________________________
00000000000000000000000000000000000000000000000000
```

The sentence `and` is now allocated in the free space once occupied by the sentence `CS2850`. The sentence `The C Programming Language` is not allocated because the memory is not big enough. This problem will be fixed in the next section.

</details>

#### Step 4: User interactions

<details>
<summary>
See making the program interactive
</summary>

Before making the program interactive, a function that increases the size of the memory if needed is required. In a real process, this means requiring the intervention of the Operating System (OS) through a system call.

In this model, `mallloc` is used to simulate the OS intervention by reallocating `memory` and `size`. The input sentences also have to be parsed and saved into strings. As we do not want to restrict the length of a user sentence, we need to buffer it into a dynamically allocated string that grows as new characters arrive.

In this step, 2 functions are implemented;

1. `void increaseMemory` which takes an integer, `nbytes`, and a pointer to the memory structure, `mem`, as inputs, computes the length of the new memory, `newLen`, using

```C
int newLen = mem-->len;
while (newLen - mem->len < nbytes)
        newLen = 2 * (newLen + 1);
```

saves the content of `memory`, `sizes`, and `len` into three temporary variables, allocates a new memory space by calling `initializeMemory(newLen, mem)`, copies the content of the temporary variable into the newly initialized memory, and free the old memory using `free`


2. `int readString`, which takes a pointer to a string, i.e. a pointer to a pointer to a character, `s`, as an input, calls `malloc(1)` to allocate a starting string, stores it in `*s`, gets characters from the terminal by calling `getchar` until you reach a new line character or `EOF`, reallocates the string to accommodate each new character through

```C
len++;
*s = malloc(len + 1);
char *temp = *s;
copyString(temp, s, len);
free(temp);
```

where `len` is the length of the string before attaching the new character, stores the new character in the `len`-th entry of `*s`, `null` terminates the final string, and returns `1` if the last character is a new line character and `0` if the last character is `EOF`.

To copy the temporary integer array into the newly allocated size array,sizes, a function is written, `copyArray(int *old, int *new, int len)`, by adapting `copyString`. `heapAllocatorQ3` also needs to be modified. Instead of returning `mem->len` if `spaceScanner` does not find a suitable free block, the function should call `increaseMemory` until such a block becomes available.

For example, the early-return statement of the old implementation can be replaced with

```C
int t0;
while ((t0 = spaceScanner(nbytes, mem)) == mem->len) increaseMemory(nbytes, mem);
```

The implementation can be tested by compiling and running `step4.c`. The deallocator removes the second sentence of each group of three sentences after the user enters the third one. A sample output of this program is given in the Introduction.

</details>
</details>

### Test Cases

<details>
<summary>
See test cases
</summary>

#### Step 1

<details>
<summary>
See step 1 test cases
</summary>

Testing the implementations of
```
void initializeMemory(int len, struct space *mem)
void cleanMemory(struct space *mem)
```

<table>
   <tr>
      <th>Test</th>
      <th>Expected</th>
   </tr>
   <tr>
      <td>
         0
      </td>
      <td>
<pre>

</pre>
      </td>
   </tr>
   <tr>
      <td>
         1
      </td>
      <td>
<pre>
_
0
_
0
_
0
</pre>
      </td>
   </tr>
   <tr>
      <td>
         10
      </td>
      <td>
<pre>
__________
0000000000
__________
0000000000
__________
0000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         50
      </td>
      <td>
<pre>
__________________________________________________
00000000000000000000000000000000000000000000000000
__________________________________________________
00000000000000000000000000000000000000000000000000
__________________________________________________
00000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
</table>

</details>

#### Step 2

<details>
<summary>
See step 2 test cases
</summary>

Testing the implementations of
```
void initializeMemory(int len, struct space *mem)
void cleanMemory(struct space *mem)
int stackAllocator(int nbytes, struct space *mem)
void deallocator(int t0, struct space *mem)
```

<table>
   <tr>
      <th>Test</th>
      <th>Expected</th>
   </tr>
   <tr>
      <td>
         0
      </td>
      <td>
<pre>

</pre>
      </td>
   </tr>
   <tr>
      <td>
         1
      </td>
      <td>
<pre>
_
0
_
0
_
0
_
0
_
0
</pre>
      </td>
   </tr>
   <tr>
      <td>
         15
      </td>
      <td>
<pre>
_______________
000000000000000
_______________
000000000000000
CS2850+++++____
11/////////0000
_______________
000000000000000
_______________
000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         30
      </td>
      <td>
<pre>
______________________________
000000000000000000000000000000
Brian Kernighan+++++__________
20//////////////////0000000000
Brian Kernighan+++++__________
20//////////////////0000000000
Brian Kernighan+++++__________
20//////////////////0000000000
______________________________
000000000000000000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         70
      </td>
      <td>
<pre>
______________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000
Brian Kernighan+++++__________________________________________________
20//////////////////00000000000000000000000000000000000000000000000000
Brian Kernighan+++++CS2850+++++_______________________________________
20//////////////////11/////////000000000000000000000000000000000000000
Brian Kernighan+++++___________Dennis Ritchie+++++____________________
20//////////////////0000000000019/////////////////00000000000000000000
______________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
</table>

</details>

#### Step 3

<details>
<summary>
See step 3 test cases
</summary>

Testing the implementations of
```
void initializeMemory(int len, struct space *mem)
void cleanMemory(struct space *mem)
void deallocator(int t0, struct space *mem)
int spaceScanner(int nbytes, struct space *mem)
int heapAllocatorQ3(int nbytes, struct space *mem)
```

<table>
   <tr>
      <th>Test</th>
      <th>Expected</th>
   </tr>
   <tr>
      <td>
         0
      </td>
      <td>
<pre>

</pre>
      </td>
   </tr>
   <tr>
      <td>
         1
      </td>
      <td>
<pre>
_
0
_
0
_
0
_
0
_
0
_
0
_
0
</pre>
      </td>
   </tr>
   <tr>
      <td>
         15
      </td>
      <td>
<pre>
_______________
000000000000000
_______________
000000000000000
CS2850++_______
8///////0000000
CS2850++_______
8///////0000000
CS2850++_______
8///////0000000
CS2850++and++__
8///////5////00
_______________
000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         30
      </td>
      <td>
<pre>
______________________________
000000000000000000000000000000
Brian Kernighan++_____________
17///////////////0000000000000
Brian Kernighan++CS2850++_____
17///////////////8///////00000
Brian Kernighan++_____________
17///////////////0000000000000
Brian Kernighan++and++________
17///////////////5////00000000
______________________________
000000000000000000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         70
      </td>
      <td>
<pre>
______________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000
Brian Kernighan++_____________________________________________________
17///////////////00000000000000000000000000000000000000000000000000000
Brian Kernighan++CS2850++_____________________________________________
17///////////////8///////000000000000000000000000000000000000000000000
Brian Kernighan++________Dennis Ritchie++_____________________________
17///////////////0000000016//////////////00000000000000000000000000000
Brian Kernighan++________Dennis Ritchie++The C Programming Language++_
17///////////////0000000016//////////////28//////////////////////////0
Brian Kernighan++and++___Dennis Ritchie++The C Programming Language++_
17///////////////5////00016//////////////28//////////////////////////0
______________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
</table>

</details>

#### Step 4

<details>
<summary>
See step 4 test cases
</summary>

Testing the implementations of
```
void initializeMemory(int len, struct space *mem)
void cleanMemory(struct space *mem)
void deallocator(int t0, struct space *mem)
void copyArray(int *a, int *b, int len)
int spaceScanner(int nbytes, struct space *mem)
void increaseMemory(int nbytes. struct space *mem)
int heapAllocator(int nbytes, struct space *mem)
int readString(char **s)
```

<table>
   <tr>
      <th>Test</th>
      <th>Input</th>
      <th>Expected</th>
   </tr>
   <tr>
      <td>
         1
      </td>
      <td>
<pre>
the
starting size
of the memory
is one!
</pre>
      </td>
      <td>
<pre>
_
0
__________
0000000000
the++_____
5////00000
______________________________________________
0000000000000000000000000000000000000000000000
the++starting size++__________________________
5////15/////////////00000000000000000000000000
the++_______________of the memory++___________
5////00000000000000015/////////////00000000000
the++is one!++______of the memory++___________
5////9////////00000015/////////////00000000000
______________________________________________
0000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         2
      </td>
      <td>
<pre>
the starting size
of
the
memory
is<br />
2
!
</pre>
      </td>
      <td>
<pre>
__
00
______________________________
000000000000000000000000000000
the starting size ++__________
20//////////////////0000000000
the starting size ++of ++_____
20//////////////////5////00000
______________________________________________________________
00000000000000000000000000000000000000000000000000000000000000
the starting size ++_____the++________________________________
20//////////////////000005////00000000000000000000000000000000
the starting size ++_____the++memory++________________________
20//////////////////000005////8///////000000000000000000000000
the starting size ++_____the++memory++is ++___________________
20//////////////////000005////8///////5////0000000000000000000
the starting size ++2++__the++memory++is ++___________________
20//////////////////3//005////8///////5////0000000000000000000
the starting size ++2++__the++memory++is ++!++________________
20//////////////////3//005////8///////5////3//0000000000000000
______________________________________________________________
00000000000000000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         15
      </td>
      <td>
<pre>
t
h
e
s
t
a
r
t
i
n
g
size of the memory is 15!
</pre>
      </td>
      <td>
<pre>
_______________
000000000000000
t++____________
3//000000000000
t++h++_________
3//3//000000000
t++___e++______
3//0003//000000
t++___e++s++___
3//0003//3//000
________________________________
00000000000000000000000000000000
t++___e++s++t++_________________
3//0003//3//3//00000000000000000
t++___e++s++___a++______________
3//0003//3//0003//00000000000000
t++___e++s++___a++r++___________
3//0003//3//0003//3//00000000000
t++___e++s++___a++r++t++________
3//0003//3//0003//3//3//00000000
t++___e++s++___a++r++___i++n++__
3//0003//3//0003//3//0003//3//00
__________________________________________________________________
000000000000000000000000000000000000000000000000000000000000000000
t++___e++s++___a++r++___i++n++g++_________________________________
3//0003//3//0003//3//0003//3//3//000000000000000000000000000000000
t++___e++s++___a++r++___i++n++___size of the memory is 15!++______
3//0003//3//0003//3//0003//3//00027/////////////////////////000000
__________________________________________________________________
000000000000000000000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         30
      </td>
      <td>
<pre>
the starting size of the memory
is
t h i r t y<br />
(30)
</pre>
      </td>
      <td>
<pre>
______________________________
000000000000000000000000000000
______________________________________________________________________________________________________________________________
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
the starting size of the memory++_____________________________________________________________________________________________
33///////////////////////////////000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
the starting size of the memory++is++_________________________________________________________________________________________
33///////////////////////////////4///00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
the starting size of the memory++____t h i r t y++____________________________________________________________________________
33///////////////////////////////00013////////////0000000000000000000000000000000000000000000000000000000000000000000000000000
the starting size of the memory++____t h i r t y++(30)++______________________________________________________________________
33///////////////////////////////00013////////////6/////0000000000000000000000000000000000000000000000000000000000000000000000
______________________________________________________________________________________________________________________________
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
   <tr>
      <td>
         70
      </td>
      <td>
<pre>
the starting size of the memory is
....
...
..
.
seventy (70)
!
</pre>
      </td>
      <td>
<pre>
______________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000
the starting size of the memory is++__________________________________
36//////////////////////////////////0000000000000000000000000000000000
the starting size of the memory is++....++____________________________
36//////////////////////////////////6/////0000000000000000000000000000
the starting size of the memory is++______...++_______________________
36//////////////////////////////////0000005////00000000000000000000000
the starting size of the memory is++..++__...++_______________________
36//////////////////////////////////4///005////00000000000000000000000
the starting size of the memory is++..++__...++.++____________________
36//////////////////////////////////4///005////3//00000000000000000000
the starting size of the memory is++..++__...++___seventy (70)++______
36//////////////////////////////////4///005////00014////////////000000
the starting size of the memory is++..++__...++___seventy (70)++!++___
36//////////////////////////////////4///005////00014////////////3//000
______________________________________________________________________
0000000000000000000000000000000000000000000000000000000000000000000000
</pre>
      </td>
   </tr>
</table>

</details>

</details>

### Pseudocode

<details>
<summary>
See the pseudocode of the functions implemented
</summary>

```C
function void initializeMemory(int len, struct space *mem)
  let mem->memory point to a dynamically allocated array of length len* sizeof(char)
	let mem->sizes point to a dynamically allocated array of length len * sizeof(int)
	set mem->len to len
	i <- 0
	while i is smaller than mem->len do
		set the i-th entry of mem->memory to FREE
		set the i-th entry of mem->sizes to FREESIZE
		i <- i + 1
	print mem->memory using printMemory
	print mem->sizes using printSizes
```

```C
function void cleanMemory(struct space *mem)
	i <- 0
	while i is smaller than mem->len do
		set the i-th entry of mem->memory to FREE
		set the i-th entry of mem->sizes to FREESIZE
		i <- i + 1
	print mem->memory using printMemory
	print mem->sizes using printSizes
	free mem->memory
	free mem->sizes
```

```C
function int stackAllocator(int nbytes, struct space *mem)
	t0 <- 0
	while t0 + nbytes is not greater than mem->len and the t0-th entry of mem->sizes is not FREESIZE do
		t0 <- t0 + 1
	if t0 + nbytes is larger than mem->len then
		return mem->len
	t <- 0
	while t is smaller than nbytes and t0 + t is smaller than mem->len do
		set the (t0 + t)-th entry of mem->memory to BUSY
		set the (t0 +t)-th entry of mem->sizes to BUSYSIZE
		t <- t + 1
	set the t0-th entry of mem->sizes to nbytes
	return t0
```

```C
function void deallocator(t0, struct space *mem)
	if t0 equals mem->len or is negative then
		return
	let nbytes be the value stored in the t0-th location of mem->sizes
	t <- 0
	while t is smaller than nbytes do
		set the (t0 + t)-th entry of mem->memory to FREE
		set the (t0 + t)-th entry of mem->sizes to FREESIZE
		t <- t + 1
```

```C
function int spaceScanner(int nbytes, struct space *mem)
  t0 <- 0
  s <- 0
  while s=0 and t0 is smaller than mem->len do
    t <- 0
    while t0 + t is not greater than mem->len and the (t0 + t)-th entry of mem->sizes is FREESIZE do
      t <- t+1
    if t is larger than or equal to nbytes then
      s <- 1
    else
      t0 <- t0 + 1
  return t0
```

```C
function int heapAllocatorQ3(int nbytes, struct space *mem)
  t0 <- spaceScanner(nbytes, mem)
  if t0 == mem->len then
    return t0
  t <- 0
  while t is smaller than nbytes do
    set the (t0 +t)-th entry of mem->memory to BUSY
    set the (t0 +t)-th entry of mem->sizes to BUSYSIZE
    t <- t + 1
  set the t0-th entry of mem->sizes to nbytes
  return t0
```

```C
function void increaseMemory(int nbytes, struct space *mem)
  t <- mem->len
  while the difference betweent and mem->len is smaller than nbytes do
    t <- 2(t + 1)
  let s be a pointer to char
  s <- mem->memory
  let a be a pointer to int
  a <- mem->sizes
  ℓ <- mem->len
  call initializeMemory with arguments t and mem
  copy the content of the character array pointed by s into mem->memory using copyString
  copy the content of the integer array pointed by a into mem->sizes using copyArray
  free s and a
```

```C
function int heapAllocator(int nbytes, struct space *mem)
  t0 <- spaceScanner(nbytes, mem)
  while t0 equals mem->len do
    increaseMemory(nbytes, mem)
    t0 <- spaceScanner(nbytes, mem)
  t <- 0
  while t is smaller than nbytes do
    set the (t0 + t)-th entry of mem->memory to BUSY
    set the (t0 + t)-th entry of mem->sizes to BUSYSIZE
    t <- t + 1
  set the t0-th entry of mem->sizes to nbytes
  return t0
```

```C
function int readString(char **s)
  t <- 0
  c <- getchar()
  allocate 1 byte in the heap using malloc
  let *s point to the newly allocated memory
  let **s be the null character
  while c is not a new line character or EOF do
    p <- *s
    t <- t + 1
    allocate a character array of t + 1 bytes in the heap using malloc
    let *s point to the newly allocated memory
    copy the content of the character array pointed by p into ∗s
    free p
    set the t-th entry of the newly allocated character array to c
    set the (t + 1)-th entry of the newly allocated character array to the null character
    c <- getchar()
  if c is EOF then
    return 0
  return 1
```
</details>