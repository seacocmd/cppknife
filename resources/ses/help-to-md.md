# Help to MD Document Builder

## Objectives
Put the usage help info and the examples from a source file and replace it in the description file as markdown document.

## Usage
```
sesknife help-to-md.ses -Dsource=SOURCE -Dtarget=TARGET -DhelpOutput=HELP_OUTPUT
```

Example:

```
fileknife --help >/tmp/help.txt
sesknife example-to-md.ses -Dsource=tools/fileknife.cpp \
  -Dtarget=doc/programs/fileknife.md -DhelpOutput=/tmp/help.txt
```

### Parameter

- SOURCE: the source file (*.cpp)
- TARGET: the md file
- HELP_OUTPUT: redirection of the "&lt;program> --help" call in a file

### Preconditions
#### SOURCE
A C++ file with:

```
void examples() {
  printf(
      R"""(Any text describing the examples
)""");
```

### TARGET
	## Examples
	```
	Any text describing the example
	```

	## Usage
	```
	Any text describing the usage
	```
	