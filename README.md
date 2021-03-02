# **BlackLeopard**
### Contains various page table resolution options in preparation for a GPU DMA attack. This repository utilizes the **PTEditor** repository: https://github.com/misc0110/PTEditor

### All functions included in `pteditor.h` are defined in PTEditor repository. The functions documented in this repository are just a few of the pertinant functions necessary for this project.

# Project Files

### ***possible_buffers***  

`./possible_buffers <pid>`

- This takes a process ***pid*** and prints the range of possible virtual addresses and thier associated physical addresses
and page frame numbers of the processe's variables. This information is needed in order to map the physical addresses and page frame numbers to a 
GPU controller process to perform Direct Memory Access and retrieve the value of a desired variable in plain text. 

### ***read_address***     

`./read_address <pid (1234)> <virtual address (0x1234abcd)>`

- This takes the ***pid*** of a target process and the ***virtual address** that contained the desired content (password, keys, etc.)
This resolves the page table entry of the virtual address, takes the variables physical address, and maps it to its own process 
address space with **`void * ptedit_pmap(size_t physical, size_t pfn)`**. This can only be done while the target process is running. 
The GPU controller process is needed in order to read the physical address after the target process is killed. 

### ***src***

- Contains the kernel module that must be loaded in order to run `possible_buffer` and `read_address`. Also contains the
`pteditor.h` file.

# **Compile/Run**

Navigate to the the **src** directory.

`sudo make`

`sudo insmod pteditor.ko`

Navigate to either *possible_buffers* or *read_address*

`make`

`./possible_buffers <pid>`

`./read_address <pid> <address>`


# **Key Functionality**

### `ptedit_entry_t` `ptedit_resolve(void * address, pid_t pid)`

Resolves the page-table entries of all levels for a virtual address of a given process.

Returns A structure containing the ***page-table entries*** of all levels.

### Parameters

`address` The virtual address to resolve

`pid` The pid of the process (0 for own process)

___


### `int` `ptedit_get_pagesize()` 

Returns the default *page size* of the system

Returns Page size of the system in *bytes*

___



### `void` `ptedit_update(void * address,pid_t pid, ptedit_entry_t * vm)` 

Updates one or more page-table entries for a virtual address of a given process. 

The TLB for the given address is flushed after updating the entries.

### Parameters

`address` The virtual address

`pid` The pid of the process (0 for own process)

`vm` A structure containing the values for the page-table entries and a bitmask indicating which entries to update. Obtained using `pte_resolve(`


___


###  `size_t` `ptedit_pte_get_pfn(void * address, pid_t pid)` 

Reads the PFN directly from the PTE of an address.

Returns *The page-frame number* (PFN)

#### Parameters

`address` The virtual address

`pid` The pid of the process (0 for own process)


___


### `size_t` `ptedit_get_pfn(size_t entry)` 

Returns the *page-frame number (PFN)* of a page-table entry.

### Parameters

`entry` The page-table entry to extract the PFN from a resolved page table. 

*size_t entry* is resolved with `pte_resolve(void *address, size_t pid)`

Returns The *page-frame number*
___

### `void` `ptedit_read_physical_page(size_t pfn,char * buffer)` 

Retrieves the content of a physical page.

### Parameters

`pfn` The page-frame number (PFN) of the page to read

`buffer` A buffer which is large enough to hold the content of the page

___

### `void *` `ptedit_pmap(size_t physical, size_t pfn)`

Map a physical address range to the virtual address space.

Returns A *virtual address* that can be used to access the physical address.

### Parameters

`physical` The physical address to map

`pfn` The page frame number of the physical address

**Note:** *This function is not supported on Windows.*

___

### `size_t` `ptedit_get_paging_root(pid_t pid)`

Returns the *root* of the paging structure (i.e., CR3 on x86 and TTBR0 on ARM).

Returns The ***phyiscal address*** (not PFN!) of the first page table (i.e., the PGD)

### Parameters

`pid` The proccess id (0 for own process)

___

### `void` `ptedit_print_entry(size_t entry)`

Pretty prints a page-table entry.

### Parameters

`entry` A page-table entry
