![alt text](Logo.png "AirFreight Logo") # AirFreight
A tool to check Serial Numbers in Packages (e.g. air freight)

###### Developed for a company to check their icoming freight.

## Usage
You need a csv Table with Serial Numbers or any other strings you want to check (the program doesn't care). When you open it it will ask you to insert the name of the file and will load the numbers in it.

After that you can check if every number from your table is present, like in a package for example. If you want to seperate the numbers, you can press p and Enter to create a new package.

At the end AirFreight is going to write text files with the numbers in the right package (Also PDF147 Codes from v3).

## Installation
If you just want to check some numbers, download the executable in the folder of each version. If you want to compile it on your own, download the cpp file and compile it, with gcc for example.

If you need AirFreight a bit longer you may want to download the installer and have it in your app directory (from v3_GUI on)

The program is written for windows, if someone wants to port it to linux, feel free to do so.

## Thanks
Thanks to the programmers of [zint](https://github.com/zint/zint) (Barcode Generator) and Bill Stewart for [PathMgr](https://github.com/Bill-Stewart/PathMgr/) (dll to add AirFreight to PATH variable).
