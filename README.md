This is the first try after learning meta program.

This purpose of the project is to create a script like c++ framework.
The first priority of the project is easy to use and then the performance.
So it will sometimes sacrifice the performance for easily using.

Here are some main features of the framework:
1. No memory management:
   All objects are created by CSharedPtr, which is an enhanced std::shared_ptr.
   Users do not need to care the life cycle of the object.
   No alloc and no free. Everything is a shared pointer.

2. Enhancement string:
   The CString class can support most string operations like
   Find, Split, Slice, Strip, etc...
   Simple json supported by converting between CString and CJson.

3. Promise chain support.
   There are some performance penalty to use the promise chain,
   especiall the async promise chain.
   However, according to the basic idea, such sacrify is acceptible.
   Hoping the C++ compiler can formally support this feature in the future.

4. Other features.
   A powerful iterator framework from which the developer can be
   benefited by writting few codes.
   An experimental function class which is different to the std::function
   and not using the virtual function. (Maybe have worse performance, need test).
   A simple buf fast memory pool which is based on CAS.
   An experimental feature to support regex.

5. The framework is designed to be distributed.
   All the interfaces/libraries are separated to different packages.
   A package manager can be used to maintain the dependency among the packages.

6. Cross platfrom ready:
   The project is mainly developed on Ubuntu64.
   There is a plan to support windows visual studio.
   Other platfrom can also be supported.

TODO:
1. It is the prototype of the framework.
   More tests/debuging will be done late.

2. The test framework is ready but more test cases should be added.

3. A runtime environment with which the user's codes can be executed like a script.
   This is really a tough task to me and I cannot promise this.
   Any friends who familiar with this and are willing to give me help is really appreciated. :)

4. Others TODO to be continued...

