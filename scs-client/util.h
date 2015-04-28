#ifndef _UTIL_H_
#define _UTIL_H_

// base64 encode bytes.  The output buffer must have at least
// ((4 * (inLen + 1)) / 3) bytes in it.  Returns the number of bytes written
// to [out].
int base64Encode(const unsigned char *in, int inLen, char *out);

// Compute HMAC-SHA-1 with key [key] and message [message], storing result
// in [hmac]
void HMAC_SHA1(unsigned char hmac[20], const unsigned char *key, int key_len,
			   const unsigned char *message, int message_len);


std::string URLEncode(const std::string& sIn);
std::string URLDecode(const std::string& sIn);

CString format_size(unsigned long long size);

extern std::string CString2string(CString str);
extern CString string2CString(const std::string& str);

std::wstring MultiByteToWide(const std::string& mb, UINT32 code_page);
std::string WideToMultiByte(const std::wstring& wide, UINT32 code_page);
std::string MultiByteToUtf8(const std::string& mb);
std::string Utf8ToMultiByte(const std::string& utf8);

BOOL GetAppDataPath(CString& strPath);
void GetExePath(CString& strPath);
BOOL DirectoryExists(const CString& strPath);
BOOL FileExists(const CString& strPath);

std::string EncryptString(const std::string& str);
std::string DecryptString(const std::string& str);

#endif