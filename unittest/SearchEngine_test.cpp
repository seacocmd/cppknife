/*
 * SearchEngine_test.cpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(SearchEngineTest, basic) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(R"""(copy "Hello" !_main
store !_main "###" 
)""");
  replaceString(script, "###", fnTarget, 1);
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  auto contents = readAsString(fnTarget.c_str(), logger);
  ASSERT_STREQ(contents.c_str(), "Hello\n");
  delete logger;
}

TEST(SearchEngineTest, copyHereDocument) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(copy <<EOS !_main
line1
line2
EOS
store !_main "###" 
)""");
  replaceString(script, "###", fnTarget, 1);
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  auto contents = readAsString(fnTarget.c_str(), logger);
  ASSERT_STREQ(contents.c_str(), "line1\nline2\n");
  delete logger;
}
TEST(SearchEngineTest, log) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script("log /Hello world!/");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}
TEST(SearchEngineTest, ifStatement) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(copy <<EOS !_main
Hello world!
EOS
if s/Hello/
  log "OK"
else
  log "wrong"
endif
log "end"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}
TEST(SearchEngineTest, numericAssignment) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(!(no) := 3
!(no) := !(no) + 1 - 2
log! "No: !(no) expected: 2"
!(no) := !(no) * 6 / 3 % 3
log! "No: !(no) expected: 1"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}

TEST(SearchEngineTest, whileStatement) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  //auto fnTrace = temporaryFile("trace.log", nullptr, true);
  std::string script(
      R"""(copy <<EOS !_main
one man
two men
EOS
!(count) := 0
while r/m[ae]n/
  !(count) := !(count) + 1
  log! "loop !(count)"
endwhile
log! "count: !(count) expected: 2"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace(fnTrace.c_str());
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}
TEST(SearchEngineTest, predefinedVariables) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load !_main "###"
if r/m[ae]n/
  log! "file: !(_file) line: !(_line) lines: !(_lines)"
  log! "hit: !(_hit)"
  log! "date: !(_date) !(_time)"
else
  log "unexpected"
endif
)""");
  replaceString(script, "###", fnData, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(one man and one woman
Line2
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}

TEST(SearchEngineTest, move) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load !_main "###"
move s/e3/
log! "3 expected: !(_start) !(_hit) !(_position)"
move 1:3
log! "expected: 1:3 !(_position)"
move +1:-1
log! "expected: 2:2 !(_position)"
move +1
log! "expected: 3:2 !(_position)"
move 0:+99
log! "expected: 3:6 !(_position) 1:1 !(_mark)"
)""");
  replaceString(script, "###", fnData, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(Line1
Line2
Line3
Line4
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}
TEST(SearchEngineTest, mark) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load !_main "###"
mark save !(pos1)
move 2:3
log! "expected: 1:1 2:3 !(_mark) !(_position)"
mark set
log! "expected: 2:3 2:3 !(_mark) !(_position)"
move +1:-1
log! "expected: 3:2 !(_position)"
mark exchange
log! "expected: 3:2 2:3 !(_mark) !(_position)"
mark restore !(pos1)
log! "expected: 1:1 !(_mark)"
)""");
  replaceString(script, "###", fnData, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(Line1
Line2
Line3
Line4
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}

TEST(SearchEngineTest, insert) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load !_main "###"
insert 2:5 "XXX"
move 1:1
if s/LineXXX2/
  log "correct"
else
  log "wrong"
endif
log !_main
)""");
  replaceString(script, "###", fnData, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(Line1
Line2
Line3
Line4
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}

TEST(SearchEngineTest, deleteStatement) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnOut = replaceString(fnData, ".inp", ".out");
  std::string script(
      R"""(load !_main "#1"
move 2:2
mark set 
move s/abc/
delete! !(_mark) !(_start)
delete 1:0 2:0
store !_main "#2"
)""");
  replaceString(script, "#1", fnData, 1);
  replaceString(script, "#2", fnOut, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(Line1
Line2
Line3 abc
Line4
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  auto output = readAsString(fnOut.c_str(), logger);
  ASSERT_STREQ(output.c_str(), R"""(Labc
Line4
)""");
  delete logger;
}

TEST(SearchEngineTest, replace) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnOut = replaceString(fnData, ".inp", ".out");
  std::string script(
      R"""(load !_main "#1"
replace r/line(.)/i "Y!1" if s/4/
replace r/line(.)/i "X!1" 2:1 3:1
replace r/line(.)/iL "l!1"  
store !_main "#2"
)""");
  replaceString(script, "#1", fnData, 1);
  replaceString(script, "#2", fnOut, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(Line1 wow
Line2 ABC
Line3 abc
Line4
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  auto output = readAsString(fnOut.c_str(), logger);
  ASSERT_STREQ(output.c_str(), R"""(l1 wow
X2 ABC
Line3 abc
Y4
)""");
  delete logger;
}
TEST(SearchEngineTest, conditionNumeric) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(!(int) := 5
if !(int) > 5
  stop ">"
else
  log "> ok"
endif
if 5 >= 6
  stop ">="
endif
if 5 < 5
  stop "<"
endif
if 5 <= 4
  stop "<="
endif
if 5 <= 4
  stop "<="
endif
if 5 != 5
  stop "!="
endif
if 5 == 6
  stop "=="
endif
if 0
  stop "0"
endif
if -1
  log "-1 is true"
else
  stop "-1"
endif
)""");
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  writeText(fnScript.c_str(), script.c_str());
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_TRUE(engine.testAndRun());
  delete logger;
}
TEST(SearchEngineTest, conditionStringCondition) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(!(string) = "B"
if "!(string)" -gt "B"
  stop "-gt"
else
  log "-gt ok"
endif
if "B" -ge "C"
  stop "-ge"
endif
if "B" -lt "B"
  stop "-lt"
endif
if "B" -lt "A"
  stop "-lt"
endif
if "B" -le "A"
  stop "-le"
endif
if "B" -ne "B"
  stop "-ne"
endif
if "B" -eq "C"
  stop "-eq"
endif
if ""
  stop "<empty>"
endif
if "abc"
  log "'abc' is true"
else
  stop "'abc'"
endif
)""");
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  writeText(fnScript.c_str(), script.c_str());
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_TRUE(engine.testAndRun());
  delete logger;
}

TEST(SearchEngineTest, leave) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(!(count) := 0
while 1
  !(count) := !(count) + 1
  if !(count) > 5
    leave 2
    stop "leave"
  endif
endwhile
)""");
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  writeText(fnScript.c_str(), script.c_str());
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_TRUE(engine.testAndRun());
  delete logger;
}
