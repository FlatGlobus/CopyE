CopyE a various of copy command. It finds files in destination directory by masks/filenames from source directory and replaces them. 
Command line syntax:
Source : -s <masks/files> for example -s *.txt c:\temp\todo.doc ?oo.tx? 
Destination : -d <directory> for example -d c:\doc
Recursive: -r <sd> s - source, d - destination. for example -r sd recursive finds files in source and destination directories.

copye -s c:\doc\*.txt -d c:\ -r sd replace all *.txt files in c:\ and it's subcatalogs from c:\doc and its subcatalogs
copye -s c:\doc\*.txt -d c:\ -r s replace all *.txt files in c:\ from c:\doc and its subcatalogs
