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
* [Contributing](docs/Contributing.md) - Includes code styles & naming conventions.
* [Building](docs/Building.md)
* [Testing](docs/Testing.md)
* [Performance](docs/Performance.md)
* [Learning Resources](docs/Learning_Resources.md)

Library Documentation
---------------------
* Application Kernel
* Core
* [DBAL](docs/library/DBAL.md)
* [FastCGI](docs/library/FastCGI.md)
* Log
* Net
* Service (gRPC abstraction)
* [Templating](docs/library/Templating.md)
* [Web](docs/library/Web.md)

Code Dependencies
-----------------
Core dependencies:
* gRPC
  * Nexus of RPC applications

Non-critical dependencies:
* YAML C++:
  * Reading application config files
  * Loading routes into a `MappedRouter` (optional)
* CPP Redis:
  * For Redis session support
* GTest:
  * Running test suites

DBAL Requirements:
* Cassandra: Drivers are required on the OS; see the prerequisites in the [building documentation](docs/Building.md)
* BigTable: Google Cloud APIs are included as a project dependency
