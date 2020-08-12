Bes Templating
==============
Goal
----
The Bes templating library is designed so that:
* It remains as close as possible to Jinja2 syntax
* It has high-speed real-time rendering of templates, at the cost of:
* Up-front processing time to parse templates

Functionality
-------------
Key Jinja2 functionality supported includes:

### Tags
* Value tags `{{ ... }}`
* `{% if .. elif .. else .. endif %}` tags
* Basic for loops (no else or if conditions)
* Comment tags `{# ... #}`
* Block tags and template extensions
* Include tags
* Macros (default values not supported)

### Expressions
* Member object referencing {{ object.member_object.sub_member }}
* Literal expressions for
  * String objects {{ "literal string" }}
  * Integer and floating-point numbers {{ 123.456 }}
  * C-style characters `{{ 'c' }}`
  * Boolean values `{% if true %}` (with alpha rendering: `{{ true }}`)
  * Mixed-type arrays `{% for n in ["item1", 32, false] %}`
* `is defined`
* `in` for loops, but not yet available for `if` statements
* Negation with `not`
* Comparison and equality operators:
  * `==`, `!=`
  * `<`, `<=`, `>`, `>=`
  
### Filters
* `trim`, `rtrim` and `ltrim` for trimming whitespace
* `nl2br` and `nl2p` for converting line breaks to `<br/>` or `<p>..</p>` tags

### Other
* White-space controls `{{- ... -}}`
* A `loop` object is made available inside loops containing member objects:
  * `index` and `index1`) (int, zero/one-indexed loop iteration)
  * `first` and `last` (bool, true if the first/last iteration)
  * `odd` and `even` (bool, true if the odd/even iteration - one-indexed)

Planned
-------
* Additional filters

Not Supported
-------------
Key Jinja2 features not supported and on the _immediate_ road-map:
* Assignment operators
* Mathematical operators

Any of the above may be added in the future (request it and it'll happen).

Differences From Jinja2
-----------------------
Function style syntax `something()` does not exist in this implementation, this has created a few minor differences:

### Super
Nested blocks may use `super` context, but should do it in the form of a context item (variable), not a function call:

    {{ super }}
    

### Include
Include tags have the same syntax as `extends`, for example:

    {% include "some_template" %} 

Type Mapping
------------
C++ is a strongly-typed language, and Jinja was built on top of Python. As such maintaining strong-typing against
context objects while trying to permit a non-typed templating syntax must come with caveats.

* Scalar types, including all numbers and `std::string` (and `char*`) objects have built-in mappings and will work 
  seamlessly
* `std::vector<std::string>` will automatically be iterable
* Custom classes require a template specialisation so that the templating engine knows how to render, compare, iterate 
  the object and how to access member objects


Performance & Efficiency
------------------------
Rendering a template is designed to be as quick as possible, at the expense of fully parsing the template when loading
it. As such you should consider:
* Pre-load the templates before they are needed
* Create a `ContextBuilder` or `Context` object for each hit/render to avoid locks slowing down read access
* Type comparisons within the template cannot be cached (as the context is dynamic and has scope), so avoid complex
  comparison expressions as they require transforming the type into generic types to perform the operation
* Filters require buffering data, which is less efficient than direct streaming - avoid if possible
* If you're rendering to HTML, the chances are you can achieve the same result as a filter using CSS
