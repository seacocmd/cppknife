# Changelog

## [Unreleased]

## [0.5.2] - 2023-07-10

### Added
- New script help-to-md.ses with documentation help-to-md.md
- New parameter in SearchParser::parseBuffer(): returnNullIfMissing
- New parameter in LineList::insert(): addNewline
- New global parameter "--example" in dbknife, geoknife, sesknife and textknife 

### Changed
- SearchEngine: A new syntax of the delete statement: delete from START excluding END in BUFFER
- SearchEngine: all lines will be interpolated without exceptions: interpolation operator "!" removed
- SearchEngine: Load-statement: Error message if the source file does not exist.
- SearchEngine: assert statement contains variable ids instead of "$(&lt;id>)".
- SearchEngine: syntax of "string.substring" changed: "=" removed
- SearchEngine: syntax of "replace" changed: "from"/"behind" and "excluding"/"including" and "count" without "=".
- CMakeList.txt: upgrade to CMake V3.20 (Debian Bookworm)
- Documentation in doc/scripting/*.md improved
- Logger: error adds the prefix "+++"

### Fixed:
- If an error in a script occurs the program is stopped now.
- It is not an error to have no buffer in the call statement.
- example-to-md.ses works now.
- SearchEngine: Variables will be saved and restored while testing syntax only.
- SearchEngine: Buffer must be cleared (and position must be reset) on "load".
- sesknife: Parameter --trace has now a default value of false. 


## [0.5.1] - 2023-07-04

### Added
- fileknife.cpp: 
    - new global argument: --example
    - new argument for "list": --short-name
- textknife.cpp:
	- new argument for "adapt": --replacement
- LineAgent::estimateLineCount()
- LineReader::reset()
- StringLinesStream::reset()

### Changes
- LineAgent::readFromFile(): reads using FileAgent(). Stops on binary data.

### Fixed
- FileAgent: openFile() resets now _hasBinaryData
- fileknife.cpp: improvements in usage message
- ToolsCommons.cpp: wrong attribute name "types" in  populateFilter(): -> "type"
- Traverser.cpp: FileAgentLinux(): removed: FTS_COMFOLLOW and FTS_DONTCHDIR

