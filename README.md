# cdat

> A lightweight c data structures library 
- [INSTALLATION](#INSTALLATION)
- [ABOUT](#ABOUT)
- [USAGE](#USAGE)


# INSTALLATION
Note: Only tested using gcc, to use a different compiler change the CC variable in the make file

Clone repository into your lib folder of an existing c project

``` 
cd lib
git clone https://github.com/PrinceOfPuppers/cdat.git
```
You can then test the library by running the following
``` 
cd cdat
make test
```

## Compiling
To compile cdat
```
make -C /path/to/cdat 
```
If you wish to use gdb on cdat then compile with debug recipe
```
make -C /path/to/cdat debug
```

## Linking
To link cdat, you must add the following include and lib flags to gcc when compiling your main project
```
include: -Ilib/cdat/include
lib:     -L ./lib -l:cdat/lib/cdat.a
```


# ABOUT
cdat is a lightweight and very low overhead data structure library, supporting doubly-linked-lists (just called linked-lists or `ll` for short), hash-sets `hs`, and hash-maps `hm`. All data structures support are type invariant and can be run as copy-on-write or simply by holding references.

# USAGE
For a full list of functionality, see the following header files in `./cdat/include/cdat/*`:
- `linked-list.h` (doubly linked)
- `hash-set.h`
- `hash-map.h`
- `helpers.h` (holds some generic types for callback functions, as well as hash functions)

The functions are generally self explanatory with the barring the following notes:
- `ll`, `hm`, `hs` are short for linked-list, hash-map, and hash-set respectively
- each data structure has an iterator, call `*_next` to get a `ll` node containing the next value, and `*_iter_reset` to set iter to first element
- `*_pop will` remove a value and return it afterwards no matter the copy-on-write status, you must free it
- `*_pop_val` will search the data structure by value for `ll` and `hs`, and by key for `hm`, and return it
- commands with try in the name will attempt to do something and if they cant, return a boolean. Other commands will throw throw a runtime error if they fail ie) `*_pop` vs `*_try_pop`.
- arguments labeled `out_*` will be populated with return values (unless you pass a NULL pointer to them)

There are 3 functions for freeing each data structure, the last one is the intended way to free:
- `*_free_keep_vals` calls free on data structure leaves entries (intended for no copy-on-write data)
- `*_free_free_vals` calls free on data structure and all its entries (intended for copy-on-write data structures)
- `*_free` chooses between the prior two based on if the was made with copy-on-write (intended way to free)
