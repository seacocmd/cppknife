/*
 * Script_test.cpp
 *
 *  Created on: 11.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
using namespace cppknife;

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(ScriptTest, copyHereDocumentInterpolated) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(
no := 1
copy <<EOS ~_main
line$(no)
line2
EOS
store ~_main "###" 
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
TEST(ScriptTest, copyHereDocumentNotInterpolated) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(
no := 1
copy <<'EOS' ~_main
line$(no)
line2
EOS
store ~_main "$(file)" 
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.defineVariable("file", fnTarget.c_str());
  ASSERT_EQ(0, engine.testAndRun());
  auto contents = readAsString(fnTarget.c_str(), logger);
  ASSERT_STREQ(contents.c_str(), "line$(no)\nline2\n");
  delete logger;
}
TEST(ScriptTest, copyFrom) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(copy <<'EOS' ~_main
Line1
Line2
EOS
copy from ~_main starting 1:3 including 2:3 to ~data
str = buffer.join ~data "X"
if "$(str)" -ne "ne1XLin"
  stop "wrong range"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  //engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}
TEST(ScriptTest, log) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script("log /Hello world!/");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}
TEST(ScriptTest, ifStatement) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(copy <<EOS ~_main
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
TEST(ScriptTest, numericAssignment) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(no := 3
no := $(no) + 1 - 2
log "No: $(no) expected: 2"
no := $(no) * 6 / 3 % 3
log "No: $(no) expected: 1"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}

TEST(ScriptTest, whileStatement) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  //auto fnTrace = temporaryFile("trace.log", nullptr, true);
  std::string script(
      R"""(copy <<EOS ~_main
one man
two men
EOS
count := 0
while r/m[ae]n/
  move $(__end)
  count := $(count) + 1
  log "loop $(count)"
  if $(count) > 3
    stop "loop overflow"
  endif
endwhile
log "count: $(count) expected: 2"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  delete logger;
}
TEST(ScriptTest, predefinedVariables) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load ~_main "###"
if r/m[ae]n/
  log "file: $(__file) line: $(__line) lines: $(__lines)"
  log "hit: $(__hit)"
  log "date: $(__date) $(__time)"
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

TEST(ScriptTest, move) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load ~_main "###"
move s/e3/
log "4 expected: start: $(__start) end: $(__end) hit: $(__hit) current: $(__position)"
move 1:3
log "expected: 1:3 $(__position)"
move +1:-1
log "expected: 2:2 $(__position)"
move +1
log "expected: 3:2 $(__position)"
move 0:+99
log "expected: 3:6 $(__position) 1:1 $(__mark)"
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
TEST(ScriptTest, mark) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load ~_main "###"
mark save $(pos1)
move 2:3
log "expected: 1:1 2:3 $(__mark) $(__position)"
mark set
log "expected: 2:3 2:3 $(__mark) $(__position)"
move +1:-1
log "expected: 3:2 $(__position)"
mark exchange
log "expected: 3:2 2:3 $(__mark) $(__position)"
mark restore $(pos1)
log "expected: 1:1 $(__mark)"
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

TEST(ScriptTest, insert) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  std::string script(
      R"""(load ~_main "###"
insert 2:5 "XXX"
move 1:1
if s/LineXXX2/
  log "correct"
else
  log "wrong"
endif
log ~_main
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

TEST(ScriptTest, deleteStatementInline) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnOut = replaceString(fnData, ".inp", ".out");
  std::string script(
      R"""(load ~_main "#1"
delete from 2:2 including 2:4 in ~_main
delete behind 3:2 excluding 3:5 in ~_main
delete from 4:2 count 3 in ~_main
store ~_main "#2"
)""");
  replaceString(script, "#1", fnData, 1);
  replaceString(script, "#2", fnOut, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(abcd
a23456
b23456
c23456
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.setTrace("-");
  engine.testAndRun();
  auto output = readAsString(fnOut.c_str(), logger);
  ASSERT_STREQ(output.c_str(), R"""(abcd
a3456
b256
c56
)""");
  delete logger;
}

TEST(ScriptTest, deleteStatementMultiline) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnOut = replaceString(fnData, ".inp", ".out");
  std::string script(
      R"""(load ~_main "#1"
move 2:2
mark set 
move s/c2/
delete from $(__mark) excluding $(__start) in ~_main
store ~_main "#2"
)""");
  replaceString(script, "#1", fnData, 1);
  replaceString(script, "#2", fnOut, 1);
  writeText(fnScript.c_str(), script.c_str());
  std::string data(R"""(Line1
a23456
b23456
c23456
)""");
  writeText(fnData.c_str(), data.c_str());
  SearchEngine engine(*logger);
  //engine.setTrace("-");
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  engine.testAndRun();
  auto output = readAsString(fnOut.c_str(), logger);
  ASSERT_STREQ(output.c_str(), R"""(Line1
ac23456
)""");
  delete logger;
}

TEST(ScriptTest, replace) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnOut = replaceString(fnData, ".inp", ".out");
  std::string script(
      R"""(load ~_main "#1"
replace r/line(.)/i "Y$1" if s/4/
replace r/line(.)/i "X$1" from 2:1 excluding 3:1
replace r/line(.)/iL "l$1"  
store ~_main "#2"
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
TEST(ScriptTest, conditionNumeric) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(int := 5
if $(int) > 5
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
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}
TEST(ScriptTest, conditionStringCondition) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(string = "B"
if "$(string)" -gt "B"
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
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(ScriptTest, leave) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnScript = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(count := 0
while 1
  count := $(count) + 1
  if $(count) > 5
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
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(ScriptTest, call) {
  FEW_TESTS();
	auto logger = buildMemoryLogger(100, LV_DEBUG);
	auto fnSource = temporaryFile("example.ses", "unittest", true);
	auto fnSource2 = temporaryFile("sum.ses", "unittest", true);
	std::string script(
	    R"""(
a := 3
b := 5
copy <<EOS ~params
a=$(a)
b=$(b)
EOS
call "###" ~params
sum = buffer.shift ~_result
if $(sum) != 8
  stop "expecting 8 but found: $(sum)"
endif
)""");
  script = replaceString(script, "###", fnSource2.c_str(), 1);
  writeText(fnSource.c_str(), script.c_str());
	std::string script2(R"""(
rc := $(a) + $(b)
copy "$(rc)" ~_result
)""");
  writeText(fnSource2.c_str(), script2.c_str());
	SearchEngine engine(*logger);
	engine.loadScript("example", fnSource.c_str());
	engine.selectScript("example");
  //engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}
TEST(ScriptTest, exit) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnSource2 = temporaryFile("sum.ses", "unittest", true);
  std::string script(R"""(
exit 3 global
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  //engine.setTrace("-");
  ASSERT_EQ(3, engine.testAndRun());
  delete logger;
}
TEST(ScriptTest, script) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnSource2 = temporaryFile("sum.ses", "unittest", true);
  std::string script(
      R"""(
script sum
  _rc := $(a) + $(b)
endscript
script max
  if $(a) > $(b)
    _rc := $(a)
  else
    _rc := $(b)
  endif
endscript
copy <<EOS ~params
a=3
b=5
EOS
call sum ~params
log "sum of 3 + 5: $(_rc)"
call max "a=-3" "b=5"
if "$(_rc)" -ne "5"
  stop "wrong max"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  //engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}
TEST(ScriptTest, assert) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnSource2 = temporaryFile("sum.ses", "unittest", true);
  std::string script(
      R"""(
a := 1
b = ""
_rc := 1
copy "" ~data
assert a b ~data ~_main _rc
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  //engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(ScriptTest, select) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(
copy "Buffer1" ~b1
copy "Buffer2" ~b2
select ~b2
if s/Buffer2/
  log "OK"
else
  stop "failed b2-1"
endif
select push ~b1
if s/Buffer1/
  log "OK"
else
  stop "failed b1"
endif
select pop
if s/Buffer2/<
  log "OK"
else
  stop "failed b2-2"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  //engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(ScriptTest, conditionalAssignment) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(
x := 3
x ?= 2
if $(x) != 3
  stop "ensure 3 $(x)"
endif
y ?= "Hi"
if "$(y)" -ne "Hi"
  stop "ensure y $(y)"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(ScriptTest, buildCppHeader) {
  // FEW_TESTS();
#ifdef NEVER
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  SearchEngine engine(*logger);
  engine.loadScript("resources/ses/c-standard-headers.ses", fnSource.c_str());
  engine.selectScript("c-standard-headers.ses");
  engine.defineVariable("mode", "unittest");
  engine.defineVariable("path", fnTarget.c_str());
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
#endif
}

TEST(ScriptTest, markStoreRestore) {
  //FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(
copy <<EOS ~_main
1: abcdefg
2: defghij
3: 1234567
EOS
move s/bc/
mark set search
mark save pos1
move s/hij/
mark save pos2
mark restore pos1
if "$(__mark)" -ne "1:5"
  stop "not 1:5: $(pos1)"
endif
mark set 3:2
if "$(__mark)" -ne "3:2"
  stop "not 3:2: $(__mark)"
endif
move 2:2
mark set
move 1:1
mark exchange
if "$(__mark)" -ne "1:1"
  stop "not 1:1: $(__mark)"
endif
if "$(__position)" -ne "2:2"
  stop "not 2:2 after exchange: $(__position)"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.setTrace("-");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

