# The Search Engine

## Table of Content
## Content
* [The Purpose](searchengine.md#the-purpose)
* [Script Engine Process](searchengine.md#script-Engine-process)
* [Script File Structure](searchengine.md#file-Structure)
* [Buffers](searchengine.md#Buffers)
* [Variables](searchengine.md#Variables)
* [String Constants](searchengine.md#string-constants)
* [Positions](searchengine.md#Positions)
* [Range](searchengine.md#Range)
* [Conditions](searchengine.md#Conditions)
* [Statements](searchengine.md#Statements)

## Content of External Documents

- [Basic statements](basic_statements.md)
	- assert
	- Assignment
	- leave
	- if else endif
	- script endscript
	- stop
	- while endwhile
- [File and buffer related statements](file_statements.md)
	- copy
	- delete
	- load
	- log
	- select
	- store
- [Search and replace related statements](search_statements.md)
    - mark
	- move
	- replace
- [Functions](functions.md)
	- Scope "buffer"
	- Scope "math"
	- Scope "os"
	- Scope "string"


## The Purpose
The script engine implements an interpreter of a text search and manipulation language.
The language knows variables, sub scripts, and control statements if and while.

## Script Engine Process
The engine reads a script file and interprets that line by line.

It is modular: An script can call other scripts. You can define also scripts inside a script.

## Script File Structure
The file is a sequence of statements, empty lines and comments.

- An __empty line__ is a line containing whitespaces only.
- A __comment__ is a line starting with '#'.
- A __statement__ owns always a startline and possibly more following lines.
  - A startline starts with a key word (name of the statement) followed by parameters divided by blanks.
  - The key word can be preceeded by whitespaces. 
  - A following line starts with the '+'. Note: the '+' is not part of the content.

## Buffers
- A buffer is a amount of lines.
- A buffer has a name (alphanumeric) and a content (a list of lines)
- Each script can have any number of local buffers.
- There are any number of global buffers. The name of the global buffers starts with '_'.
- The content of a buffer can be addressed in the script by ~&lt;name&gt;
- Additionally a part of a buffer can be addressed: 
  - a single line: ~&lt;name&gt;:&lt;line_number>
  - line numbers starts with 1
  - a range of lines: ~&lt;name&gt;:&lt;start>-&lt;end> The end line is included. if &lt;end> &lt; 0: it is relative to the end. -1: last line
  - a range of lines: ~&lt;name&gt;:&lt;start>:&lt;count>
  - &lt;start> is a start position
  - &lt;end> is an end position (exclusive)
  - &lt;count> is the number of lines
  - It is no error to specify more lines than available.
- The context defines whether the buffer name is meaned or the buffer contents.

There are builtin buffers:
- ~_main: the main text buffer of the search engine

### Examples
    load ~csv "data.csv"
    log ~csv:1-3
    log ~csv:$(start):$(count)

## Variables
Variables are "named values". There are local variables, only known and accessible in the script,
and global variables, accessable in all scripts. Global variables starts with "$_".
- A variable has a name (alphanumeric).
- A variable stores a simple value, e.g a line number.
- The value of the variable is expressed by $(&lt;name>).
- If the statement name ends with "!" the variable can be in any position of a statement. It will be replaced by its value before the statement is interpreted.
- A variable will be defined by the assignment statement: __&lt;name> = &lt;text>__ or __&lt;name> := &lt;numeric-expression>__

### Predefined Variables:
**Note**: the predefined global variables starts with two underlines.

- $(__line): the current line (1-based): 1..N
- $(__line0): the current line (0-based): 0..N-1 
- $(__column): the current column (1-based: 1..M
- $(__column0): the current column (0-based): 0..M-1
- $(__lines): the count of lines of the current buffer
- $(__position): the current position, e.g. "3:29"
- $(__mark): the current mark, e.g. "24:3"
- $(__start): the start position of the last search. Undefined if no hit.
- $(__end): the position behind the last search. Undefined if no hit.
- $(__hit): the result of the last search statement: empty string if not found, the found string otherwise
- $(__length): the length of **$(__hit)**. 0 if there was no hit.
- $(__buffer): the name of the current buffer
- $(__file): the filename of the current buffer
- $(__date): the current date, e.g. 2023.01.25
- $(__time): the current time, e.g. 22:44

### Interpolated script lines
The lines in the script may contain variable names at any position. These will be replaced by the value of the variable before the 
"normal" processing of the line starts. This is called __interpolation of the line__.

Examples:

    log "the buffer has $(_lines) lines"
    counter := $(counter) + $(_lines) + 2
    if $(counter) > 0
      move +$(counter)
    endif

## String Constants
* A string constant is a starting delimiter, the content and the trailing delimiter.
* Starting and trailing delimiters must be equal.
* The content must not contain the delimiter.

### Examples
    "Hello world"
    /a second string with " and ' inside/

## Search String
There are different possibilities to define a search string:

- s&lt;delimiter>fix_string<delimiter>&lt;flags>: There are no meta characters.
- r&lt;delimiter>fix_string<delimiter>&lt;flags>: a regular expression
- m&lt;delimiter>fix_string<delimiter>&lt;flags>: a expression used in shells: '*' means any string '?' a single char.


Flags:

- __i__: ignore case
- __L__: searches only in the current line
- __B__: searches backwards
- __T__: do not change the current position ("temporary")
- __^__: starts the search at the begin of the current line
- __$__: starts the search at the end of the current line
- __<__: starts the search at the begin of the buffer
- __>__: starts the search at the end of the buffer

__Search and current position:__

If the search starts at the current position (unless one of the flags ^, < > are used).
If the search is successful the current position is set behind the found hit (unless the flag __T__ (temporary) is used).
If the search is not successful the position remains unchanged.

### Examples
    s"Title"iT
    r/the\s*title!?/i^
    m!The*itle!B

### Backward Searches
When the flag __B__ is used the search is done on the lines below the current position.
If the string is not found, the current position is not changed.

### Inline Searches
When the flag __L__ is used the search is done only in the current line.

## Position and Mark

A position is a location in a buffer, specified by:

- an absolute line number, starting with 1, e.g. 255
- a relative line number: a sign and a linenumber, e.g. -2, +7
- a search string defined in a condition ("if" or "while") or the search command

The position can be stored in a variable: Than the format is &lt;line>:&lt;column>, for example "344:12".

There is a special positon named __mark__. It is uses to have a second position to define a range:
- Search the start of the range
- Set the mark
- Search the end of the range
- Copy or delete from **$(__mark)** to **$(__position)**

## Text
A text is:
- a buffer content: see above, e.g. ~&lt;buffername>:1-3
- a string constant: see above, e.g. "Hello world"

## Conditions
The __if__ and the __while__ statement uses a condition. That may be a

- A search expression: The value is __true__ if the search condition has been found: Starting search at the current position.
  If the search is successful the current position is behind the found hit. If the search is not successful the position remains unchanged.
- A comparison: Two operands and a operator between that. Example $(value) < 3
    - numeric operators: == != < <= > >= 
    - string operators: -eq -ne -lt -le -gt -ge: equal, "not equal" "lower than" "lower equal" "greater than" "greater equal"
- A single value:
    - A number: 0 means false, otherwise means __true__.
    - A string: "" (empty string) means false, otherwise means __true__.
