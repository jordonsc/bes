Building a Web Application Class
================================
When running the Web Server, you'll need to build out your own:

* Web Application: will configure the Web Server, notably loading a SessionInterface.
* Controllers: a controller is responsible for responding to a HTTP request, after the URI has been broken down into an 
  "action".

Optionally:
* Router: the router is responsible for taking a given URI and deciding which Controller to use. You can use the 
  `MappedRouter` which loads a YAML configuration to resolve the connection.
 * Session Interface: the session interface is responsible for storing and retrieving user sessions. A `RedisSessionMgr`
   exists to store sessions in a Redis database. 
 
