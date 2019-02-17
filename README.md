# stlconvert
**stlconvert** is a program that converts STL from ASCII format to binary and vice versa.

## Usage
**stlconvert** automatically detects input STL type and converts it to the opposite type. Arguments are following:
```
$ stlconvert INPUT OUTPUT
```
Example:
```
$ stlconvert input-ascii.stl output-binary.stl
```
## Building
On Linux, simply call `make`. After that, you can use `stlconvert` that was built in the repository root:
```
$ make
$ ./stlconvert
```
