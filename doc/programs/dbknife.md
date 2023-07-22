# dbknife

## Objectives
That program allows database administration for Mysql (Mariadb) and Postgres.

## Examples

```
# Create a mysql database "dbwiki" and the administrator user "jonny" with passwd "TopSecret":
dbknife create-db --driver=mysql --user=jonny:TopSecret --administrator dbwiki

# Describe the postgresql database "dbstock":
dbknife info --driver=postgres dbstock
# Describe the table "persons" in the mysql database "mycompany" with details:
dbknife info --details --driver=mysql mycompany.persons

# show all databases, the system databases too:
dbknife list-dbs --systems --driver=mysql
# show the databases matching the pattern "db*":
dbknife list-dbs --driver=mysql --pattern=db*

# Describe the usage:
dbknife --help
```

## Usage

```
dbknife [<options>]  MODE
    Database management and more
  -?,--help
    Shows the usage information., e.g. --help -?
  -l LOG-LEVEL,--log-level=LOG-LEVEL
    Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG, e.g. --log-level=123 -l0
  -v,--verbose
    Show more information, e.g. --verbose -v
  --examples
    Show usage examples, e.g. --examples --examples --examples --examples --examples
  MODE
    What should be done:
    create-db
      Create a database and a user
    info
      Create a database and a user
    license
      Create a license.
    list-dbs
      List all databases.
    mailer
      A daemon sending emails defined by files.
    unlicense
      Show the license.
    unveil
      Decrypt a file or stdin.
    veil
      Encrypt a file or stdin.
dbknife create-db [<options>]  DATABASE
    Create a database and a user
  -?,--help
    Shows the usage information., e.g. --help -?
  -d DRIVER,--driver=DRIVER
    The DBMS system: mysql, e.g. --driver=mydata
  -u USER,--user=USER
    Create a user. Format: <username>:<password>, e.g. --user=jonny:TopSecret
  -a,--administrator
    Sets the user rights as administrator., e.g. --administrator -a
  -r,--read-only
    Sets the user rights for read only access., e.g. --read-only -r
  DATABASE
    Name of the database., e.g. mydata
dbknife info [<options>]  WHAT
    Create a database and a user
  -?,--help
    Shows the usage information., e.g. --help -?
  -d DRIVER,--driver=DRIVER
    The DBMS system: mysql, e.g. --driver=mydata
  -t,--detail
    Show more information, e.g. --detail -t
  WHAT
    Defines what should be described. Format: <db>[.<table>, e.g. dbwarehouse dbwarehouse.users
dbknife license    Create a license.
  -?,--help
    Shows the usage information., e.g. --help -?
  -h HOST,--host=HOST
    The licensed host., e.g. --host=mydata
  -3 SECRET3,--secret3=SECRET3
    The 3rd secret., e.g. --secret3=mydata
  -a ADDITIONAL-SECRET,--additional-secret=ADDITIONAL-SECRET
    Additional secret. Format: <scope>.<name>=<secret> Scopes: application user, e.g. --additional-secret=user.jonny=NobodyKnows!
dbknife list-dbs    List all databases.
  -?,--help
    Shows the usage information., e.g. --help -?
  -s,--system
    Lists the system database too., e.g. --system -s
  -d DRIVER,--driver=DRIVER
    The DBMS system: mysql, e.g. --driver=mydata
  -p PATTERN,--pattern=PATTERN
    Only dbs matching that pattern will be listed, e.g. --pattern=*person*
dbknife mailer    A daemon sending emails defined by files.
  -?,--help
    Shows the usage information., e.g. --help -?
  -t TASKS,--tasks=TASKS
    The directory containing the task files., e.g. --tasks=/home/jonny -t.
  -i INTERVAL,--interval=INTERVAL
    The waiting interval in seconds between test for tasks., e.g. --interval=123 -i0
dbknife unlicense [<options>]  FILE
    Show the license.
  -?,--help
    Shows the usage information., e.g. --help -?
  -t TOKEN,--token=TOKEN
    The token., e.g. --token=mydata
  FILE
    The license file to show., e.g. /data/myfile.txt yourfile.pdf
dbknife unveil [<options>]  APPLICATION INPUT OUTPUT
    Decrypt a file or stdin.
  -?,--help
    Shows the usage information., e.g. --help -?
  -u USER,--user=USER
    The password of that user will be asked for authentification., e.g. --user=mydata
  -c CODE,--code=CODE
    The password for authentification. If not given the password will be asked., e.g. --code=mydata
  APPLICATION
    The application to which the configuration file belongs., e.g. mydata
  INPUT
    The file to decrypt. If '~' stdin is used., e.g. /data/myfile.txt yourfile.pdf
  OUTPUT
    The decrypted file. If '~' stdout is used. If '' (empty) the input file is changed., e.g. mydata
dbknife veil [<options>]  APPLICATION INPUT OUTPUT
    Encrypt a file or stdin.
  -?,--help
    Shows the usage information., e.g. --help -?
  -u USER,--user=USER
    The password of that user will be asked for authentification., e.g. --user=mydata
  -c CODE,--code=CODE
    The password for authentification. If not given the password will be asked., e.g. --cod.ÃZŽU
  APPLICATION
    The application to which the configuration file belongs., e.g. mydata
  INPUT
    The file to encrypt. If '~' stdin is used., e.g. /data/myfile.txt yourfile.pdf
  OUTPUT
    The encrypted file. If '~' stdout is used. If '~' (empty) the input file is changed., e.g. mydata
+++ help requested
```
