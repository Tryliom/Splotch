# Splotch
A multiplayer game where you drop rectangles to stomps the opponent.

## Setup - CLion

First, you need to install the following dependencies:
- CMake
- VCPKG
- WSL2 (Windows Subsystem for Linux) with Ubuntu 22.04

### WSL2
First, you need to install WSL2. You can follow the instructions on the [official documentation](https://docs.microsoft.com/en-us/windows/wsl/install).

Second, you need to install Ubuntu 22.04. You can follow the instructions on the [official documentation](https://docs.microsoft.com/en-us/windows/wsl/install#install-a-linux-distribution).

Don't forget to update your system with the following commands:
```bash
sudo apt update
sudo apt upgrade
```

And install the following dependencies

#### VCPKG
You need to install [vcpkg](https://vcpkg.io/en/getting-started.html) and run the following command:
```bash
vcpkg integrate install
```

#### SFML dependencies
```bash
sudo apt-get install libx11-dev libxrandr-dev libxi-dev libudev-dev libgl1-mesa-dev
```

During this install, you can encounter the following error:
```
libcuda.so.1 is not a symbolic link on WSL2
```

If this happens, open powershell or cmder as admin and run `cmd` or cmd in admin mod

```
cd \Windows\System32\lxss\lib
del libcuda.so
del libcuda.so.1
```

Work on cmder but not powershell
```
mklink libcuda.so libcuda.so.1.1
mklink libcuda.so.1 libcuda.so.1.1
```

Then, run again the following command:
```bash
sudo apt-get install libx11-dev libxrandr-dev libxi-dev libudev-dev libgl1-mesa-dev
```

### CLion
You need to install CLion. You can follow the instructions on the [official documentation](https://www.jetbrains.com/clion/download/).

You need to create cmake profile with the following settings.
#### Debug-windows
![Debug-_client](./documentation/images/debug-_client.png)

Fill the CMAKE options with the following values:
```
-DCMAKE_TOOLCHAIN_FILE=<Link to your vcpkg.cmake> -DPORT:INT=43845 -DHOST_NAME:STRING="localhost"
```
The `PORT` and `HOST_NAME` are the default values. You can change them if you want, but you need to change the debug-server values too.

Example:
```
-DCMAKE_TOOLCHAIN_FILE=C:\tools\vcpkg\scripts\buildsystems\vcpkg.cmake -DPORT:INT=43845 -DHOST_NAME:STRING="localhost"
```

#### Release-windows
![Release-_client](./documentation/images/release-_client.png)

Fill the CMAKE options with the following values:
```
-DCMAKE_TOOLCHAIN_FILE=<Link to your vcpkg.cmake> -DPORT:INT=43845 -DHOST_NAME:STRING="<Your server IP>"
```

#### Release-linux
![Release-server](./documentation/images/release-server.png)

Fill the CMAKE options with the following values:
```
-DCMAKE_TOOLCHAIN_FILE=<Link to your vcpkg.cmake in WSL> -DPORT:INT=43845
```

Example:
```
-DCMAKE_TOOLCHAIN_FILE=~/dev/vcpkg/scripts/buildsystems/vcpkg.cmake -DPORT:INT=43845
```

You need to run each profiles for them to recognize HOST_NAME and PORT.