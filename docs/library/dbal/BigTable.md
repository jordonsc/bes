DBAL: Google Cloud Bigtable
===========================

Getting Started
---------------
`gcloud` and `cbt` tools are essential for working with Cloud Bigtable. 

* [gcloud SDK](https://cloud.google.com/bigtable/docs/installing-cloud-sdk)
* [cbt tool](https://cloud.google.com/bigtable/docs/cbt-overview)
* [cbt quickstart](https://cloud.google.com/bigtable/docs/quickstart-cbt)

Credentials
-----------
You will need a JSON key from a service account in Gcloud. You can create this in the IAM section of the Console, then
adding the _Bigtable Administrator_ role.

# Define An Instance ID

    export BES_CBT_INSTANCE="bes-`whoami`"

# Set Default Project

    echo project = prj-bes > ~/.cbtrc
    echo instance = $BES_CBT_INSTANCE >> ~/.cbtrc

> `prj-bes` is the official Bes project account, you can change this to your own account. 

# Set Default Credentials

    export GOOGLE_APPLICATION_CREDENTIALS="PATH"

Provisioning A Test Instance
----------------------------
To create/delete a Google Bigtable instance for testing:

    gcloud bigtable instances create $BES_CBT_INSTANCE --display-name="Bes - `whoami`" --cluster-storage-type=SSD --cluster-config=id=bes-test-c1,region=australia-southeast1,nodes=1
    gcloud bigtable instances delete $BES_CBT_INSTANCE

> NB: these instances are very expensive, be sure to delete them after testing!

Using the CLI tool
------------------
Confirm working access with:

    cbt ls
    
> You might be asked to enable the Admin API if you've never used it before.


