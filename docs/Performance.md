Application Tuning
==================
FastCGI
-------
There are a couple thresholds for connection handling,

* How many threads we run to respond to connections
* How many connections we allow the socket to queue before rejecting connections at a network level
  * Defined in the `SocketConnector`

RPC Apps
--------
Each RPC application has a pool for each RPC it responds to, not the overall service. That means, if an RPC service has
3 RPCs, by default they'll each get 10 threads. But a single RPC could consume 90% of the traffic - it's pool could be
fully consumed while the other two pools are nearly empty. As such, you need to balance the thread-pools of each RPC
according to the traffic with consideration to the time each RPC takes to complete.

* RPC thread-pool size
