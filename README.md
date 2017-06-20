# vChip
vopi's chip8 emu

## Screen Shots
![Screenshot 1](https://github.com/vopi181/vChip/blob/master/screenshots/screen1.png)


## Keys
```
1234
qwer
asdf
zxcv
```

## Building on Windows

1. Install VCPKG
2. Install SFML:x64-windows using VCPKG
3. Build (**WILL PROBABLY NEED TO CHANGE WHERE THE VCPKG FOLDER IS LOCATED IN THE CMAKEFILE**)
4. Copy DLLs (From VCPKG install folder to output dir)
5. Launch using vChip.exe "roms/ROMHERE"

## Building on Linux/Unix-like (OSX, *BSD)

Although I use linux daily, I happened to code the GUI specific parts on windows so I can't promise building it works perfectly

1. Install SFML (atleast system, window, graphics sfml libs) from your package manager (APT, pacman, brew, ports etc)
2. Try to build
3. If it works, play!
4. If not, mess with cmake to fix your issue finding SMFL libs
5. Please file in issue with the fix
6. Launch using ./vChip "roms/ROMHERE"