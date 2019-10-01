import os
from conans import ConanFile
from conans.errors import ConanInvalidConfiguration
from conans.tools import os_info, SystemPackageTool

class FreedomDevicetreeToolsConan(ConanFile):
    name = "freedom-devicetree-tools"
    # ~ version = "8.2.0"
    settings = "os_build", "arch_build"
    url = None # We might consider pointing on toolchain repo here
    description = """Freedom devicetree tools package"""
    license = "MIT"
    
    def build(self):
        self.run("./configure")
        self.run("make")

    # ~ def package_id(self):
        # ~ del self.info.settings.build_type

    # ~ def package(self):
        # ~ # Copy metal includes 
        # ~ self.copy("*.h", dst="include/metal", src="../metal/*")
        # ~ self.copy("*", dst="", src=self.name)

    # ~ def package_info(self):
                                        
    def requirements(self):
        print("requirements")
        if (os_info.linux_distro in ("debian", "ubuntu")) :
            pack_name = ["device-tree-compiler", "libfdt-dev"]
        if( os_info.linux_distro in ("arch", "manjaro")) :
            pack_name = ["dtc"]
            
        if pack_name:
            installer = SystemPackageTool()
            installer.install(pack_name)
            for package in pack_name :
                installer.install(package)
