import os
from conans import ConanFile
from conans.errors import ConanInvalidConfiguration, ConanException
from conans.tools import os_info, SystemPackageTool

class FreedomDevicetreeToolsConan(ConanFile):
    name = "freedom-devicetree-tools"
    version = "v201905"
    settings = "os"
    url = None # We might consider pointing on toolchain repo here
    description = """Freedom devicetree tools package"""
    # exports_sources is use to build from source
    exports_sources = "*"
    license = "MIT"
    
    def build(self):
        if(self.settings.os != "Linux"):
            raise ConanInvalidConfiguration("unsupported os: %s" %
                                            (self.settings.os))
        try : 
            self.run("./configure")
        except ConanException:
            self.run("make distclean")
            self.run("./configure")
        self.run("make -j")

    def package(self):
        # Copy metal includes 
        self.copy("freedom-*", dst="bin", src="", excludes= "*.c++")

    def package_info(self):
        if(self.settings.os != "Linux"):
            raise ConanInvalidConfiguration("unsupported os: %s" %
                                            (self.settings.os))
                                            
        # ~ self.env_info.path.append(self.package_folder)                                
        # ~ bin_folder = os.path.join(self.package_folder, "bin")
        # ~ self.env_info.path.append(bin_folder)
                                            
    def requirements(self):

        if (os_info.linux_distro in ("debian", "ubuntu")) :
            pack_name = ["device-tree-compiler", "libfdt-dev"]
        if( os_info.linux_distro in ("arch", "manjaro")) :
            pack_name = ["dtc"]
            
        if pack_name:
            installer = SystemPackageTool()
            installer.install(pack_name)
            for package in pack_name :
                installer.install(package)
