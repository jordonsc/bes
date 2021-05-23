Development Log
===============

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
