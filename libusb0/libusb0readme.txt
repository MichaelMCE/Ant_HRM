
This is a middle man wrapper which intercepts all libusb0 communications.

libusb0.dll: 
This Dll sits between your application and libusb0.dll, intercepting all calls to libusb0.dll, dumping the data to console, before posting onward to _libusb0.dll

_libusb0.dll:
This is an unmodified libusb0.dll file renamed to _libusb0.dll which is dynamically loaded by the above DLL.

Put both of these in to your application folder when debugging - do not ship!
