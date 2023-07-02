# Search and Replace Statements
The following statements can be used to search or replaces some texts.

## Chapter Overview
- [Searchengine](searchengine.md)
- [Basic statements](basic_statements.md)
- [File and buffer related statements](file_statements.md)
- [Search and replace related statements](search_statements.md)
- [Functions](functions.md)

## Table of Content
- [Mark](#Mark)
- [Move](#Move)
- [Replace](#Replace)
## Move
Sets the current position in the addressed buffer. If no buffer is given the current buffer is used. 

### Syntax
    move <position> [buffer]

### Parameters
- **position**: the position to move. Synax is **line:column**. 
	May be an absolute position (**line** or **column** is a number without sign)
	or a relative position (**line** or **column** have a sign).
- **buffer**: the related buffer. Default is the current buffer.

### Examples
    # set the position to line 1 column 5
    move 1:5
    # set the position one line behind, don't change column
    move +1
    # sets the position 3 characters behind the current column, line is unchanged
    move 0:+3
    # set the position to the first line containing "<body>" in any case in buffer "~csv"
    move s/<body>/i ~csv
    # search backward to the first line containing "Miller" or "Muller"
    move r/M[iu]ller/B

## Mark
Sets or save the mark. The mark is initially set to the begin of the buffer.

__Note__: the mark is a special position different to the current position. 
It can be used in some commands to define a range.

- "mark set" copies the current position to the mark.
- "mark set search" copies the last search position to the mark. __Note__: the current position after a search is behind the hit.
- "mark exchange" exchanges the current position and the mark.
- "mark save" stores the mark in a given variable.
- "mark restore" fetches the mark from a variable.

### Syntax
    mark set [search]
    mark exchange
    mark { save | restore } <variable>

### Parameters
- **set**: if the keyword exists the mark is set to the current buffer condition.
- **search**: if the keyword exists the mark is set from the last hit.
- **exchange**: if the keyword exists than mark and current position will be exchanged.
- **save**: if the keyword exists than the mark is stored to the **variable**
- **restore**: if the keyword exists than the mark is set from the **variable**.
- **variable**: the name of the variable (syntax: $(id)) where the mark should be stored or from where the mark is restored.

### Examples
    move s/start/
    mark set search
    mark save $(start)
    move s/end/
    delete mark
    mark restore $(start)

## Replace
Replace a pattern with a given string, in the current line, a range or the whole buffer.

- If the search expression is a regular expression containing groups (delimited by '(' and ')'), 
the replacement string max contain references to that groups: $&lt;groupNo>.
That refererences will be replaced by the value of the found substring belonging to the group.
If the flags of the search expression contains "L" than the replacement is done only in the current line of the buffer
- If a buffer is given the replacement is done in that buffer. Otherwise the current buffer is used.
- If a range is given with start and end position the replacement is done in the range only.
- If a count is given by "count=&lt;count> than that number of replacements is done in each processed line.
- The possible if defines a search expression. The replacement is done only if the (whole) line matches that search expression.

### Syntax
    replace <search-expression> <replacement-string> [<buffer>] [<start-position> <end-position>] [count=<count>] [if <filter>]

### Parameters:
- **search-expression**: defines the pattern to search.
- **replacement**: the string defining the replacement. May be **references** the groups noted in the search expression with '(' and ')'.
- **start-position**: defines the start of the range where the replacement take place. Syntax: **line**:**column**.
- **end-position**: defines the end of the range where the replacement take place. Syntax: **line**:**column**.
- **count**: only that amount of replacements take place. Default: unlimited.
- **filter**: a search expression: Only lines containing that pattern will be processed.

### Examples
    # Replace only in the first 4 lines:    
    replace s/Admin/ "User" ~_main 1:1 5:0
    # Swaps the first two number columns only in the current line:
    replace r/(\d+),(\d+)/ "~2,~1" line count=1
    # Replaces "," only in the current line of the buffer ~_names:
    replace s","L ";" ~_names
    # Replace "Jonny" by "Jimmy" in the whole buffer
    replace s/Jonny/ "Jimmy" if /user:/



