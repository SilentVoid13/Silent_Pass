# TODO List

## Globals 
- [ ] Add Linux 32 bit support
- [ ] Add MAC OSX Support
- [ ] Add FileZilla support
- [ ] Add Opera support
- [ ] Add Brave Browser support
- [ ] Refactor functions (parameter names, ...)
- [ ] Make clean Makefile (use cmake ?)
- [x] Create the README file
- [x] Add Code Documentation
- [x] Add Internet Explorer V7-9 support 
- [x] Add Argument Parsing for Windows
- [x] Add Microsoft Edge - IE10 support
- [x] Handle empty entries (only website entries / nothing)
- [x] Add all functions to header files

## For Chrome
### Global
- [ ] Better alternative to malloc / strncpy (maybe memcpy / memset)
- [x] Handle cipher text length problem
### Linux
- [ ] Implement plain text / KWallet support
### Windows
- [ ] Fix cJSON segfault problem (or change JSON lib)
- [ ] Find a better way to check for the version prefix
- [x] Split decrypt_chrome_cipher() 
- [x] Fix aead_decrypt() problem
- [x] Fix masterkey problem
- [x] Handle cipher text  padding

## For Firefox
- [ ] Choose the profile we want (or try for every profile)
- [x] Add Windows compatibility
- [x] Split main function
- [x] Enhance code
- [x] Add Master Password option

## For Internet Explorer / MSEdge
- [ ] Fix output_file problem
- [ ] Fix Long password problem (Not adding to output file)
- [ ] Add functions to query full IE history (For IE7-IE9)
- [ ] Add IE11 Support (same as IE10 ?)
- [ ] Add IE6- Support ?
- [x] Add IE7-IE9 Support
- [x] Test Vault for Windows 7
