# imcap-kwin
imcap = "Image Capture"

Screenshot tool for KDE Plasma / KWin desktops. Wayland compatible.


This program will take a snapshot of the active screen, save a (timestamp).png image file to `/tmp/USER/screenshot/`, and exit.


This is not a full featured screenshot editor, only a capture tool.
Use an image editor of your choice for further processing. (See my 'imcrop' tool.)
It's meant to be small and fast, do its job and get out of your way, and not bother you with popups and long load times that break the flow.


Be aware that on many linux systems, /tmp is cleared upon reboot. So if you want to keep your screenshots, you need to move them to a permanent location.
I designed it this way to be friendlier to my SSD, although perhaps it's not a big deal. That's just how it works, for now.
It's really just a quick hack for my personal benefit, but if it might be useful to you, here you go.


## Setup

This program can be built and installed with CMake. If you are unfamiliar with this process, there are many tutorials online. Try installing the package `cmake-gui` from your repo's package manager and run it. Select the folder with the source code, then a temporary output directory for compiling, hit Generate, and choose your compiler. If using the default "Unix makefile" option, open a terminal in that temp build directory and type "make && make install"


In your KDE Plasma system settings, you can now go to Workspace > Shortcuts, and add a new application. Point it at `/usr/share/applications/imcap-kwin.desktop` and set the key to PrintScr or whatever you want your global shortcut to be. Hit Apply and try it out. A png image should appear in `/tmp/YOU/screenshot/`.


If the thing compiles, but the automatic install fails for some reason and you don't see a .desktop file there, here is what needs to happen. First, copy the executable file "imcap-kwin" to `/bin` or `/usr/bin`. Next, there is a .desktop file in the source tree that must be copied to `/usr/share/applications`. This is required to grant permission to take screenshots on a Wayland session. If you want to change the location of the executable, this .desktop file must be modified accordingly. Use any text editor to open it.


