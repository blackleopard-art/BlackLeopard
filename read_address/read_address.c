#include <stdio.h>

#include <ptedit_header.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

#define TAG_OK COLOR_GREEN "[+]" COLOR_RESET " "
#define TAG_FAIL COLOR_RED "[-]" COLOR_RESET " "
#define TAG_PROGRESS COLOR_YELLOW "[~]" COLOR_RESET " "

//unsigned long target;//DEFAULT USE

int is_normal_page(size_t entry) {
#if defined(__i386__) || defined(__x86_64__)
  return !(entry & (1ull << PTEDIT_PAGE_BIT_PSE));
#elif defined(__aarch64__)
  return 1;
#endif
}

int main(int argc, char *argv[]) {
    if (ptedit_init()) {
      printf(TAG_FAIL "Error: Could not initalize PTEditor, did you load the kernel module?\n");
      return 1;
    }

    void *target;
    int target_pid;
    printf("Enter the address of the target: ");
    scanf("%p", &target);
    printf("\nEnter the pid of the target: ");
    scanf("%d", &target_pid);

    size_t phys = 0;

    ptedit_entry_t entry = ptedit_resolve(target, target_pid);
    //ptedit_entry_t entry = ptedit_resolve(&target, 0);//DEFAULT USE

    if(is_normal_page(entry.pd)) {
        printf(TAG_PROGRESS "Page is 4KB\n");
        ptedit_print_entry(entry.pte);
        //phys = (ptedit_get_pfn(entry.pte) << 12) | (((size_t)&target) & 0xfff);//DEFAULT USE
        phys = (ptedit_get_pfn(entry.pte) << 12) | (((size_t)target) & 0xfff);
    } else {
        printf(TAG_PROGRESS "Page is 2MB\n");
        ptedit_print_entry(entry.pd);
        //phys = (ptedit_get_pfn(entry.pd) << 21) | (((size_t)&target) & 0x1fffff);//DEFAULT USE
        phys = (ptedit_get_pfn(entry.pd) << 21) | (((size_t)target) & 0x1fffff);
    }

    printf(TAG_OK "Virtual address: %p\n", target);
    printf(TAG_OK "Physical address: 0x%zx\n", phys);

    size_t phys_pfn = ptedit_get_pfn(entry.pd); 
    printf(TAG_OK "Physical pfn: %ld\n", phys_pfn);
    void *access = ptedit_pmap(phys, phys_pfn);
    volatile char **str = access;
    //printf(TAG_OK "address[0] = " COLOR_YELLOW "%c" COLOR_RESET "\n", *(volatile char*)access); //for a single character
    printf(TAG_OK "address[0] = " COLOR_YELLOW "%s" COLOR_RESET "\n", (char *)str);  //for a string



    ptedit_cleanup();

    printf(TAG_OK "Done\n");
}
