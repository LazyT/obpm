# **Omron Blood Pressure Manager**

## Description

Tired of Omrons original software because it's only available for Windows and requires an internet connection for uploading your private health data into the cloud? Then try OBPM for free and use it on Windows, Linux and MacOS!

The current version supports the following features:

* Omron HEM-7322U (M500 IT, M6 Comfort IT) and HEM-7131U (M400 IT, M3 IT)
* import blood pressure and heart rate via usb from device or csv/sql files and keep it private on your pc
* export to csv/sql/pdf documents or print it
* analyse, zoom, set time range, change diagram type, see minimum/average/maximum values
* context sensitive help via F1 key
* online update notification
* multi language (English and German at the moment, translators welcome)
* cross platform (same look & feel on Windows, Linux, MacOS)

## Screenshots

The main window with some records...

![main window](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot1.png)

Print records for your archive or doctor...

![printouts](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot5.png)

Configure your preferred settings...

![setup dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot8.png)

If you have more than 1 device connected select the right one based on the serial number and choose your model...

![import dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot2.png)

Some infos about the program and possible interactions...

![about dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot3.png)

If you like it don't forget to donate some beer and support further development...

![donate dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot6.png)

Keep informed about updates automatically at program start...

![update dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot7.png)

Press F1 to get help for the current context...

![help window](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot9.png)

The installer...

![installer dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot4.png)

***Hint for Linux user:***

If you get permission denied errors you should create a udev rule:

* disconnect device
* create file (e.g. "/etc/udev/rules.d/99-obpm.rules") as root and add the following line

		KERNEL=="hidraw*", ATTRS{idVendor}=="0590", ATTRS{idProduct}=="0090", MODE="0666"

* reconnect device

## Download Installer

Download and install the latest version for your operating system:

* [Windows](https://github.com/LazyT/obpm/releases/download/1.0.5/OBPM-1.0.5-win.exe)
* [Linux](https://github.com/LazyT/obpm/releases/download/1.0.5/OBPM-1.0.5-lin.run)
* [MacOS](https://github.com/LazyT/obpm/releases/download/1.0.5/OBPM-1.0.5-mac.dmg)

## Build from Source

If the installer binaries doesn't work for you build it yourself:

1) checkout the source code

		git clone https://github.com/LazyT/obpm

2) change into the new obpm directory and generate the Makefile

		cd obpm && qmake

3) compile the source code

		make

GUI fans just install the [Qt-Environment](http://www.qt.io/download-open-source), open the "obpm.pro" project file and click on the build button.

***Hints for compiling:***

 - Qt5.4 or greater required
 - libudev-dev on Linux required

## Credits

OBPM is based on

* [Qt](http://www.qt.io)
* [QCustomPlot](http://www.qcustomplot.com)
* [hidapi](http://www.signal11.us/oss/hidapi)

Thanks for this great software!

Have a nice day and always a healthy blood pressure... :smirk:
