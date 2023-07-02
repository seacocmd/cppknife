# fileknife

## Objectives
That program offers services of files and directories.

## Examples
```
# Show a statistic about the filetree in path /home ignoring the .git subdirectories:
fileknife du /home --directories=-.git
# Ignore all .git directories, show only *.cpp and *.hpp files:
fileknife du --files=*.cpp,*.hpp /home

# Show the 20 youngest, newest, largest files from /etc.
fileknife extrema /etc
# Show the 5 youngest, newest directories and symbolic links from /var/spool.
fileknife extrema --count=5 --types=dl /var/spool

# Show all logfiles from /var/log that ar younger than 3 days and a size larger than 1 kByte:
fileknife list --files=*.log /var/log --days=-3 --size=+1k
# Show all HTML files unless index.html and modified in more than 5 minutes and limit the path depth to 3:
fileknife list --files=*.html,-index.html --max-depth=3 --minutes=+5 /srv/www

# Count the lines, words and characters of all source files:
fileknife wc --files=*.cpp /home/ws/cpp
```

## Usage

```
fileknife --help
fileknife [<options>]  MODE
    Version Jun 20 2022
    A universal tool working on files and directories
  -?,--help
    Shows the usage information., e.g. --help -?
  -l LOG-LEVEL,--log-level=LOG-LEVEL
    Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG, e.g. --log-level=123 -l0
  -v,--verbose
    Show more information, e.g. --verbose -v
  MODE
    What should be done:
    du
      Counts files, directories, bytes
    extrema
      Creates a statistic about the youngest/oldest/largest files
    list
      Lists some files/directories
    wc
      Counts words, lines and characters
fileknife du [<options>]  BASE
    Counts files, directories, bytes
  -?,--help
    Shows the usage information., e.g. --help -?
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
fileknife extrema [<options>]  BASE
    Creates a statistic about the youngest/oldest/largest files
  -?,--help
    Shows the usage information., e.g. --help -?
  -c COUNT,--count=COUNT
    Maximal count of entries per extremum will be collected, e.g. --count=5 -c100
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
fileknife list [<options>]  BASE
    Lists some files/directories
  -?,--help
    Shows the usage information., e.g. --help -?
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
fileknife wc [<options>]  BASE
    Counts words, lines and characters
  -?,--help
    Shows the usage information., e.g. --help -?
  -b,--bytes
    Only count bytes, e.g. --bytes -b
  -l,--lines
    Only count lines, e.g. --lines -l
  -w,--words
    Only count words, e.g. --words -w
  -L,--max-line-length
    Only return maximal line length, e.g. --max-line-length -L
  -f FORMAT,--format=FORMAT
    A template for the output format: placeholders: %% %w(ords) %l(ines) %c(chars) %M(axlength) %f(ullname) %n(ode) %p(ath), e.g. --format='lines: %l max-line: %L percent: %%
  -y SUMMARY,--summary=SUMMARY
    The replacement of the filename in the summary line<summary>, e.g. --summary=mydata
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
```
