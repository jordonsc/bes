Contributing
============

Code Style
----------
Follow the style guidelines in the `.clang-format` file.

Casing & Naming Conventions
---------------------------
Code Type        | Casing             
---------------- | ----------------- 
Namespace        | Snake Case        
Class Names      | Upper Camel Case  
Function Names   | Lower Camel Case  
Variable Names   | Snake Case        

> Namespaces should be chained together: `namespace my::child::namespace {}`

### Example:

    namespace my_root::foo::bar {
    
    class MyClass {
       public:
        void myFunction();
    };
    
    }

> NB: Treat acronyms as a word: `MyApiClass`

Include Guards
--------------
Use `#pragma once`.

> Why? Simplicity and to avoid copy/paste errors.  
> All supported compilers & platforms safely support this feature. 
