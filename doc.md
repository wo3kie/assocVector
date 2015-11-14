# Welcome to the AssocVector documentation!

## FAQ

### _AssocVector_, what is it?
AssocVector functionally is like a map, but it is implemented with an array for low memory consumption and fast search. Searching in a sorted array has _O(log(n))_ complexity, the same as in balanced tree however it is faster by a constant factor.
AssocVector was inspired by Loki::AssocVector written by Andrei Alexandrescu, with _insert_ and _erase_ methods significantly improved (_O(sqrt(N))_ vs. _O(N)_). All the other methods should not be slower.


**Most important links:**  
GitHub source code  : https://github.com/wo3kie/AssocVector  
GitHub wiki page    : https://github.com/wo3kie/AssocVector/wiki  
GitHub wiki changes : https://github.com/wo3kie/AssocVector/wiki/changes  
Author's web page   : http://www.lukaszczerwinski.pl/assoc_vector.en.html  
  
  
### Under what licence _AssocVector_ is distributed?
    Copyright (c) 2012, £ukasz Czerwiñski
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  
  
### Does _AssocVector_ have any dependencies?
No it does not depend on any other libraries (except STL) but **C++11 Standard** is required. If you need to run AssocVector with C++11 support please use AssocVector 1.0.x version instead.
  
  
### Which compilers are supported?
* g++ >= 4.6.

If you need to run AssocVector with other compiler please use AssocVector 1.0.x version instead.
  
  
### How to build code?
* g++ -Wall --pedantic --std=c++0x -O1 unit.test.cpp -o unit.test
* g++ -Wall --pedantic --std=c++0x -O3 perf.test.cpp -o perf.test -I _path to Loki/Boost libraries_
  
### Are there any performance tests for it?
Yes, you may found them with source code. There are tests for some different architectures. Code is built with g++ -O3 -DNDEBUG options.
  
For Intel Celeron 1.4GHz, 1GB RAM, rvalue reference, click [here](http://lukaszczerwinski.pl/av.perf.celeron.html).  
For Intel Core 2 Duo 2.5GHz, 3GB RAM, rvalue reference, click [here](http://lukaszczerwinski.pl/av.perf.intel.html).  
  
  
### Is there any _a one minute tutorial_, how to use _AssocVector_.
AssocVector is completely compatible with _std::map_ and usage of it is exactly the same.

    av[ "c++" ] = 1983;  
    av[ "java" ] = 1995;  
    av[ "scala" ] = 2001;  
    assert( av[ "c++" ] == 1983 );  
    assert( av.count( "java" ) == 1 );  
    assert( av.find( "scala" ) != av.end() );  
  
  
### How does _AssocVector_ work?
AssocVector is composed of three arrays,
* first called 'storage', with objects
* second called 'buffer', with objects
* third called 'erased', with pointers to erased objects

All arrays are kept sorted all the time. Insert operation puts new items into 'buffer'. Since 'buffer' is much shorter than 'storage' it is quite effective. When 'buffer' is completely full it is merged into 'storage'. This operation may be longer however it is performed only sometimes.

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    |   |   |   |  
    +---+---+---+  

After insert 2

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 |   |   |  
    +---+---+---+  

After insert 3

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 |   |  
    +---+---+---+  

After insert 49

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 | 49|  
    +---+---+---+  

After insert 5

    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 2 | 3 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 49| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 5 |   |   |  
    +---+---+---+  

After AssocVector::merge called

    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 49| 50|   |   |  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+...  
    |   |   |   |  
    +---+---+---+  

All the time size of 'buffer' is kept as sqrt('storage'.size()) which is an optimal coefficient for it.

Erasing items is implemented in two ways. Items present in 'buffer' are erased immediately. Items in 'storage' are not erased immediately, but they are marked as erased using 'erased' table. If 'erased' table is filled in completely such items are really removed from 'storage'. Size of 'erased' is the same as size of 'buffer' and equal sqrt('storage'.size()).

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 | 49|  
    +---+---+---+  
    |   |   |   |  
    +---+---+---+  

After erase 6

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 2 | 3 | 49|  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  

After erase 2

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 | 49|   |  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  


After erase 49

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 |   |   |  
    +---+---+---+  
    |*6 |   |   |  
    +---+---+---+  

After erase 0

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 |   |   |  
    +---+---+---+  
    |*0 |*6 |   |  
    +---+---+---+  

After erase 11

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 0 | 1 | 4 | 6 | 8 | 9 | 11| 13| 32| 35| 47| 48| 50|  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 3 |   |   |  
    +---+---+---+  
    |*0 |*6 |*11|  
    +---+---+---+  

After erase 50

    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    | 1 | 3 | 4 | 8 | 9 | 13| 32| 35| 47| 48| 50|   |   |  
    +---+---+---+---+---+---+---+---+---+---+---+---+---+  
    |   |   |   |  
    +---+---+---+  
    |*50|   |   |  
    +---+---+---+  
  
  
### Why a buffer has size _sqrt_ of _storage_?
We may assume that storage has size equal _N_, buffer has size _B_.
Putting items to the beginning of AssocVector may be considered as a sequence of following operations:

    InsertToBuffer  
    Merge  
    
    InsertToBuffer  
    Merge  
    
    InsertToBuffer  
    Merge  
    
    ...  
  
    InsertToBuffer  
    Merge  
  
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
  
  
### What _AssocVector::merge_ method is for?
The method flatenizes container. It merges a buffer, an erased and a main storage together. After it an _AssocVector_ is like single, continuous array.
  
  
### There are methods like _\_insert_, _\_erase_, _\_find_, what these methods are for?
These methods are an extensions which has to be explicitly enabled with _AV_ENABLE_EXTENSIONS_ macro. These methods has a little bit different signature in comparision to STL versions, but thanks to is are faster.

* AssocVector::\_insert( value\_type const & ) returns _bool_ instead of _iterator_ type.

* AssocVector::\_find( key\_type const & ) returns _\_iterator_ instead of _iterator_. _\_iterator_ is a simplified version of _iterator_, missing _iterator_category_ tag. It is rather like an optional. \_iterator does not support increment/decrement operations, it allows dereference only.

* _AssocVector::\_end()_ is design to be used with _AssocVector::\_find_ together.

* _AssocVector::\_erase( iterator )_ returns _bool_ instead of an iterator.
  
  
### Is there any additional documentation for _AssocVector_?
No, it is not. GitHub/AssocVector/Wiki should be consider as the more in depth documentation about _AssocVector_. If it is missing something just let me know.
  
  
### I have an idea how to improve an _AssocVector_ even more. What should I do?
Just let me know!
  
  
### I have some additional test cases worth to be added to _AssocVector_'s unit tests. What should I do?
Just let me know!
  
  
### I with an _AssocVector_ to have an additional feature like... (eg.: erase items by predicate). May you do it?
No, I do not. Only features available in standard std::map are and will be supported.
  
  
### I found a bug, what should I do?
Please report it in GitHub/AssocVector Issues tracker. The more description you provide the easier it is to me to fix the bug.
  
  
### About author
£ukasz Czerwiñski, (wo3kie?gmail?com)
