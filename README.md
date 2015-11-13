# **Omron Blood Pressure Manager for M500IT**

## Description

Tired of Omrons original software because it's only available for Windows and requires an internet connection for uploading your private health data into the cloud? Then try OBPM for free and use it on Windows, Linux and MacOS!

The current version supports the following features:

* import blood pressure and heart rate via usb from the device and keep it private on your pc
* export to csv/pdf documents or print it
* analyse, zoom, set time range, change diagram type, see minimum/average/maximum values
* multi language (English and German at the moment, translators welcome)

## Screenshots

The main window with some records...

![main window](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot1.png)

Print records for your archive or doctor...

![main window](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot5.png)

If you have more than 1 device connected select the right one based on the serial number...

![import dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot2.png)

Some infos about the program and possible interactions...

![about dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot3.png)

If you like it don't forget to donate some beer! :smirk:

The installer...

![setup dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot4.png)

##### Hint for Linux users

If you get permission denied errors you should create a udev rule:

* disconnect device
* create file (e.g. "/etc/udev/rules.d/99-obpm.rules") as root and add the following line

		KERNEL=="hidraw*", ATTRS{idVendor}=="0590", ATTRS{idProduct}=="0090", MODE="0666"

* reconnect device

## Download Installer

Download and install the latest version for your operating system:

* [Windows](https://github.com/LazyT/obpm/releases/download/1.0.2/OBPM-1.0.2-win.exe)
* [Linux](https://github.com/LazyT/obpm/releases/download/1.0.2/OBPM-1.0.2-lin.run)
* [MacOS](https://github.com/LazyT/obpm/releases/download/1.0.2/OBPM-1.0.2-mac.dmg)

## Build from Source

If the installer binaries doesn't work for you build it yourself:

1) checkout the source code

		git clone https://github.com/LazyT/obpm

2) change into the new obpm directory and generate the Makefile

		cd obpm && qmake

3) compile the source code

		make

**Hint:** Qt5.4 or greater required!

GUI fans just install the [Qt-Environment](http://www.qt.io/download-open-source), open the "obpm.pro" project file and click on the build button.

## Dependencies

OBPM is based on

* [Qt](http://www.qt.io)
* [QCustomPlot](http://www.qcustomplot.com)
* [hidapi](http://www.signal11.us/oss/hidapi)

Thanks for this great software!

Have a nice day and always a healthy blood pressure...
