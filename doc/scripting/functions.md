# Functions
There are functions to create variable contents.
The functions are grouped by **scopes**. 

## Chapter Overview
- [Searchengine](searchengine.md)
- [Basic statements](basic_statements.md)
- [File and buffer related statements](file_statements.md)
- [Search and replace related statements](search_statements.md)
- [Functions](functions.md)

## Table of Content
- [Scope Buffer](#Scope Buffer)
    - buffer.difference
	- buffer.join
	- buffer.pop
	- buffer.shift
	- buffer.sort
	- buffer.split
- [Scope Math](#Scope Math)
	- math.random
- [Scope Os](#Scope Os)
	- os.basename
	- os.cd
	- os.changeextension
	- os.copy
	- os.dirname
	- os.exists
	- os.listfiles
	- os.pwd
	- os.popd
	- os.pushd
	- os.tempname
- [Scope String](#Scope String)
	- string.index
	- string.length
	- string.piece
	- string.replace
	- string.search
	- string.substring

## Scope Buffer
Offers some functions on buffers.

### buffer.difference
Returns the line number where two buffers are different.
Returns 0 if there is no difference.

#### Syntax
    buffer.difference <buffer1> <buffer2>

#### Examples
    diff := buffer.difference ~origin ~new
    if diff == 0
      log "buffers are equal"
    else
      log "first difference in line $(diff)"
    fi
	
### buffer.join
Puts all lines of a buffer into a result string.

#### Syntax
    buffer.join <buffer> [<separator>]

#### Examples
    dummy := buffer.split ~list "d,e,f" ","
    dummy = buffer.sort ~list
	list = buffer.join ~list ","
	
### buffer.pop
Returns the last non empty line as result. Removes that line from the buffer.

#### Syntax
    buffer.pop <buffer>

#### Examples
    file = buffer.pop ~files
	current = buffer.pop ~csv
	
### buffer.shift
Returns the first non empty line as result. Removes that line from the buffer.

#### Syntax
    buffer.shift <buffer>

#### Examples
    file = buffer.unshift ~files
	current = buffer.unshift ~csv
### buffer.sort
Sorts the buffer contents by lines. Returns the number of lines.

#### Syntax
    buffer.sort <buffer>

#### Examples
    count = buffer.sort ~files
	
### buffer.split
Splits a text into lines and store that in a buffer. Returns the number of created lines.

#### Syntax
    buffer.split <buffer> <string> [separator]

#### Examples
    count = buffer.split ~names $(data) ","
	
## Scope Math
Offers some mathematical functions.

### math.random
Returns a random number.

#### Syntax
    math.random <maximum> [<minimum>]

#### Examples
    fileNo = math.random 10
	month = math.random 28 1
	
## Scope Os
Offers some functions of the operating system: Operates on files/directories...

### os.basename
Returns the filename of a given file without path.

#### Syntax
    os.basename <filename>

#### Examples
    node =  os.basename $(fullname)
	
### os.cd
Returns the current directory and than changes it to a given path.
If the path does not exists, an empty string is returned.

#### Syntax
    os.cd <path>

#### Examples
    node =  os.cd "/home"
	
### os.copy
Copies a file. Returns 1 on success or 0 on error.

If the keyword "unique" is found the name is modified if the target already exists.

#### Syntax
    os.copy <source-name> <target-name> [unique]

#### Examples
    rc = os.copy "$(source)" "/tmp/$(source).safe"
    rc = os.copy "/config/mydata" "/tmp" unique
	
### os.changeextension
Sets the file extension of a given filename.

#### Syntax
    os.changeextension <filename> <new-extension>

#### Examples
    output =  os.changeextension $(input) ".out"
	
### os.dirname
Returns the path of a given filename.

#### Syntax
    os.dirname <filename>

#### Examples
    path =  os.dirname "$(file)"
	
### os.exists
Tests whether a file exists. Returns 1 if the file exists, 0 otherwise.

#### Syntax
    os.exists <filename>

#### Examples
    exists =  os.exists "/home/data"
	
### os.isdir
Tests whether a file is a directory. Returns 1 if the file is a directory, 0 otherwise.

#### Syntax
    os.isdir <filename>

#### Examples
    dummy =  os.isdir "/home/data"
	
### os.listfiles
Puts the filenames of a given directory into a buffer. Returns the count of files.

If at least one of the keywords "files" "dirs" or "links" is given than only that kind of files will be found.
Otherwise all kind of files will be found.

#### Syntax
    os.listfiles <buffer> <directory> including <search_expression>] [excluding <search_expression>] [files] [dirs] [links]

#### Examples
    countFiles =  os.listfiles ~files "/home" including r/\.[ch]pp$/ excluding r/test/i files
    countDirs =  os.listfiles ~dirs "/workbench" excluding r/test/i dirs
	
### os.mkdir
Creates a directory. Returns 1 on success and 0 on failure. If the director yet exists nothing is done.

#### Syntax
    os.mkdir <directory>

#### Examples
    success =  os.mkdir "/home/jonny/data"
	
### os.pwd
Returns the current directory.

#### Syntax
    os.pwd

#### Examples
    path =  os.pwd
	
### os.popd
Returns the last entry from the directory stack and change the current directory to that.
This is the counterpart of **os.pushd**.

#### Syntax
    os.popd

#### Examples
    path =  os.popd
	
### os.pushd
Puts the current directory onto a directory stack and change the current directory to a given path.
This is the counterpart of **os.popd**.

#### Syntax
    os.pushd <path>
#### Examples
    path =  os.pushd "/src/cpp"
	
### os.tempname
Returns a unique temporary filename.

#### Syntax
    os.tempfile <name> [<suffix>] [in <sub-directories>]

- &lt;name>: the returned filename starts with that string.
- &lt;suffix>: the returned filename ends with that string. If that suffix is given a unique filename is created.
- &ltsub-directories>: specifies a subdirectory in the temporary directory.

#### Examples
    file =  os.tempname "test_data.csv"
    file =  os.tempname "test" ".csv" in "unittest/csv"


## Scope String
Offers some functions of the operating system: Operates on files/directories...

### string.length
Replaces length of a string.

#### Syntax
    string.index <element> <list> [<separator>]

#### Parameters:
- __element__: that element will be searched.
- __list__: a list with items separated by a separator.
- __separator__: the separator between the items in the list. Default: " " (blank).
- __returns__: -1: not found. Otherwise: the index (0 based) of the element in the list.

#### Examples
    ix := string.index "2" "1 2 4 8 16"
    id := string.index "alice" "alice,bob,charly" ","
    
### string.length
Replaces length of a string.

#### Syntax
    string.length <string>

#### Examples
    length = string.length "$(node)"
    
### string.piece
Returns an element of a given list given by the index. That list is a string with a given separator: default is the blank " ".
If the index is out of range the empty string is returned.

#### Syntax
	string.piece <index> <list> [<separator>]

#### Parameters:
- __index__: the string with that index in the list will be returned.
- __list__: a string with the items separated by the separator.
- __separator__: the separator between the items in the list. Default: " " (blank).
- __returns__: the specified item from the list.

#### Examples
    name = string.piece "$(id)" "alice bob charly"
    unit = string.piece 3 "nanometer,micrometer,millimeter,meter,kilometer" ","
    
### string.replace
Replaces text in another text. Backreferences (syntax $<no>) can be used.

#### Syntax
    string.replace <string> <pattern> <replacement> [count=<count>]

#### Parameters:
- __string__: the string to inspect.
- __pattern__: a search expression.
- __replacement__: the found pattern will be replaced by that string.
- __count__: if given only that count of searches will be done.
- __returns__: the modified string.

#### Examples
    node = string.replace "$(node)" r/^(\w+)\.cpp/ "~1_test.cpp"
    name = string.replace '$(name)' r/\s*/ ?_?
    definition = string.replace "$definition" r/\s*=\*/ "=" count=1

### string.search
Search a regular expression in a string. 
The result is a string. The first part of the string is the whole string matching the pattern.
If there are groups (defined by parenteses) that will returned as a list in a string separated by a given separator.
If the pattern is not found an empty string is returned.

#### Syntax
    string.search <string> <pattern> [<separator>]

#### Parameters:
- __string__: the string to inspect.
- __pattern__: a search expression
- __separator: the separator of the result string between the groups. Default: " "
- __returns__: the string with the found groups.

#### Examples
    hits = string.search "hello.cpp" r/^(\w+)(\.?pp)/ ";"
    if $(hits) -ne "hello.cpp;hello;.cpp"
      stop "error"
    endif
    extension = piece "$(hits)" 2 ";"

### string.substring
Returns a part of a given string

The start position can be specified with "from" (including) and "behind" (excluding).
The end position can be specified with "excluding" or "including" or with the specification "count".

#### Syntax
    string.substring <string> <start> [{ count <count> | {including | excluding} <end_position>]]

#### Parameters:
- __start__: the position of the first character to copy: 1..N
- __count__: the maximum count of characters to copy
- __end_position__: the last position to copy: including or excluding
- __returns__: the substring.

#### Examples
    name1 = string.substring "$(name)" from 3
    name2 = string.substring /$(name)/ from 3 count 2 
    name3 = string.substring =$(name)= from 3 including 5
    name4 = string.substring '$(name)' behind $(pos1) excluding 9
    name5 = string.substring "$(name)" excluding 7
    name5 = string.substring "$(name)" count 3
    
	
