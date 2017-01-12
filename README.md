# ReactFS
Distributed File System with change reactors


### Pre-requistes for building the code on MAC

* Install macports from [here](https://guide.macports.org/chunked/installing.macports.html)
* Install cmake from [here](https://cmake.org/download/)

Now make use of *macports* and install the following.

* Invoke macports using : `sudo port`
* Now run the below set of commands to install the pre-requiste libraries

```
install zlib xz gettext expat libiconv gperf ncurses
install openssl
install flex
install bison
install astyle (devel)
install boost
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

#### Ubuntu
```
sudo apt-get install cmake
sudo apt-get install uuid
sudo apt-get install zlib1g-dev
sudo apt-get install libssl-dev
sudo apt-get install flex
sudo apt-get install bison
sudo apt-get install astyle
sudo apt-get install libboost-all-dev
```

#### Create build environment file:
```
Create file ${PROJECT_ROOT}/.env
Add:
    export JDK_LIB_HOME=<DIR where JDK is installed>
```
