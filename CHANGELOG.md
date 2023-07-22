# Changelog

## [0.5.12] - 2023-07-22 Installation/Update

## Fixed
- InstallOrUpdate.sh: links to binaries and library

## [0.5.11] - 2023-07-22 Installation/Update

## Fixed
- readme.txt: wrong script name

## [0.5.10] - 2023-07-22 Installation/Update

## Fixed
- InstallOrUpdate.sh: Correct download link, copying of the script

## [0.5.9] - 2023-07-22 Installation/Update

## Fixed
- InstallOrUpdate.sh: Correct download link

## [0.5.8] - 2023-07-22 Installation/Update

## Changed
- tar archive instead of single binaries
- CreateRelease: build of the tar archive

## Fixed
- InstallOrUpdate.sh: Correct download links

## [0.5.7] - 2023-07-22 Installation/Update

## Added
- script releases/amd64/InstallOrUpdate.sh
- releases/amd64/versions.txt

## [0.5.6] - 2023-07-22

## Added
- script CreateRelease
- not ready: SocketServer.cpp SocketClient.cpp Agents.cpp netknife.cpp netknife_main.cpp
- geoknife dbknife netknife

## Changed
- README.md: Describing the build process.

## Fixed
- CMakeList.txt: building with unittests

## [0.5.5] - 2023-07-20

## Added
- cppknife_control.hpp: Controls the usage of unittests
- unready: net/SocketServer.cpp and net/SocketClient.cpp

## Changed
- CMakeList.txt: Possibility to exclude the unittests
- CMakeList.txt: only two libraries libcppknife and libcppknifeunittest

## [0.5.4] - 2023-07-16

## Added
- LineAgent::openFile(): new parameters checkBinary and ignoreError
- textknife: subcommand search: new option --string
- textknife: subcommand checksum
- textknife: logging of processed files
- StringTool::crc32() and StringTool::crc32Update()
- Examples for "textknife search"
- CommandHandler::isValid()
- FsEntryLinux::_parent
- CommandHandler::lastInstance()

## Changed

- Base64: parameters changed: encodeBase64() and decodeBase64()
- LineAgent::setBuffer(): handles the state "already read data".
- LineAgent::fillBuffer(): endless loop when requested bytes == 0

## Fixed

- FunctionEngine::osCopy(): wrong parsing of "unique"
- textknife: filtering out directories from processing
- textknife search: handling of '--list' common with '--invert-match'

## [0.5.3] - 2023-07-15

## Added:
- SearchEngine: mark set <absolute-position>
- SearchEngine::variableAsString(): works now with $(<id>) pattern
- extended mode in dbknife
- LineAgent:
	- Automatic buffer resizing for unlimited line length.
	- File position of the current line
	- Line number of the current line
	- Access to predesessors of the current line.
- ArgumentParser: regular expressions know the flag 'w' (word) now
- ArgumentParser: regular expressions always uses the mode ECMAScript now
- Base64.cpp
- textknife: subcommand "search"

## Fixed:
- Improvements in the whole documentation: programs and SES language.
- Corrections in the "mark" statement: <id> as variable not as $(<id>)
- 

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

