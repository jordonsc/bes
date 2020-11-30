Web Server Configuration
========================
If you using the `web.templating` `TemplatingApp`, you'll automatically gain some application-level configuration 
options that manage the web framework. CLI options will override the config option.

CLI Options
-----------

    -b --build <string>         A version used to identify your build and cache-bust your assets for new releases
    -d --debug                  Enables debug output, allowing you to view error details in dev environment
    -t --templating <fn>        Path to templating schema
    -r --routing <fn>           Path to routing schema
    
Config Options
--------------

    web.build                   (string) Build version (the same as the --build CLI option)
    web.templating              (string) Path to templating schema
    web.routing                 (string) Path to routing schema
    web.sessions.secure         (bool)   Mark session cookies as `Secure` for HTTPS transport only
    web.sessions.ttl            (int)    Session TTL in seconds, zero for infinite
    web.sessions.cookie         (string) Cookie name for session ID (default: bsn)
    web.sessions.prefix         (string) All session IDs will will prefixed with this (default: S)

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

