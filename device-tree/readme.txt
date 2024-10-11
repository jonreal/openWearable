Use 'BeagleBoard-DeviceTrees'
https://github.com/beagleboard/BeagleBoard-DeviceTrees

ln -s <device-tree> ~/BeagleBoard-DeviceTrees/src/arm
cd ~/BeagleBoard-DeviceTrees
make install_arm

Compiled device trees:
/boot/dtbs/<kernel>/
