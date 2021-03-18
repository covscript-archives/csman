# Csman

* **Platform**: **Linux** & **MacOs** & **Windows**

## Description 

  The csman is a package manager. This tool helps managing all packages that users installed for CovScript. To install and control them easily, also handle their dependencies correctly.    
  
  It need online condition. But you can only use the part of function offline.    
  
  You can set some vars by writing them down in configure file of csman.   
  
***

## Usage

### Download

#### compile from source(for all paltform)

use    

``` shell
git clone https://github.com/covscript/csman/
cd csman
make 
```

#### for Linux

use    

``` shell
sudo apt install csman
```

or    

``` shell
sudo yum install csman
```

#### Official Homepage

goto our [homepage](https://covariant.cn/covscript/) for all platform downloading.       

+ Version management: Support for installing multiple CovScript versions, and support for setting default versions
	+ csman install <latest | nightly | version> [--reinstall | --fix]
	+ csman uninstall <all | version-regex> [--clean]
	+ csman checkout <latest | version>
	+ csman run [-v version] <command>
	+ csman list
+ Extended management: a fully functional networked package manager that supports runtime version management
	+ csman install <package name> [--reinstall | --fix]
	+ csman uninstall <package name(regex)> [--clean]
	+ csman list
+ Configuration Manager: Command line configuration manager
	+ csman config set
	+ csman config unset
	+ csman config get
