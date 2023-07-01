# C-Standard-Header

## Objective

Creates or replaces a header comment in files with C or a similar programming language.

That language must know the multi line comment syntax <code>/* ... */</code>.

## Parameters

- **path**: The path of the source files (relative or absolute). Examples: 'source' '/home/ws/source' '.'
- **pattern**: A  regular expression describing the files to process. Example: '.*\.?pp'
- **author**: The author of the source. Example: jonny@example.com
- **license**: The name of the license. Example: CC0
- **year**: The creation year. Default: the current year. Example: 2022
- **month**: The creation year. Default: the current month. Example: 10
- **day**: The creation day. Default: the current day. If empty a random day is taken. Example: 23

## Example
```
sesknife -Dpath=src '-Dpattern=\.[ch]pp$' -Dauthor=jonny@example.com -Dlicense=CC0 c-standard-header.ses
```
