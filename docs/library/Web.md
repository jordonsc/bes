Web Server
==========
The Web library is an extension of the FastCGI library, offering a suite of HTTP tools and a web framework for providing
a separation of logic between the core HTTP engine, your controllers and view layer.

The Web library has a few extensions that further enable functionality but are not included by default to reduce
dependencies.

### Core Features
* HTTP framework
* Header, cookie and parameter abstraction
* Session management (requires a driver)
* Routing interfaces
  * A YAML-based mapped router

### Web.Redis
* A Redis session driver via
  * a direct connection; or
  * Sentinel connections

### Web.Templating
* Integration with the Bes templating library to provide Jinja2 style template support
* Further abstracted view/controller layer


Web Library Documentation
-------------------------
* [Application Level Configuration](web/AppConfiguration.md)
* [HTTP Server Configuration](web/HttpConfiguration.md)
* [Building Your Application Classes](web/Building.md)
