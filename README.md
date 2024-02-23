# VanityFlare

Stellar (http://stellar.org) vanity address generator

# Quick how-to

Create your tables in MySQL (substitute your db_name):
```
mysql db_name < db.sql
```

Setup your `SVP_USER` and `SVP_PASS` environment variables (username and password for the MySQL database user)

Generator(s): Run multiple instances of:
```
./vgen.exe > vfpipe
```

Consumer: Run
```
./cat vfpipe > ./filldb.py
```

Monitor: Run
```
./monitor.py
```


# Requirements

https://github.com/offscale/docopt.c
