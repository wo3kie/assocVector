## Copyright (C) 2012 Łukasz Czerwiński

## AssocVector
AssocVector is a container like a std::map but it is implemented using an array instead of a balanced tree. Comparing with balanced tree an array uses less memory and due to keeping data locally binary search in a sorted array can be faster by a constant factor than in a balanced tree. AssocVector was inspired by Loki::AssocVector written by Andrei Alexandrescu, however _insert_ and _erase_ methods were significantly improved (_O(sqrt(N))_ vs. _O(N)_). All the other methods should not be slower.

## Project website
https://github.com/wo3kie/assocVector  
  
## Requirements
C++11  
[Loki](http://loki-lib.sourceforge.net/) (optional for performance test)

## How to build it?
make
  
## How to run it?
For unit test
```
./ut
```

For performance test
```
./pt
```

## How to use it?
AssocVector is absolutely compatible with _std::map_
```
    av[ "c++" ] = 1983;  
    av[ "java" ] = 1995;  
    av[ "scala" ] = 2001;  
    assert( av[ "c++" ] == 1983 );  
    assert( av.count( "java" ) == 1 );  
    assert( av.find( "scala" ) != av.end() );  
```

## How does _AssocVector_ work?
AssocVector is composed of three arrays
* first with obects called 'storage'
* second with objects called 'buffer'
* third with pointers called 'erased'

All arrays are kept sorted all the time. _Insert_ puts a new item into 'buffer'. Since 'buffer' is much shorter than 'storage' this is an effective oprtation. When 'buffer' is filled completely it is merged with 'storage'. This operation may not be cheap but it is performed only from time to time. Lets take a look at an example:

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    |   |   |   |  
    +---+---+---+  
```

After insert 2

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 2 |   |   |  
    +---+---+---+  
```

After insert 3

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 2 | 3 |   |  
    +---+---+---+  
```

After insert 49

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 2 | 3 | 49|  
    +---+---+---+  
```

After insert 5

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 2 | 3 | 4 | 6 | 8 | 9 | 11| 13| 32| 49|   |   |   |   |  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 5 |   |   |   |
    +---+---+---+---+  
```

After AssocVector::merge called

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 8 | 9 | 11| 13| 32| 49|   |   |   |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |   |   |   |   |
    +---+---+---+---+  
```

Erase of item is implemented in two ways. Item present in 'buffer' is erased immediately. Item present in 'storage' is not erased, but it is marked as 'removed' using 'erased' table. If 'erased' table is full all coresponding items are removed from 'storage'. Size of 'erased' is the same as size of 'buffer' and it is equal to sqrt('storage'.size()).

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 2 | 3 | 49|  
    +---+---+---+  
    |   |   |   |  
    +---+---+---+  
```

After erase 6

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 2 | 3 | 49|  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  
```

After erase 2

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 3 | 49|   |  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  
```

After erase 49

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 3 |   |   |  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  
```

After erase 0

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 3 |   |   |  
    +---+---+---+  
    |*0 |*6 |   |  
    +---+---+---+  
```

After erase 11

```
    +---+---+---+---+---+---+---+---+---+
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32|
    +---+---+---+---+---+---+---+---+---+
    | 3 |   |   |  
    +---+---+---+  
    |*0 |*6 |*11|  
    +---+---+---+  
```

After erase 50

```
    +---+---+---+---+---+---+---+---+---+
    | 1 | 3 | 4 | 8 | 9 | 13| 32|   |   |
    +---+---+---+---+---+---+---+---+---+
    |   |   |   |  
    +---+---+---+  
    |*50|   |   |  
    +---+---+---+  
```

## Why does a buffer have a size equal _sqrt_ of _storage_?
Inserting items to the beginning of AssocVector may be considered as a sequence of following operations:

```
    InsertToBuffer  
    Merge  
    
    InsertToBuffer  
    Merge  
    
    InsertToBuffer  
    Merge  
    
    ...  
  
    InsertToBuffer  
    Merge  
```

We may assume that a storage has a size equal _N_ and a buffer has a size equal _B_. If a buffer has a size of _B_, couple of operations (_InsertToBuffer_, _Merge_) is executed _N_/_B_ times. The bigger buffer is, the longer it takes to insert an item into it but the less frequently it necesitates 'merge' operation, and on the other hand the smaller buffer is, the shorter it takes to insert an item into it but 'merge' operation has to be performed more often. We have to find such a buffer size _B_ as a function of storage size _N_, to make all these operations optimal.  
  
One _InsertToBuffer_ takes 1+2+3+4+...+_B_ = (1+_B_)(_B_/2) = (_B_/2)(1+_B_) assignments. All _InsertToBuffer_ operations take _N_/_B_ times more, that's (_N_/_B_)(_B_/2)(1+_B_) = (_N_/2)(1+_B_) assignments.  
  
We can spot also that  
1st _Merge_ takes *B* assignments  
2nd _Merge_ takes 2*B* assignments (_B_ moves for making a place, _B_ assignments)  
3rd _Merge_ takes 3*B* assignments (2*B* moves for making a place, _B_ assignments)  
4th _Merge_ takes 4*B* assignments (3*B* moves for making a place, _B_ assignments)  
...  
last (_N_/_B_)th _Merge_ takes (_N_/_B_)_B_ = _N_ assignments.  
  
All _Merge_ operations together take (*B*+2*B*+3*B*+4*B*+...+(_N_/_B_)_B_) assignments:  
(*B*+2*B*+3*B*+4*B*+...+(_N_/_B_)_B_)  
= _B_(1+2+3+4+...+(_N_/_B_))  
= _B_((1+_N_/_B_)(_N_/_B_)/2)  
= (1+_N_/_B_)(_N_/2) assignments  
  
Putting it all together we are getting, that all operations take
(_N_/2)(1+_B_)+(1+(_N_/_B_))(_N_/2)  
= (_N_/2)((1+_B_)+(1+_N_/_B_))  
= (_N_/2)(_B_+_N_/_B_+2) assignments.  
  
Now we have to find such a _B_ that is optimal. In matematical words we would like to find a minimum for the function  
f(_B_) = (_N_/2)(_B_+_N_/_B_+2)  

Function f(_B_) = (_N_/2)(_B_+_N_/_B_+2) has minimum value if f(_B_) = _B_+_N_/_B_ has a minimum as well  
  
f(_B_) = _B_+_N_/_B_  
f'(_B_) = 1-_N_/(_B_^2)  
f'(_B_) = 0  
1 = _N_/(_B_^2)  
_B_ = sqrt(_N_).  
  
## Contributors
I would like to thank Arkadiusz Nagórka for making a great code review and finding some bugs.

