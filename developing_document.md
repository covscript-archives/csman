# CSMAN Developing Document
A package manager for Covscript.
## 1.Requirement abstraction
+ Installing (packages & runtimes).
+ Deleting (packages & runtimes).
+ Checking out (packages & runtimes).
## 2.Convention

+ The **DIR Construction** of csman:
    
    + **/~/.csman_config.json**: to tell csman all details
    + **/~/.csman_installed**: what had csman isntalled
    + **csmanPath/** (default = ~/.csman/)  
        + source.json    
        + error_log.txt    
        + **packages/**    
            + somePackages...
        + **runtimes/**    
            + someRuntimes...    
            
+ Some description of **some json files**(except [csman.json](https://csman.info/csman.json), all URLs are from json file.):
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
        + path: home path/.csman/
        + description: packages complete information for local csman, merged from **platform.json and Generic.json** above.
    + **.csman_config.json:**
        + path: home path/.csman/
        + description: configuration file for local csman.
+ Some description of nomenclature in **source.json**
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