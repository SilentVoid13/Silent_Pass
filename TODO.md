# TODO List


## Globals 
- [ ] Refactor functions (parameter names, ...)
- [ ] Add Internet Explorer / Microsoft Edge support
- [ ] Add MAC OSX Support
- [ ] Create the README file
- [ ] Handle Windows arguments parsing
- [ ] Make clean Makefile (cMake ?)
- [x] Handle empty entries (only website entries / nothing)
- [x] Add all functions to header files

## For Chrome

### Global
- [ ] Implement Brave Browser support
- [ ] Better alternative to malloc / strncpy (memcpy / memset)
- [x] Handle cipher text length problem
### Linux
- [ ] Implement plain text / KWallet support
### Windows
- [ ] Find a better way to check for the version prefix
- [ ] Split decrypt_chrome_cipher() 
- [ ] Fix aead_decrypt() problem
- [ ] Add Argument Parsing
- [ ] Fix cJSON segfault problem (or change JSON lib)
- [x] Fix masterkey problem
- [x] Handle cipher text  padding

## For Firefox
- [ ] Choose the profile we want
- [x] Add Windows compatibility
- [x] Split main function
- [x] Enhance code
- [x] Add Master Password option
