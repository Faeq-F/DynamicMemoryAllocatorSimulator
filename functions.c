#include "auxiliary functions.c"

void copyArray(int* old, int* new, int len) {
    for (int i = 0; i < len; i++) {
        new[i] = old[i];
    }
}

void initializeMemory(int len, struct space *mem){
	
	mem->memory = (char*)malloc(len * sizeof(char));
    mem->sizes = (int*)malloc(len * sizeof(int));
    mem->len = len;

	// Initialize memory and sizes arrays
	for (int i = 0; i < mem->len; i++) {
        	mem->memory[i] = FREE;
	        mem->sizes[i] = FREESIZE;
	}

	printMemory(mem);
	printSizes(mem);
}

void cleanMemory(struct space* mem) {
    
    // Clear memory and sizes arrays
    for (int i = 0; i < mem->len; i++) {
        mem->memory[i] = FREE;
        mem->sizes[i] = FREESIZE;
    }

    printMemory(mem);
    printSizes(mem);

    free(mem->memory);
    free(mem->sizes);
}

int stackAllocator(int nbytes, struct space *mem) {
    
    int t0 = 0;

    // Search for the first entry with enough space
    while ((t0 + nbytes) <= mem->len && mem->sizes[t0] != FREESIZE) {
        t0++;
    }

    // Check if there is not enough space for allocation
    if ((t0 + nbytes) >= mem->len) {
        return mem->len; // Unsuccessful allocation
    }

    // Fill the allocated memory in memory and sizes arrays
    int t = 0;
    while (t < nbytes && (t0 + t) < mem->len) {
        mem->memory[t0 + t] = BUSY;
        mem->sizes[t0 + t] = BUSYSIZE;
        t++;
    }

    mem->sizes[t0] = nbytes;
    return t0; // Index of the first entry of the newly allocated block
}

void deAllocator(int t0, struct space *mem) {
    
    // Check if the index is invalid
    if (t0 == mem->len || t0 < 0) {
        return;
    }

    int nbytes = mem->sizes[t0];
    int t = 0;

    // Clean the correct part of the memory and sizes arrays
    while (t < nbytes) {
        mem->memory[t0 + t] = FREE;
        mem->sizes[t0 + t] = FREESIZE;
        t++;
    }
}

int spaceScanner(int nbytes, struct space* mem) {
    
    int t0 = 0;
    
    // Loop through the memory to find a free block
    while (t0 < mem->len) {
        int t = 0;
        
        // Search for a block of the correct size
        while ((t0 + t) < mem->len && mem->sizes[t0 + t] == FREESIZE) {
            t++;
            
            // Check if the block is large enough
            if (t > nbytes) {
                return t0;
            }
        }
        t0++;
    }
    return mem->len; // Large enough block not found
}

void increaseMemory(int nbytes, struct space* mem) {
    
    int t = mem->len;

    // Calculate the new length for memory
    while ((t - mem->len) < nbytes) {
        t = 2 * (t + 1);
    }

    // Temporary fields
    char* s = mem->memory;
    int* a = mem->sizes;
    int l = mem->len;

    // Initialize a new memory space
    struct space temp;
    initializeMemory(t, &temp);

    // Copy the old memory into the new memory
    copyString(s, temp.memory, l);
    copyArray(a, temp.sizes, l);

    free(s);
    free(a);

    *mem = temp;
}

int readString(char** s) {
    
    int t = 0;
    int c = getchar();
    *s = (char*)malloc(1); // Allocate memory for the initial string
    **s = '\0';

    while (c != '\n' && c != EOF) {
        char* p = *s;
        t++;

        // Allocate memory for extra chars
        *s = (char*)malloc(t + 1);
        copyString(p, *s, t - 1);
        free(p);

        // Store new char in the string
        (*s)[t - 1] = (char)c;
        (*s)[t] = '\0';

        c = getchar();
    }

    return (c == '\n') ? 1 : 0;
}

int heapAllocator(int nbytes, struct space* mem) {

    // Search for a block with enough space
    int t0 = spaceScanner(nbytes, mem);

    // Increase memory if a block with enough space is not found
    while (t0 == mem->len) {
        increaseMemory(nbytes, mem);
        t0 = spaceScanner(nbytes, mem);
    }

    int t = 0;
    // Fill the allocated memory in memory and sizes arrays
    while (t < nbytes && (t0 + t) < mem->len) {
        mem->memory[t0 + t] = BUSY;
        mem->sizes[t0 + t] = BUSYSIZE;
        t++;
    }

    mem->sizes[t0] = nbytes;

    return t0; // Index of the first entry of the newly allocated block
}

//from step 3 - modified to become the above for step 4
int heapAllocatorQ3(int nbytes, struct space* mem) {
    
    // Search for a block with enough space
    int t0 = spaceScanner(nbytes, mem);

    if (t0 == mem->len) {
        return t0; // A block with enough space is not found
    }

    int t = 0;
    // Fill the allocated memory in memory and sizes arrays
    while (t < nbytes && (t0 + t) < mem->len) {
        mem->memory[t0 + t] = BUSY;
        mem->sizes[t0 + t] = BUSYSIZE;
        t++;
    }

    mem->sizes[t0] = nbytes;

    return t0; // Index of the first entry of the newly allocated block
}
