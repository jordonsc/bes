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
``  * A YAML based mapped router

### Web.Redis
* A Redis session driver

### Web.Templating
* Integration with the Bes templating library to provide Jinja2 style template support
* Further abstracted view/controller layer


Web Server Configuration
------------------------
If you using the `web.templating` `TemplatingApp`, you'll automatically gain some configuration options that manage the
web framework. CLI options will override the config option.

### CLI Options

    -b --build <string>         A version used to identify your build and cache-bust your assets for new releases
    -d --debug                  Enables debug output, allowing you to view error details in dev environment
    -t --templating <fn>        Path to templating schema
    -r --routing <fn>           Path to routing schema
    
### Config Options

    web.build                   (string) Build version (as per --build CLI option_
    web.templating              (string) Path to templating schema
    web.routing                 (string) Path to routing schema
    web.sessions.auto-create    (bool)   Automatically create a new session for each visit
    web.sessions.ttl            (int)    Session TTL in seconds, zero for infinite

### Redis Session Configuration

> Requires the `web.redis` library with the `RedisSession` manager added to the web server.
    
Root node: `web.sessions.redis`:
    
    .host                       (string) Hostname/address to server
    .port                       (int)    Port of server
    .timeout                    (int)    Connection timeout in milliseconds; defaults to 250
    .sentinel-name              (string) Enables sentinel mode, sets the name of the sentinel service to query
    .sentinels                  (list)   List of the below map:
        .host                   (string) Address of sentinel node
        .port                   (int)    Port of sentinel node
        
If `web.sessions.redis.sentinal-name` has a value, and `web.sessions.redis.sentinels` has 1 or more values, then the 
Redis manager will use Sentinel connections. If either of these conditions are not met, then the manager expects that
`web.sessions.redis.host` is populated for a direct connect. The port will default to the default Redis port of `6379`.
