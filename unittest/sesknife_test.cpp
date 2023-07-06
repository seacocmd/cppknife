/*
 * Script_test.cpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../tools/sesknife.hpp"
using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(SesKnifeTest, log) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("example.data", "unittest", true);
  std::string script(
      R"""(
log "Hi $(name)"
log "File: $(_file)"
log ~_main
)""");
  writeText(fnScript.c_str(), script.c_str(), script.size(), logger);
  std::string data(R"""(
Hi World!
)""");
  writeText(fnData.c_str(), data.c_str(), data.size(), logger);
  const char *argv[] = { "-Dname=Jonny", "-D_file=/tmp/data.txt",
      fnScript.c_str(), fnData.c_str() };
  size_t argc = sizeof argv / sizeof argv[0];
  searchEngineScriptKnife(argc, const_cast<char**>(argv), logger);
  delete logger;
}

TEST(SesKnifeTest, helpToMd) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = "resources/ses/help-to-md.ses";
  auto fnSource = temporaryFile("example.cpp", "unittest", true);
  auto fnTarget = temporaryFile("example.md", "unittest", true);
  auto fnHelp = temporaryFile("help.txt", "unittest", true);
  std::string sourceText(
      R"""(
void doNothing(){
}
void examples() {
  printf(
      R"""(# Any text
new line1
new line2
)!!!!!!);
void wait(){
}
)""");
  writeText(fnSource.c_str(), replaceString(sourceText, "!!", "\"").c_str());
  std::string helpText(
      R"""(Usage: example <options>
--help: show this info
)""");
  writeText(fnHelp.c_str(), helpText.c_str());
  std::string markdown(
      R"""(
# The file
## Examples
```
The old text
```
The rest should be unchanged.

## Usage
```
The old help text
```
Any other text.
)""");
  writeText(fnTarget.c_str(), markdown.c_str(), markdown.size(), logger);
  auto argSource = formatCString("-Dsource=%s", fnSource.c_str());
  auto argTarget = formatCString("-Dtarget=%s", fnTarget.c_str());
  auto argHelp = formatCString("-DhelpOutput=%s", fnHelp.c_str());
  const char *argv[] = { argSource.c_str(), argTarget.c_str(), argHelp.c_str(),
      fnScript };
  size_t argc = sizeof argv / sizeof argv[0];
  ASSERT_EQ(0, searchEngineScriptKnife(argc, const_cast<char**>(argv), logger));
  auto contents = readAsString(fnTarget.c_str(), logger);
  ASSERT_STREQ(contents.c_str(), R"""(
# The file
## Examples
```
# Any text
new line1
new line2
```
The rest should be unchanged.

## Usage
```
Usage: example <options>
--help: show this info
```
Any other text.
)""");
  delete logger;
}

