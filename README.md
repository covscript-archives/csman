# Csman

* **Platform**: Linux&MacOs&Windows

## Description 

  The Csman is a package manager. This tool helps managing all packages that users installed for CovScript. To install and control them easily, also handle their dependencies correctly.    
  
  Csman is a tool which need online condition. Users can only use a part of function offline.    
  
  Wherever users install it in, csman can successfully run. And users can set some options in configure file of Csman.    

## Usage

### Download

#### compile from source(for all paltform)

use    

```
git clone https://github.com/Access-Rend/csman
```

then in the path of where you clone csman (../csman)  
  
```
make 
```

#### for linux

use    

```
sudo apt install csman
```

or    

```
sudo yum install csman
```

#### official homepage

goto our homepage for all platform downloading.    

coming soon...   

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
