# textknife

## Objectives
That program offers services for text searching/manipulating.

## Links
- [Search Enginge Script](searchengine.md) For more complex things.

## Examples
```
(# Adapt standard values for the PHP configuration:
textknife adapt --template=php /etc/php/8.2/php*.ini
# Show the adapting parameters:
cat /etc/cppknife/adapt/php.conf

# Adapt a configuration file: change the variable "max_memory" if it exists or set it otherwise.
textknife adapt --anchor=/#.*max_memory/i '--pattern=/^max_memory\s*=/' "--replacement=max_memory=512k" /etc/php/8.2/fpm/php.ini

# Find the strings in all sourcefiles (*.cpp and *.hpp) in the directory /home/ws and subdirs:
textknife strings /home/ws/*.cpp,*.hpp
# Find strings in files older than 7 days and maximum nesting depth of 3:
textknife strings /home/ws/*.cpp,*.hpp --days=+7 --max-depth=3

# Describe the usage:
textknife --help
```

## Usage

```
textknife [<options>]  MODE
    Managing text on files in a directory tree
  -?,--help
    Shows the usage information., e.g. --help -?
  -l LOG-LEVEL,--log-level=LOG-LEVEL
    Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG, e.g. --log-level=123 -l0
  -v,--verbose
    Show more information, e.g. --verbose -v
  --examples
    Show usage examples, e.g. --examples --examples --examples --examples --examples
  MODE
    What should be done:
    adapt
      Adapts configuration files.
    search
      Searches regular expressions.
    strings
      Fetches the strings delimited by ' or " from files.
textknife adapt [<options>]  SOURCE BASE
    Adapts configuration files.
  -?,--help
    Shows the usage information., e.g. --help -?
  -t TEMPLATE,--template=TEMPLATE
    Use a template: php, e.g. --template=php
  -P PATTERN,--pattern=PATTERN
    The pattern describing the key., e.g. --pattern=/^max_memory\s*=
  -R REPLACEMENT,--replacement=REPLACEMENT
    The replacement of the pattern, e.g. --replacement=max_memory = 512k
  -a ANCHOR,--anchor=ANCHOR
    If pattern is not found the replacement is inserted near that anchor., e.g. --anchor=/#.*max_memory
  -A,--above-anchor
    Insert above the anchor line., e.g. --above-anchor -A
  SOURCE
    A directory with or without a list of file patterns., e.g. /etc/php/8.4/*.conf
  BASE
    The start directory or a list of file patterns delimited by ',', first with path. Preceding '-' defines a NOT pattern, e.g. . /home/jonny/*.c,*.h,-*tmp*
  -f FILES,--files=FILES
    Only files matching that patterns will be found, e.g. --files=;*.cpp;*.hpp;-test*
  -p DIRECTORIES,--directories=DIRECTORIES
    Only directories matching that patterns will inspected, e.g. --directories=;-.git;-*tmp*;-*temp*
  -d MIN-DEPTH,--min-depth=MIN-DEPTH
    The minimum path depth (0 is the depth of the start directory), e.g. --min-depth=0 -d3
  -D MAX-DEPTH,--max-depth=MAX-DEPTH
    The maximum path depth (0 is the depth of the start directory), e.g. --max-depth=1 -D99
  -m MINUTES,--minutes=MINUTES
    The found files must be older (if < 0) or newer (if > 0) than that amount of minutes, e.g. --minutes=123 -m-1793
  -y DAYS,--days=DAYS
    The found files must be older (if < 0) or newer (if > 0) than that amount of days, e.g. --days=10 -y-60
  -s SIZE,--size=SIZE
    The found files must have a size lower  (if < 0) or larger (if > 0) than that size. Units: [kmgt], e.g. --size=1234 -s3k
  -t TYPE,--type=TYPE
    The file type: f(ile) d(irectory) l(ink) s(ocket) b(lock) p(ipe) c(har), e.g. --type=f,d,l -td
textknife strings [<options>]  SOURCE BASE
    Searches regular expressions.
  -?,--help
    Shows the usage information., e.g. --help -?
  -P PATTERN,--pattern=PATTERN
    The pattern describing the key., e.g. --pattern=/^max_memory\s*=
  -o,--only-matching
    Displayes only the matched string, e.g. --only-matching -o
  -l,--list
    Show the filename only, not the matching lines, e.g. --list -l
  -v,--invert-match
    Show the lines NOT matching the patterns, e.g. --invert-match -v
  -m MAX-COUNT,--max-count=MAX-COUNT
    Stops file processing after that count of matching lines, e.g. --max-count=123 -m0
  SOURCE
    A directory with or without a list of file patterns., e.g. *.cpp,*.hpp,src/test* src1,src2 /home/joe
  BASE
    The start directory or a list of file patterns delimited by ',', first with path. Preceding '-' defines a NOT pattern, e.g. . /home/jonny/*.c,*.h,-*tmp*
  -f FILES,--files=FILES
    Only files matching that patterns will be found, e.g. --files=;*.cpp;*.hpp;-test*
  -p DIRECTORIES,--directories=DIRECTORIES
    Only directories matching that patterns will inspected, e.g. --directories=;-.git;-*tmp*;-*temp*
  -d MIN-DEPTH,--min-depth=MIN-DEPTH
    The minimum path depth (0 is the depth of the start directory), e.g. --min-depth=0 -d3
  -D MAX-DEPTH,--max-depth=MAX-DEPTH
    The maximum path depth (0 is the depth of the start directory), e.g. --max-depth=1 -D99
  -m MINUTES,--minutes=MINUTES
    The found files must be older (if < 0) or newer (if > 0) than that amount of minutes, e.g. --minutes=123 -m-1793
  -y DAYS,--days=DAYS
    The found files must be older (if < 0) or newer (if > 0) than that amount of days, e.g. --days=10 -y-60
  -s SIZE,--size=SIZE
    The found files must have a size lower  (if < 0) or larger (if > 0) than that size. Units: [kmgt], e.g. --size=1234 -s3k
  -t TYPE,--type=TYPE
    The file type: f(ile) d(irectory) l(ink) s(ocket) b(lock) p(ipe) c(har), e.g. --type=f,d,l -td
textknife strings [<options>]  SOURCE BASE
    Fetches the strings delimited by ' or " from files.
  -?,--help
    Shows the usage information., e.g. --help -?
  SOURCE
    A directory with or without a list of file patterns., e.g. *.cpp,*.hpp,src/test* src1,src2 /home/joe
  -o OUTPUT,--output=OUTPUT
    The strings will be put there, one per line, sorted. If '-' stdout is used., e.g. --output=mydata
  BASE
    The start directory or a list of file patterns delimited by ',', first with path. Preceding '-' defines a NOT pattern, e.g. . /home/jonny/*.c,*.h,-*tmp*
  -f FILES,--files=FILES
    Only files matching that patterns will be found, e.g. --files=;*.cpp;*.hpp;-test*
  -p DIRECTORIES,--directories=DIRECTORIES
    Only directories matching that patterns will inspected, e.g. --directories=;-.git;-*tmp*;-*temp*
  -d MIN-DEPTH,--min-depth=MIN-DEPTH
    The minimum path depth (0 is the depth of the start directory), e.g. --min-depth=0 -d3
  -D MAX-DEPTH,--max-depth=MAX-DEPTH
    The maximum path depth (0 is the depth of the start directory), e.g. --max-depth=1 -D99
  -m MINUTES,--minutes=MINUTES
    The found files must be older (if < 0) or newer (if > 0) than that amount of minutes, e.g. --minutes=123 -m-1793
  -y DAYS,--days=DAYS
    The found files must be older (if < 0) or newer (if > 0) than that amount of days, e.g. --days=10 -y-60
  -s SIZE,--size=SIZE
    The found files must have a size lower  (if < 0) or larger (if > 0) than that size. Units: [kmgt], e.g. --size=1234 -s3k
  -t TYPE,--type=TYPE
    The file type: f(ile) d(irectory) l(ink) s(ocket) b(lock) p(ipe) c(har), e.g. --type=f,d,l -td
+++ help requested
```
