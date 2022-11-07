# Silent_Pass

Silent_Pass is a cross-platform software credential harvester written in pure C.

It currently supports Linux and Windows OS. 32 bit version is available, so it should work on most of the machines, even older ones.

## License

Silent_Pass is licensed under the GNU AGPLv3 license. Refer to [LICENSE](https://github.com/SilentVoid13/Silent_Pass/blob/master/LICENSE.txt) for more informations.

## Support

Silent_Pass currently support the following softwares :

- Google Chrome
- Mozilla Firefox
- Internet Explorer / Microsoft Edge
- Chromium
- FileZilla
- Git

You can suggest me a software you want me to add.

## Usage

```bash
Usage: Silent_Pass [-hVvafcsFg] [--master-firefox=password] [--master-filezilla=password] [-o filename]
  -h, --help                Display this help and exit
  -V, --version             Display version info and exit
  -v, --verbose             Verbose output
  -a, --all                 Harvest all softwares credentials
  -f, --firefox             Harvest Firefox credentials
  -c, --chrome              Harvest Chrome-like credentials
  -s, --specific            Harvest OS Specific softwares credentials (currently: IE / MSEdge for Windows)
  -F, --filezilla           Harvest FileZilla credentials
  -g, --git                 Harvest Git credentials
  --master-firefox=password Master password to decrypt passwords for Firefox
  --master-filezilla=password Master password to decrypt passwords for FileZilla
```

Master Password option can only be used with Firefox (currently not working on FileZilla).

Output file format is in [CSV](https://en.wikipedia.org/wiki/Comma-separated_values).

## Building

You can either get the compiled version from the latest [release](https://github.com/SilentVoid13/Silent_Pass/releases), from the [bin](https://github.com/SilentVoid13/Silent_Pass/tree/master/bin) directory, or compile it yourself.

To build it yourself you will need `cmake`

You can build the Windows binary from Windows using `mingw`. You can also cross-compile it from Linux.

You might experience some trouble when compiling with an old version of `gcc` or `mingw` (Tested and working with version >= 8.3)

Compiling time can be quite long because of the various dependencies (~1 min).

#### On Linux

For Linux compiling, you will need `gcc / g++` :

```bash
mkdir build && cd build
cmake ..
make
make install
```

To cross-compile for Windows, you will need `mingw` and more precisely `i686-w64-mingw32-gcc / i686-w64-mingw32-g++` (32 bit version) :

```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=platforms/linux/mingw32.toolchain.cmake ..
make
make install
```

To compile the 32 bit Linux version from a 64 bit Linux machine (you will need to download all the dependencies 32 bit version) :

```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=platforms/linux/linux_i686.toolchain.cmake ..
make
make install
```

#### On Windows

For Windows compiling you will need [`mingw`](https://sourceforge.net/projects/mingw-w64/)  :

```bash
mkdir build & cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
mingw32-make install
```

Compiled binaries will be available in the **bin** directory.

## Dependencies

Currently, Silent_Pass relies on the following dependencies :

- libopenssl
- libsecret
- libnss3
- gnome-keyring
- pkg-config
- iniparser
- argtable3
- sqlite3
- cJSON
- zlib
- libxml2
- libyuarel

## Known bugs

Chromium dump on Windows can crash. This is due to the cJSON library that struggles when having to deal with a lot of data. The `cJSON_Parse()`  is the function that cause the SEGFault error.

## Contributing

Feel free to contribute. See the [TODO](https://github.com/SilentVoid13/Silent_Pass/blob/master/TODO.md) list. 

## Disclaimer

I am not responsible for what you do with the information and code provided. This is intended for professional or educational purposes only.
