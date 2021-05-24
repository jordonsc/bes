Development Log
===============

2021-05-xx (JS)
---------------
### DBAL
* Added Redis to the DBAL as a key-value store
* Key-value databases will support a TTL
* Key-value databases will support "if not exists" logic
* Wide-column databases will NOT support "if not exists" logic
* Database call functions are no longer `const` qualified

A bit of refactoring has been done to the wide-column interface, notably removing the "if exists" boolean logic to
some of the create/drop calls. Instead an exception should be raised. This was done for the simplicity of the interface
as default values don't work well on abstract classes.

The "if not exists" on write operations makes sense for Redis, but on wide-column databases, typically a Paxos or 
similar implementation, this has very serious ramifications that aren't easily understood by someone looking at the
difference between a call to `apply()` or `applyNx()` (not exists), and the logic of `applyNx()` can vary dramatically
depending on how the database cluster is configured.

With Redis added to the DBAL, an obvious next step would be to look at the `web.redis` library and refactor it to use
the DBAL instead, making it more of a `web.sessions` module supporting any DBAL K/V implementation.


2021-05-23 (JS)
---------------
### DBAL
**Development on Google Bigtable has been paused.** Will instead focus on making Cassandra ideal and perhaps some other
more common wide-column databases instead. HBase is the #2 in the wide-column space, so perhaps the BigTable and HBase
work could be combined. The goal was not to get the top players, but rather also include managed systems. My appetite
for building out managed database solutions (Bigtable, Spanner, DynamoDB, etc) is somewhat dwindling.

**Removed the namespace from field names** - this was largely to account for Bigtable, and no other database engine
has similar requirements. Will instead look for a way to add this for BigTable specially, instead of the other way 
around.

**Remove the wide-column `insert` and `update` functions.** In most NoSQL databases, these are the same, and in some
there is no way to do `insert` style logic. Instead a new `apply` function has been created (which works like `update`).

**Split the DBAL into sub-modules** so you don't need the requirements for every DB engine you're not using.

The next most likely candidate to be worked on will be Redis, taking advantage of only simple key/value operations and
hopefully the TTL feature. Redis has a massive lead on any competitors for the key/value space, the closest (and very
far behind) would be DynamoDB, which also supports TTL so I remain confident with that approach.

For reference on the above assumptions, here are some good stats on DB usage:
* [Wide Column](https://db-engines.com/en/ranking/wide+column+store)
* [Key/Value](https://db-engines.com/en/ranking/key-value+store)
