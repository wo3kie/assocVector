## Copyright (C) 2012 Lukasz Czerwinski

## AssocVector
AssocVector is a container like std::map but it is implemented using an array for low memory consumption and better cache reusability. Due to keeping data locally searching in a sorted array has the same complexity as searching in a balanced tree however it is faster by a constant factor.
AssocVector was inspired by Loki::AssocVector written by Andrei Alexandrescu, however _insert_ and _erase_ methods significantly improved (_O(sqrt(N))_ vs. _O(N)_). All the other methods should not be slower.

## Project website
https://github.com/wo3kie/assocVector  
  
## Requirements
C++11
Loki from Andrei Alexandrescu (optional for performance test)

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
AssocVector is composed of three arrays,
* first called 'storage', with objects
* second called 'buffer', with objects
* third called 'erased', with pointers to erased objects

All arrays are kept sorted all the time. Insert operation puts new items into 'buffer'. Since 'buffer' is much shorter than 'storage' it is quite effective. When 'buffer' is completely full it is merged into 'storage'. This operation may be longer however it is performed only sometimes.

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    |   |   |   |  
    +---+---+---+  
```

After insert 2

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 |   |   |  
    +---+---+---+  
```

After insert 3

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 |   |  
    +---+---+---+  
```

After insert 49

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 | 49|  
    +---+---+---+  
```

After insert 5

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 2 | 3 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 49| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 5 |   |   |  
    +---+---+---+  
```

After AssocVector::merge called

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 49| 50|   |   |  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+...  
    |   |   |   |  
    +---+---+---+  
```

All the time size of 'buffer' is kept as sqrt('storage'.size()) which is an optimal coefficient for it.

Erasing items is implemented in two ways. Items present in 'buffer' are erased immediately. Items in 'storage' are not erased immediately, but they are marked as erased using 'erased' table. If 'erased' table is filled in completely such items are really removed from 'storage'. Size of 'erased' is the same as size of 'buffer' and equal sqrt('storage'.size()).

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 | 49|  
    +---+---+---+  
    |   |   |   |  
    +---+---+---+  
```

After erase 6

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 | 49|  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  
```

After erase 2

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 | 49|   |  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  
```

After erase 49

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 |   |   |  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  
```

After erase 0

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 |   |   |  
    +---+---+---+  
    |*0 |*6 |   |  
    +---+---+---+  
```

After erase 11

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 |   |   |  
    +---+---+---+  
    |*0 |*6 |*11|  
    +---+---+---+  
```

After erase 50

```
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 1 | 3 | 4 | 8 | 9 | 13| 32| 35| 47| 48| 50|   |   |  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    |   |   |   |  
    +---+---+---+  
    |*50|   |   |  
    +---+---+---+  
```

## Why a buffer has a size _sqrt_ of _storage_?
We may assume that storage has size equal _N_, buffer has size _B_.
Putting items to the beginning of AssocVector may be considered as a sequence of following operations:

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

If buffer has size of _B_, pair of operations (_InsertToBuffer_, _Merge_) is executed _N_/_B_ times.  
  
The bigger buffer is the longer takes putting into it but merges are performed less often,  
the shorter buffer is the shorter takes putting into it but merges are have to be performed more often.  
  
We have to find such buffer size _B_, as a function of storage size _N_, to make it optimal.  
  
One _InsertToBuffer_ takes 1+2+3+4+...+_B_ = (1+_B_)(_B_/2) = (_B_/2)(1+_B_) assignments.  
All _InsertToBuffer_ operations take _N_/_B_ times more, that's (_N_/_B_)(_B_/2)(1+_B_) = (_N_/2)(1+_B_) assignments.  
  
On the other hand,  
1st _Merge_ takes _B_ assignments  
2nd _Merge_ takes 2_B_ assignments (_B_ moves for making place, _B_ assignments)  
3rd _Merge_ takes 3_B_ assignments (2_B_ moves for making place, _B_ assignments)  
4th _Merge_ takes 4_B_ assignments (3_B_ moves for making place, _B_ assignments)  
...  
(_N_/_B_)th (last) Merge takes (_N_/_B_)_B_ = _N_ assignments.  
  
All _Merge_ operations take (_B_+2_B_+3_B_+4_B_+...+(_N_/_B_)_B_) assignments:  
(_B_+2_B_+3_B_+4_B_+...+(_N_/_B_)_B_) = _B_(1+2+3+4+...+(_N_/_B_)) = _B_((1+_N_/_B_)(_N_/_B_)/2) = (1+_N_/_B_)(_N_/2) assignments  
  
Putting is all together, we get, that all operations take (_N_/2)(1+_B_)+(1+(_N_/_B_))(_N_/2) assignments:  
(_N_/2)(1+_B_)+(1+(_N_/_B_))(_N_/2) = (_N_/2)((1+_B_)+(1+_N_/_B_)) = (_N_/2)(_B_+_N_/_B_+2) assignments.  
  
Now we have to find such _B_ that is optimal (find minimum) for the function f(_B_) = (_N_/2)(_B_+_N_/_B_+2)  
Function f(_B_) = (_N_/2)(_B_+_N_/_B_+2) is minimum if f(_B_) = _B_+_N_/_B_ is minimum as well  
  
f(_B_) = _B_+_N_/_B_  
f'(_B_) = 1-_N_/(_B_^2)  
f'(_B_) = 0  <->  1 = _N_/(_B_^2)  <->  _B_ = sqrt(_N_).  
  

### Contributors
I would like to thank you for your contributions
- you allowed me to improve the quality of the AssocVector library:

* Arkadiusz Nagorka, did a great code review and found some bugs  
