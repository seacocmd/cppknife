# Basic Statements

## Chapter Overview
- [Searchengine](searchengine.md)
- [Basic statements](basic_statements.md)
- [File and buffer related statements](file_statements.md)
- [Search and replace related statements](search_statements.md)
- [Functions](functions.md)

## Content
- [assert](#assert)
- [Assignment](#Assignment)
- [call](#call)
- [exit](#exit)
- [if else endif](#if)
- [leave](#leave)
- [script](#script)
- [stop](#stop)
- [while endwhile](#while)

## assert
Tests the existence of variables or buffers. If one does not exist than an error occurs.

__Note__: the variables must be noted by their identifiers, not as '$(&lt;id>).

### Syntax
    assert <variable-or-buffer1> [<variable-or-buffer2>...]]

### Parameters
- __variable-or-bufferN__: the name of a variable or the name of a buffer

### Examples
    assert ~csv name age _verbose

## Assignment
An assignment defines a variable. There are three alternatives:

- &lt;name> = &lt;text>
    - This assigns the text to the variable
    - Text may be a sequence of strings that will be concatenated
    - If the keyword __append__ is found the text is appended to the variable (if that exists).
- &lt;name> ?= &lt;text>
    - Conditional assignment: the assignment is only done if the variable does not exist.
    - An existing variable is not changed. 
- $(&lt;name>) := &lt;numeric_expression>
	- This assignes the numeric expressions to the variable
	- A numeric expression is a sequence of numeric operands and operators
	- In one statement only the operators of one precedence class are allowed
	- Precedence classes:
		- Plus and minus: + -
		- Times and divisions: * / (integer division) : (float division) % (integer modulus)
	- Allowed: $(end) - $(start) + 1
	- Not allowed (mixed precedence): 3 * 4 + 2
- Additionally a function may be called: A function has a name &lt;scope>.&lt;function>. Examples: buffer.pop math.random
  - Scopes: buffer math os


### Syntax
    <name> = <text>
    <name> := <numeric_expression>
    <name> = <function_name> [<parameters>]
    <name := function <function_name> [<parameters>]
    <name> ?= <text>

### Parameters
- __name__: the name of the variable with the syntax __$(id)__
- __text__: a string or a here document or a buffer.
- __numeric_expression__: a number or a sum of numbers or a product of numbers.
- __function_name__: a predefined function like __string.length__
- __parameters__: a parameter of the function, specific for each predefined function.

### Examples
    start = "3" 
    end := $(start) + 3
    file := function os.basename "$(fullname)"
    data ?= "buffer: " ~data:1-3

## call
Calls a script by its name (internal scripts) or by its filename (external scripts).

Parameters can be written to a buffer. Each line of that buffer must contain an assignment.
The variable names is a simple identifier, not $(name).
The right site of the assignment is a string without delimiters, not an expression.
Do not use additional blanks around the "=".

The other way to define parameters is a sequence of strings with the parameter definitions.

### Syntax
    call <scriptname> [<parameter-buffer>] [<parameter-definition-1 [<parameter-definition-2>]...]

### Parameters
- __scriptname__: an identifier (internal script) or a string: the name of the external script file.
- __parameter-buffer__: a buffer name: That buffer contains the parameter definitions: each line contains an assignment for one parameter.
- __parameter-definition-N__: a string with one parameter definition, e.g. "a=3"

### Examples
    copy <<EOS ~parameters
    name=$(lastName)
	age=3
    output=~result
    EOS
    call "../basic/login.ses" ~parameters
    rc = buffer.pop ~result
    script maximum
      assert a b
      if $(b) > $(a)
        _rc := $(b)
      else
        _rc := $(a)
      endif
    endscript
    call maximum "a=7" "b=-3"
    log "max(7, 3): $(_rc)

## exit
Finishes the script or all scripts.

If a __returncode__ (a integer between 0 and 255) is given that value is the program exit code.

If the keyword __global__ is given than all running scripts (stacked by the __call__ statement)
will be stopped, otherwise only the current script.

### Syntax
    exit [<returncode>] [global]

### Parameters
- __returncode__: the program exit code as an integer: 0..255
- __global__: if that keyword is given than all nested scripts will be finished. Otherwise only the current script is stopped.

### Examples
    if "$(data)" == ''
      log "no more data"
      exit global
    endif

## if
The __if__ statement consist on three or five parts:

- the if line with the condition
- a list of statements
- an optional __else__ statement
- a list of statements
- a closing __endif__ statement

The condition is processes. 

If the result is __true__, the following statements are processed
until an __else__ or an *endif* is found.

If the result is __false__, the lines between the __else__ and the __endif__ statements
are processed. If no __else__ statement exists nothing is done.

A condition is:

- a string: if empty the condition is false otherwise true
- a comparison: &lt;left_operand> &lt;operator> &lt;right_operand>
	- numeric operators: &lt; &lt;= &gt; &gt;= = !=
	- string operators: -lt (lower than) -le (lower equal) -gt (greater than) -ge (greater equal) -eq (equal) -ne (not equal) 
	- Operands: &lt;text>

### Syntax
    if <condition>
      <true-statement-block>
    endif

    if <condition>
      <true-statement-block>
    else
      <false-statement-block>
    endif

### Parameters
- __condition__: the condition to inspect: a comparism, a number (0 means __false__) or a string (empty string means __false__).
- __true-statement-block__: an amount of lines that will be executed if the condition is __true__.
- __false-statement-block__: an amount of lines that will be executed if the condition is __false__.

### Examples
    if $(_line) > 10
      log "too big"
    endif
    if $(_search) -ne "person:"
      log "too big"
    else
      log "OK"
    endif

## leave
That statements stops a block (inside __if__ or __while__ statements).

It is possible to leave multiple nested blocks if a __count__ is given.

### Syntax
    leave [count] [if &lt;condition</>]

### Parameters
- __count__: the amount of blocks to leave. Default is 1.
- __condition__: Only if that condition is __true__ than the leave statement takes place.

### Examples
    while s/Person:/
      if s/Adam/
         leave 2 if s/Smith/
         log "Adam but not Smith"
      endif
    endwhile

## script
Defines a (sub-)script. That script can be called with __call__ like an external script file.

Parameters can be take from a buffer (each line contains an assignment &lt;name>=&lt;value>) 
or from strings given in the call statement.

Example of a parameter buffer:
    lineNo=3
    name=Joe

Example of parameters by strings:
	call myScript "line=123" "greeting=Hello"

### Syntax
    script <name>
    <body>
    endscript
    
### Parameters
- __name__: the name of the script.
- __parameters__: a string as a blank separated list of variable names needed in the script.
- __body__: an amount of lines belonging to the script.

### Examples
    script sum
      _rc := $(a) + $(b)
    endscript
    copy <<EOS ~param
    a=3
    b=7
    EOS
    call sum ~param
    echo "sum of 3+7: $(_rc)
    call sum "a=99" "b=-3"
    echo "sum of 99 and -3: $(_rc)

## stop
That command stops the script.

### Syntax
    stop [<text>]

### Parameters
- __text__: a string with a message. That is printed to the screen.

### Examples
    stop "+++ missing pattern"
    stop

## while
A while statement consists of three parts:
- a starting __while__ statement with condition
- a block of statements
- a trailing __endwhile__ statement

The block is executed as long as the condition is __true__.

See [if](#if) for the description of a condition.

### Syntax
    while <condition>
      <statement-block>
    endwhile

### Parameters
- __condition__: the condition to inspect: a comparism, a number (0 means __false__) or a string (empty string means __false__).
- __statement-block__: an amount of lines that will be executed while the condition is __true__.

### Examples
    count := 0
    while s/person:/
      count := $(count) + 1
      move +1
    endwhile
    log "$(count) persons found"

