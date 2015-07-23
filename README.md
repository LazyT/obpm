# **Omron Blood Pressure Manager (M500IT)**

## Description

Tired of Omrons original software because it's only available for Windows and requires an internet connection for uploading your private health data into the cloud? Then try OBPM for free and use it on Windows, Linux and MacOS!

The current version supports the following features:

* import your blood pressure and heart rate via usb directly from your device and keep it private on your pc
* export this data to csv or pdf documents
* analyse your records, zoom in/out, set time range, change diagram type, see minimum/average/maximum values
* multi language (english and german at the moment, translators welcome)

The main window...

![main window](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot1.png)

If you have more than 1 device connected select the right one...

![import dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot2.png)

Some infos and actions...

![about dialog](https://raw.github.com/LazyT/obpm/gh-pages/screenshots/screenshot3.png)

If you like it don't forget to donate some beers... ;-)

## Download Binaries

Download and install the latest version for your operating system:

* [Windows](https://github.com/LazyT/obpm/releases/download/1.0.0/OBPM-1.0.0-win.exe)
* [Linux](https://github.com/LazyT/obpm/releases/download/1.0.0/OBPM-1.0.0-lin.run)
* [MacOS](https://github.com/LazyT/obpm/releases/download/1.0.0/OBPM-1.0.0-mac.dmg)

## Build from Source

If the binaries doesn't work for your operating system build it yourself:

* checkout sourcecode

		git clone https://github.com/LazyT/obpm

* change into the new obpm directory and generate the Makefile

		cd obpm && qmake
        
* compile the source

		make

GUI fans just install the [Qt-Environment](http://www.qt.io/download-open-source), open the "obpm.pro" project file and click on the build button.

## Dependencies

OBPM is based on

* [Qt](http://www.qt.io)
* [QCustomPlot](http://www.qcustomplot.com)
* [hidapi](http://www.signal11.us/oss/hidapi)

Thanks for this great software!

Have a nice day and always a healthy blood pressure...
