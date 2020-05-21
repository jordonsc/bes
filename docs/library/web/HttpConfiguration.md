HTTP Server Configuration
=========================
For the most park, you want your web server to forward pretty much everything to the FastCGI interface. The web server
is responsible for throwing 404's and redirects, and your CDN should handle your static assets.

In order to get cookies (and by extension, sessions) to work, you need to also pass the `Cookies` header to the FastCGI
gateway.

A typical Nginx configuration would look like:
    
    http {
        # ...
    
        server {
            listen 80 default_server;
    
            server_name xxxx;
            
            # If you want your web server to pull static assets from your HTTP server, place them in a web root:
            root /path/to/static/assets;
    
            # Ideally, section off the URI for static assets
            location /assets/ {
                # This would resolve to /path/to/static/assets/assets in the above root
                # Images, CSS, JS, etc should be in this folder
                # The /assets/* URI will NOT be passed to the FastCGI application
                # (No stanzas are needed inside this location block)
            }
    
            # Anything not matching the /assets/ URI we'll direct to the FastCGI application
            location / {
                # Important: all our key info is stored here, the FastCGI application expects Nginx's default values
                include /etc/nginx/fastcgi_params;
                
                # Default FastCGI address for a Kubernetes pod or a Docker container with `--net host`
                fastcgi_pass  127.0.0.1:9000;
                
                # Important: we need this to ensure we get our cookies!
                fastcgi_pass_header Cookie;
            }
    
        }
    }

You want to run your HTTP server and your FastCGI application side-by-side, if using Docker, be sure you use 
`--net host` to avoid the very significant overhead of the bridge NAT.
