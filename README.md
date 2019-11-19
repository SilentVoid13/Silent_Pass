# Silent_Pass

Silent_Pass is a cross platform browser credential harvester written in pure C.

It currently supports Linux and Windows OS.

I wrote this program mainly for entertainment and to learn more. My code is surely not perfect, do not hesitate to contact me for any problems / code enhancement suggestion at silentvoid13@protonmail.com.

## License

Silent_Pass is licensed under the GNU AGPLv3 license. Refer to [LICENSE](https://github.com/SilentVoid13/Silent_Pass/blob/master/LICENSE.txt) for more informations.

## Support

Silent_Pass currently support the following browsers :

- Google Chrome
- Mozilla Firefox
- Internet Explorer / Microsoft Edge
- Chromium

## Usage

```bash
Usage: Silent_Pass [-hVvafcs] [-m password] [-o myfile]
  -h, --help                Display this help and exit
  -V, --version             Display version info and exit
  -v, --verbose             Verbose output
  -a, --all                 Harvest all browsers credentials
  -f, --firefox             Harvest Firefox credentials
  -c, --chrome              Harvest Chrome-like credentials
  -s, --specific            Harvest OS Specific browsers credentials
  -m, --master=password     Master password to decrypt passwords (Firefox only)
  -o, --output=myfile       Ouput file
```

Master Password can only be used with Firefox (other browsers doesn't implement it).

Output file format is in CSV.

## Building

You can either get the compiled version from the latest release, from the bin directory, or compile it yourself.

You can build the Windows binary using mingw (i686-w64-mingw32-gcc). No 32 bits version for Linux is currently available.

Compiling time can be quite long because of the various dependencies (~1 min)

For Linux :

```bash
make
```

For Windows :

```bash
make win
```

Compiled binaries will be available in the bin directory.

## Dependencies

Currently Silent_Pass relies on the following dependencies :

- OpenSSL
- libsecret
- NSS

- cJSON
- argtable3

## Known bugs

Chromium dump on Windows can crash. This is due to the cJSON library that struggles when having to deal with a lot of data. The `cJSON_Parse()`  is the function that cause the SEGFault error.

## Contributing

Feel free to contribute. See the [TODO](https://github.com/SilentVoid13/Silent_Pass/blob/master/TODO.md) list. 

## Disclaimer

I am not responsible for what you do with the information and code provided. This is intended for professional or educational purposes only.