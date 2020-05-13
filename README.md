Project Bes
===========
![Egyptian God Bes](docs/img/bes.jpeg "Egyptian God Bes")

_Project Bes_ is a foundational framework for building a high-performance C++ microservices environment. It contains
application templates and an infrastructure that allows you to:
* Communicate easily with the gRPC and Protobuf messages
* Handle to HTTP requests with integrated FastCGI
* Respond to HTTP requests with an high-speed Jinja2-style templating engine
* Distribute quickly in a containerised environment such as Kubernetes
* Be cloud-friendly and cloud-provider agnostic

A core value of this project, in order to ensure stability and high-performance:
* Don't trust random library producers

As such, an absolute minimal external dependency list is mandatory for this project. Dependencies that do make it in
must be highly trusted and/or only touch non-critical components. 

Documentation Contents
----------------------
* [Building](docs/Building.md)
* [Performance](docs/Performance.md)
* [Learning Resources](docs/Learning_Resources.md)

Library Documentation
---------------------
* Application Kernel
* Concurrency
* [FastCGI + Web](docs/library/FastCGI.md)
* Filesystem
* Log
* Net
* Service (gRPC abstraction)
* [Templating](docs/library/Templating.md)

Code Dependencies
-----------------
Core dependencies:
* gRPC
  * Nexus of RPC applications

Non-critical dependencies:
* YAML C++:
  * Reading application config files
  * Loading routes into a `MappedRouter` (optional)
* GTest:
  * Running test suites

