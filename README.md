# ReactFS
Distributed File System with change reactors


### Pre-requistes for building the code on MAC

* Install macports from [here](https://guide.macports.org/chunked/installing.macports.html)
* Install cmake from [here](https://cmake.org/download/)

Now make use of *macports* and install the following.

* Invoke macports using : `sudo ports`
* Now run the below set of commands to install the pre-requiste libraries

```
install zlib xz gettext expat libiconv gperf ncurses
install openssl
install flex
install bison
install astyle (devel)
```

### Pre-requisites for building on Linux

```
install uuid (devel)
install zlib
install openssl (devel)
install flex
install bison
install astyle (devel)
```

#### Create build environment file:
```
Create file ${PROJECT_ROOT}/.env
Add:
    export JDK_LIB_HOME=<DIR where JDK is installed>
```
