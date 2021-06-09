Running Tests
=============
The tests in the Bes framework are all Gtest unit tests, but they are broken into two categories:

* Standard unit tests
* Live integration tests

All tests are run via Bazel's test functionality:

    # Run all unit & integration tests:
    bazel test //test:all
    
    # Run all unit tests (skip integration tests):
    bazel test //test:all --test_tag_filters=-integration

The `.bazelrc` file will instruct the test command output only to be verbose on failure.

> Integration tests should be skipped unless you're running local database servers. 

Integration Tests
-----------------
To run all integration tests, you'll need to make available:

* A local Redis server
* A local Cassandra server
* A test Google Bigtable instance

Local servers can be provisioned quickly via Docker:

    docker run --net host -d redis
    docker run --net host -d cassandra

To create/delete a Google Bigtable instance for testing:

    gcloud bigtable instances create bes-test --display-name="Bes Test" --cluster-storage-type=SSD --cluster-config=id=bes-test-c1,region=australia-southeast1,nodes=1
    gcloud bigtable instances delete bes-test

> Don't forget to delete this, these instances are expensive :)

You can now run the integration tests:

    bazel test //test:all --test_tag_filters=integration
