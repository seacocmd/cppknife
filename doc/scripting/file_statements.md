# Buffer and File Related Statements
The following statements operate on buffers or files. 

## Chapter Overview
- [Searchengine](searchengine.md)
- [Basic statements](basic_statements.md)
- [File and buffer related statements](file_statements.md)
- [Search and replace related statements](search_statements.md)
- [Functions](functions.md)

## Table of Content
- [copy](#copy)
- [delete](#delete)
- [insert](#insert)
- [load](#load)
- [log](#log)
- [select](#select)
- [store](#store)

## copy
Copies a text to a buffer. If the parameter append exists the text is appended at the end of the buffer.

There is the possability of a __here document__: the syntax is '<<' followed by the marker or the delimited marker.
All lines following the line of the copy statement are put into the buffer until the marker is found.
The delimited marker means: no interpolation is done.

If the marker is not delimited the text is interpolated: variable names will be replaced by the variable values.

### Syntax
    copy <text> <target> [append]

	copy from <buffer> [starting <start-position>] [including <end-position> | excluding <end-position>] [to <target>] [append]
    
    copy <<<marker> <buffer> [append]
    <line_1>
    ...
    <line_n>
    <marker>

    copy <<'<marker>' <buffer> [append]
    <line_1>
    ...
    <line_m>
    <marker>

### Parameters
- __text__: a string or a buffer expression defining the source of the copy operation
- __target__: the target buffer. Default: the current buffer.
- __buffer__: the source buffer.
- __start-position__: the position in the source buffer where the text to copy begins.
- __end-position__: the position in the source buffer where the text to copy ends.
- __including__: if that keyword is used the end position is part of the contents to copy.
- __excluding__: if that keyword is used the end position is not part of the contents to copy.
- __append__: if that keyword exists the content is appended to the target. Otherwise it replaces the target content.

### Examples
    copy "Hello" main
    copy ~csv:1:2 main append # copy the first 2 lines onto the main buffer
    copy! from ~_main starting $(start) excluding $(end) to ~csv
    copy <<EOS append
    line $(start)
    line $(end)
    EOS
    copy <<'EOS'
    line1 $(this_will_be_not_interpreted)
    line2
    EOS
    

## delete
Delete some lines from the current buffer. It can be controlled by:

- The start of the lines to delete
- The end of the lines to delete
- The key word exclude-start means: the line defined by start is not deleted
- The key word include-end means: the line defined by start will be deleted too
- The expression count=&lt;count> defines the number of lines to delete. Can be used instead of an end expression.
- If the end and the count is given, the minimum of both will be used.

### Syntax
    delete <start-position> <end-position [<buffer>] [excluding-start] [including-end] 

### Parameters
- __start-position__: the deletion starts at that position. Syntax of the position is __line:column__.
- __end-position__: the deletion ends at that position. Syntax of the position is __line:column__.
- __buffer__: a buffer name. Default is the current buffer.
- __excluding-start__: if that keywords are given the deletition starts one character behind the __start-position-__.
- __including-end__: if that keywords are given the deletition includes the character defined by __end-position__. Otherwise it ends above the __end-position__.

### Examples
   	if s/Bob/
   	  $(bob) = $(_start)
   	  if s/Eve/
   	    delete! $(bob) $(_start)
   	  endif
   	endif
   	delete 1:1 1:3 ~csv including-end

## insert
Puts a text at a given position.

### Syntax
    insert [<buffer>] <position> <text>

### Parameters
- __buffer__: the name of the buffer where the insertion is done.
- __position__: the insertion is done at that position. Syntax of the position is __line:column__.
- __text__: that text will be inserted. May be a string, a buffer or a buffer expression.

### Examples
    insert ~csv 1:1 "id;name"
    if r/Alice/
      insert! $(_start) "Bob and "
    endif
    move s/end of file/
    insert! $(_position) ~csv

## load
Loads a textfile into a buffer.

### Syntax
    load <buffer> <filename>

### Parameters
- __buffer__: the buffer name.
- __filename__: the buffer is filled by the contents of that file.

### Examples
    load ~_main unprocessed_data.txt

## log
Puts a text into the buffer named log and display it.

A text may be a string constant or a buffer content.

### Syntax
    log <text>

### Parameters
- __text__: the text to log. May be a string, a buffer or a buffer expression.

### Examples
    log "= start"
    log ~csv:1-3

## select
Selects the buffer where the script should be working ("current buffer").
That is uses in some command as the default buffer.

If the keyword __push__ exists the previous current buffer is stacked. It can be restored with __pop__.

### Syntax
    select [push] <buffer>
    select pop

### Parameters
- __buffer__: the buffer to select

### Examples
    select ~csv
    select push ~data
    select pop

## store
Stores a buffer into a textfile.

### Syntax
    store <buffer> <filename> [append]

### Parameters
- __buffer__: the name of the buffer to store.
- __filename__: a string with the file where the buffer is stored.
- __append__: if that keyword exists the buffer contents is appended to the file. Otherwise it replaces the file contents.

### Examples
    store ~main "processed_data.txt"

    store ~main "processed_data.txt" append
