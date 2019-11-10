#ifndef FIREFOX_WIN_H
#define FIREFOX_WIN_H

typedef enum {
    siBuffer,
    siClearDataBuffer,
    siCipherDataBuffer,
    siDERCertBuffer,
    siEncodedCertBuffer,
    siDERNameBuffer,
    siEncodedNameBuffer,
    siAsciiNameString,
    siAsciiString,
    siDEROID
} SECItemType;
 
struct SECItemStr {
    SECItemType type;
    unsigned char *data;
    unsigned int len;
};
 
typedef enum {
    SECWouldBlock = -2,
    SECFailure = -1,
    SECSuccess = 0
} SECStatus;
 
typedef struct SECItemStr SECItem;

typedef SECStatus (*NSSInit)(char *);
typedef void *(*PK11GetInternalKeySlot)();
typedef SECStatus (*PK11SDRDecrypt)(SECItem *, SECItem *, void *);
typedef SECItem* (*NSSBase64DecodeBuffer)(void *ptr, SECItem *, char *, unsigned int);
typedef SECStatus (*PK11Authenticate)(void *, int, void *);
typedef SECStatus (*PK11CheckUserPassword)(void *, char *);
typedef SECStatus (*NSSShutdown)();
typedef void (*PK11FreeSlot)(void *);

// Added
typedef char *(*fpPL_Base64Decode)(const char *src, int srclen, char *dest);
fpPL_Base64Decode PL_Base64Decode; 
//typedef SECStatus (*SECItemFreeItem)(SECItem *, PRBool);
//SECItemFreeItem SECItem_FreeItem;

#endif // FIREFOX_WIN_H
