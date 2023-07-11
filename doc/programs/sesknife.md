# sesknife

## Objectives
That program is an interpreter of the __Search Enginge Script__ language.

## Links
- [Search Enginge Script](searchengine.md)

## Examples
```
# Executes a script "build_summary.ses" with 2 parameters: "source" and "target":
sesknife build_summary.ses -Dsource=data/year2023.csv -Dtarget=/tmp/result.doc
# Describe the usage:
sesknife --help
```

## Usage

```
sesknife [<options>]  SCRIPT INPUT
    A search engine script processor.
  -?,--help
    Shows the usage information., e.g. --help -?
  -l LOG-LEVEL,--log-level=LOG-LEVEL
    Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG, e.g. --log-level=123 -l0
  -v,--verbose
    Show more information, e.g. --verbose -v
  -t,--trace
    Log each statement of the script., e.g. --trace -t
  -D DEFINE,--define=DEFINE
    Defines a variable/parameter. Can be used multiple times, e.g. --define=path=/usr/local/bin -Dcount=10
  --examples
    Show usage examples, e.g. --examples --examples --examples --examples --examples
  SCRIPT
    The script to process., e.g. /data/myfile.txt yourfile.pdf
  INPUT
    The files to process. Use '' for no files.
+++ help requested
```
