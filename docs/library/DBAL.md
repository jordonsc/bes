Database Abstraction Layer
==========================
The Database Abstraction Layer (DBAL) is designed to create a set of interfaces that will allow you to interact with
different database engines as if they were all the same and interchangeable. 

The DBAL breaks database engines into their high-level categories:

* Key-value
* Wide-column
* Document
* SQL

You can create multiple implementations of each category, and as long as they are in the same category, the code-level
interface is identical. This allows you quickly migrate database engines, or use different engines in different 
environments.

To achieve consistency, the DBAL will only support features that are largely in parity with other engines. Advanced
features will not be implemented, and where possible, some features will be manually implemented in the DBAL so that
other engines can maintain their capabilities.

> IMPORTANT: Because the DBAL needs to sync features, it strips all databases down to their most base level. Use 
> with discretion, and consider integrating directly to a DB in complex scenarios.

Current Implementations
-----------------------
### Wide Column
* Cassandra
* Google BigTable

Planned Implementations
-----------------------
* Redis (key-value)

Redis support is planned, although key-value databases tend to offer a lot of powerful customisations that make them so
powerful. Redis, for example, supports, TTLs and power set management. If we created another implementation - say
memcached, these engines wouldn't support the same feature set and force the DBAL to not take advantage of most of 
Redis.

Configuration
-------------
All database implementations are configured with a Context object, these are key-value stores that contain the
configuration unique to the implementation. 

    auto ctx = bes::dbal::Context();
    ctx.setParameter("hosts", "127.0.0.1");
    ctx.setParameter(bes::dbal::cassandra::KEYSPACE_PARAM, "my_keyspace");
    
    auto db = bes::dbal::Cassandra(ctx);
    
Each interface is identical, and inherits a base interface. This allows a configuration engine to return a generic
reference.

    ValueList v;
    // Values are in the format: namespace (aka column family), field name and value
    v.push_back(Value("main", "some_string", "bar"));
    v.push_back(Value("main", "id", Int64(100)));
    v.push_back(Value("main", "random_data", Int32(5)));
    auto result = db.insert("my_table", std::move(v));
    
For wide-column databases, inserts and updates are both "upserts" and do the same thing, except an insert doesn't ask
for a "key" value.
    
    v.clear();
    v.push_back(Value("main", "some_string", "hello world"));
    v.push_back(Value("main", "some_float", (Float32)123.456));
    auto result db.update("my_table", Value("main", "id", Int64(101)), std::move(v));

The above will insert all the same as if you had included in the `id` value in the field ValueList.

All results return from database operations are *futures*. This kicks off the query and allows the function to 
immediately return. The flow is not blocked until you either call `wait()` on the future, or a function that requires
the query to have completed (such as accessing row data).

    result.wait();  // wait for the operation to complete
    
> An exception will be raised in the operation fails.

    auto result = db.retrieve("my_table", Value("main", "id", (Int64)5));
    
    // Calling the row iterator will invoke `wait()`
    for (auto row : result) {
        BES_LOG(INFO) << row.at("main", "id").as<Int64>();
    }

Value objects may also be multi-value, this is used when requesting multiple keys from the database. 

> NB: wide-column database are restricted to a single indexed key field (regardless of the underlying engine)

    auto result = db.retrieve("my_table", Value("main", "id", Int64List({100, 101}));
    assert(result.rowCount() == 2);
