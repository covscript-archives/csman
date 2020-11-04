# CSMAN Developing Document
A package manager for Covscript.
## 0.Usage
+ **csman install <package name> [--redo | --fix]**: install a package
	+ **csman uninstall <package name> [--clean]**: uninstall a package
	+ **csman checkout <stable | version>**: Toggle the currently used version of CovScript(runtime).
	+ **csman run [-v version] <command>**:Run specific version directly.
	+ **csman list**: Show all packages installed.
	
	
+ Configuration Manager: Command line configuration manager
	+ **csman config set <key=value>**: set a variable of a key
	+ **csman config unset <key>**: unset a variable of a key
	+ **csman config get <key>**: show a variable of key

## 1.Requirement abstraction
+ Installing (packages & runtimes).
+ Deleting (packages & runtimes).
+ Checking out (packages & runtimes).
## 2.Convention

+ The **DIR Construction** of csman:
    
    + **/~/.csman_config.json**: to tell csman all details
    + **csmanPath/** (default = ~/.csman/)  
        + source.json 
        + packages.json   
        + error_log.txt    
            
+ Description of **some json files** on **server**(except [csman.json](https://csman.info/csman.json), all URLs are from json file.):
    + **csman.json:**
        + URL: https://csman.info/csman.json
        + description: refer to required files for building **source.json**.
    + **paltform.json:**
        + URL: BaseUrl(from csman.json) + Platform(from csman.json).json
        + description: packages information for local platform
    + **generic.json:**
        + URL: BaseUrl(from csman.json) + Generic.json
        + description: packages information for local platform
    + **source.json:**
        + COVSRIPT_HOME: environment variable, where the CovScript installed
        + CS_IMPORT_PATH: environment variable, where the packages exist
        + CS_DEV_PATH: environment variable, where the SDK exist
        + CsmanPath: home path/.csman/
        + MaxReconnectTime: number all Internet operations' max reconnection
+ **.csman_config.json:**
    + CsmanPath: ~/
    + description: configuration file for local csman.
+ Description of nomenclature
    + **STD:**  STD\(0-9\){4}\(0-9A-F\){2}
    + **ABI:** same as STD
    + **Package:** (\w+\\.)\*\w
    + **Version of Package:** (\[0-9\]+\\.){0, 3}(0-9)+\w\*
    
## 3.Function abstraction
+ **Updating Sources:** update source from server when launching csman so it can get all packages information latest and correctly.
    + Connect to [csman.json](https://csman.info/csman.json) which should guide csman where to access.
    + Then, merge **BaseUrl+Platform.json** and **BaseUrl+Generic.json** as **ONE FILE** that we call it **"source.json"**, two files shouldn't have anything repeated.
        + Two files are online file, the **"source.json"** is local.
        + To get two files, need to use **HttpGet Request** via **Https Protocol**. 

+ **Installing:**  
    + **Checking:** make sure which package have to install. This Part should mentioned again latter
    + **Downloading:** download packages from **source.json -> ContentUrl**.
    + **Decompression:** decompress **zip file** after download.
    + **Fitting Covsript:** write library into **evn_variable** ~~developing...~~
    + **Registering local:** register information into **config.json** 
    
+ **Uninstalling:**
    + **Deleting:** delete packages from .csman/packages, and delete library from **evn_variable** also.
    + **Unregistering local:** delete data from **config.json**
    
+ **~~More...~~**