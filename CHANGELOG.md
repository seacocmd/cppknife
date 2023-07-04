# Changelog

## [Unreleased]

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

