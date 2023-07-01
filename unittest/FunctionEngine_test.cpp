/*
 * FunctionEngine_test.cpp
 *
 *  Created on: 17.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(FunctionEngineTest, bufferShiftPop) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("data.inp", "unittest", true);
  auto fnTarget = temporaryFile("data.out", "unittest", true);
  std::string script(
      R"""(
load! ~_main "$(input)"
line1 = buffer.shift ~_main
line3 = buffer.pop ~_main
if "$(line1)" -ne "Line1"
  stop "line1"
endif
if "$(line1)" -ne "Line1"
  stop "line1"
endif
store! ~_main "$(output)"
)""");
  writeText(fnSource.c_str(), script.c_str());
  std::string data(R"""(

Line1
Line2 ABC
Line3

)""");
  writeText(fnData.c_str(), data.c_str());

  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  engine.defineVariable("input", fnData.c_str());
  engine.defineVariable("$(output)", fnTarget.c_str());
  ASSERT_EQ(0, engine.testAndRun());
  auto contents = readAsString(fnTarget.c_str(), logger);
  ASSERT_STREQ(contents.c_str(), "Line2 ABC\n");
  delete logger;
}

TEST(FunctionEngineTest, bufferJoinSplitSort) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
count1 = buffer.split ~data "Jonny,Charly,Adam,Berta" ","
dummy = buffer.sort ~data
names = buffer.join ~data ";"
if!! $(count1) != 4
  stop "count1"
endif
if "$(names)" -ne "Adam;Berta;Charly;Jonny"
  stop "names"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}
TEST(FunctionEngineTest, mathRandom) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
number = math.random 20 10
log! "random: $(number)"
if!! $(number) > 20
  stop ">"
endif
if $(number) < 10
  stop "<"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}
TEST(FunctionEngineTest, osBasenameDirnameChangeextension) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
name = os.basename "/tmp/abc/def.txt"
if "$(name)" -ne "def.txt"
  stop! "basename: $(name)"
endif
name = os.dirname "/tmp/abc/def.txt"
if "$(name)" -ne "/tmp/abc"
  stop "dirname"
endif
name = os.changeextension "/tmp/abc/def.txt" ".doc"
if "$(name)" -ne "/tmp/abc/def.doc"
  stop "changeextension1"
endif
name = os.changeextension "/tmp/abc/.config" ".data"
if "$(name)" -ne "/tmp/abc/.config.data"
  stop "changeextension2"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, osCdPwd) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
dir = os.pwd
dir2 = os.cd "/home"
if "$(dir)" -ne "$(dir2)"
  stop! "$(dir) -ne $(dir2)"
endif
dir = os.pwd
if "$(dir)" -ne "/home"
  stop! "$(dir) -ne /home"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, osCdPushdPopd) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
dir = os.pwd
dummy = os.pushd "/home"
dir2 = os.pwd
if "$(dir2)" -ne "/home"
  stop! "1: $(dir2) -ne /home"
endif
dummy2 = os.popd
dir3 = os.pwd
if "$(dir3)" -ne "$(dir)"
  stop! "2: $(dir) -ne $(dir3)"
endif
log! "$(dir) $(dir2) $(dir3)"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, stringLengthSubstring) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
len = string.length "abcd"
if!! $(len) != 4
  stop! "len != 4"
endif
s1 = string.substring "123456" 3
if "$(s1)" -ne "3456"
  stop! "substr 1: $(s1)"
endif
s2 = string.substring "123456" 3 count=2
if "$(s2)" -ne "34"
  stop! "substr 2: $(s2)"
endif
s3 = string.substring "123456" 3 excluding=6
if "$(s3)" -ne "345"
  stop! "substr 3: $(s3)"
endif
s4 = string.substring "123456" 3 including=5
if "$(s4)" -ne "345"
  stop! "substr 4: $(s4)"
endif
log! "$(s1) $(s2) $(s3) $(s4)"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, stringReplace) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
s1 = string.replace "Hello" r/L/i "L" count=1
if "$(s1)" -ne "HeLlo"
  stop! "replace 1: $(s1)"
endif
s2 = string.replace "Hello" r/L/i "L"
if "$(s2)" -ne "HeLLo"
  stop! "replace 2: $(s2)"
endif
log! "$(s1) $(s2)"
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, osMkdirIsdirExists) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
rc = os.mkdir "/tmp/fetest"
if!! $(rc) != 1
  stop "mkdir"
endif
rc = os.isdir "/tmp/fetest"
if!! $(rc) != 1
  stop "isdir"
endif
rc = os.exists "/tmp/fetest"
if!! $(rc) != 1
  stop "exists"
endif
rc = os.exists "/tmp/rtlpfrmpft"
if!! $(rc) != 0
  stop "! exists"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, osListfiles) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("example.script", "unittest", true);
  std::string script(
      R"""(
rc = os.mkdir "/tmp/fetest"
rc = os.mkdir "/tmp/fetest/files"
store ~_main "/tmp/fetest/file1.txt"
store ~_main "/tmp/fetest/file2.doc"
store ~_main "/tmp/fetest/file3.txt"
store ~_main "/tmp/fetest/jonny.doc"
rc := os.listfiles ~_main "/tmp/fetest" including r/file/ excluding s/.doc/ files
if $(rc) != 2
  stop! "wrong count: $(rc)"
endif
if s/file2/
  stop "excluding failed"
endif
if s/jonny/
  stop "including failed"
endif
if s/files/
  stop "excluding dirs failed"
endif
if s/file1.txt/
  log "file1.txt"
else
  stop "missing file1.txt"
endif
if s/file3.txt/
  log "file3.txt"
else
  stop "missing file3.txt"
endif
)""");
  writeText(fnSource.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnSource.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, osCopy) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.ses", "unittest", true);
  auto fnSource = temporaryFile("input.txt", "unittest", true);
  auto fnTarget = temporaryFile("output.txt", "unittest", true);
  std::string script(
      formatCString(
          R"""(rc1 := os.copy '%s' '%s'
rc2 := os.exists '%s'
if!! $(rc2) != 1
  stop "copy: missing target rc-copy: $(rc1)"
endif
)""",
          fnSource.c_str(), fnTarget.c_str(), fnTarget.c_str()));
  writeText(fnScript.c_str(), script.c_str());
  writeText(fnSource.c_str(), "Hello");
  SearchEngine engine(*logger);
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  auto contents = readAsString(fnTarget.c_str(), logger);
  ASSERT_STREQ("Hello", contents.c_str());
  delete logger;
}

TEST(FunctionEngineTest, bufferDifference) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(copy <<EOS ~buffer1
line 1
line 2
EOS
copy <<EOS ~buffer2
line 1
line 2
line 3
EOS
copy <<EOS ~buffer3
line 1
line 2
EOS
copy <<EOS ~buffer4
line 1
Line 2
EOS
diff1 := buffer.difference ~buffer1 ~buffer2
if!! $(diff1) != 3
  stop! "wrong diff (buffer1 + buffer2): $(diff1) / 3"
endif
diff2 := buffer.difference ~buffer1 ~buffer3
if!! $(diff2) != 0
  stop! "wrong diff (buffer1 + buffer3): $(diff2) / 0"
endif
diff3 := buffer.difference ~buffer2 ~buffer1
if!! $(diff3) != 3
  stop! "wrong diff (buffer2 + buffer1): $(diff3) / 3"
endif
diff4 := buffer.difference ~buffer4 ~buffer1
if!! $(diff4) != 2
  stop "wrong diff (buffer4 + buffer1): $(diff2) / 2"
endif
)""");
  writeText(fnScript.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, stringIndex) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(ix := string.index "b" "a,b,c" ","
if!! $(ix) != 1
  stop! "unexpected index: $(ix) / 1"
endif
ix := string.index "a1" "a b c"
if!! $(ix) != -1
  stop! "unexpected index: $(ix) / -1"
endif
)""");
  writeText(fnScript.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

TEST(FunctionEngineTest, stringPiece) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.ses", "unittest", true);
  std::string script(
      R"""(item = string.piece 1 "a,b,c" ","
if "$(item)" -ne "b"
  stop! "unexpected item: $(item) / b"
endif
item = string.piece 2 "a b c"
if "$(item)" -ne "c"
  stop! "unexpected item: $(item) / c"
endif
item = string.piece 4 "a b c"
if "$(item)" -ne ""
  stop! "unexpected item: '$(item)' / ''"
endif
)""");
  writeText(fnScript.c_str(), script.c_str());
  SearchEngine engine(*logger);
  engine.loadScript("example", fnScript.c_str());
  engine.selectScript("example");
  ASSERT_EQ(0, engine.testAndRun());
  delete logger;
}

