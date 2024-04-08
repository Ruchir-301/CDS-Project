#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

struct ijit_code;

/** This function pointer runs the jit'd code */
typedef void (*ijit_interpreter)(struct ijit_code *code);

/** This struct stores a block of jit code */
struct ijit_code {
    char name[32]; /* run name */
    char jitted[1024]; /* machine code to run */
    ijit_interpreter interp; /* fallback interpreter function */
};

int main() {
    /* Allocate executable area to store JIT code: */
    struct ijit_code *code=(struct ijit_code *)mmap(
        (void *)0x3badc000, /* pointer to base address of new jit_code structure */
        4096, /* bytes of code to run */
        PROT_READ|PROT_WRITE|PROT_EXEC, /* allow anything: RWX */
        MAP_ANONYMOUS|MAP_SHARED|MAP_FIXED, /* allocate memory where I say */
        -1, 0 /* file descriptor and offset not used. */
    );
    if (code==(struct ijit_code *)-1) {
        perror("mmap"); exit(1);
    }

    /* Set up structure */
    code->interp=0;
    printf("Loading name into code struct at %p...\n",(void *)code);

    /* Prompt user to enter name */
    printf("Enter the name (max 31 characters): ");
    fflush(stdout);

    /* Read name with proper bounds checking */
    if (1 != scanf("%31s", code->name)) {  // Limiting input to 31 characters
        printf("ERROR in scanf. Exiting.\n");
        exit(1);
    }
    printf("Loaded name %s (%zd bytes)\n", code->name, strlen(code->name));

    /* Run the code */
    if (code->interp) {
        printf("Seems to be an interpreter at %p: running it...\n",(void *)code->interp);
        code->interp(code);
        printf("Back from interpreter.\n");
    } else {
        printf("No interpreter found. Exiting.\n");
    }
    return 0;
}
