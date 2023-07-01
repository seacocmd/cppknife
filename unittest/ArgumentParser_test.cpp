/*
 * ArgumentParser_test.cpp
 *
 *  Created on: 18.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../tools/ArgumentParser.hpp"

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

using namespace cppknife;
TEST(ArgumentParserTest, basics) {
  FEW_TESTS();
  auto logger = buildMemoryLogger();
  ArgumentParser parser("My App", logger);
  parser.add("--verbose", "-v", DT_BOOL, "Print more info");
  parser.addMode("mode", "What should be done", "draw,info");
  ArgumentParser drawParser("draw", logger);
  parser.addSubParser("mode", "draw", drawParser);
  drawParser.add("input", nullptr, DT_STRING, "Input file", "-");
  drawParser.add("--width", "-w", DT_NAT, "Drawing width in pixel", "-");
  ArgumentParser infoParser("info", logger);
  infoParser.add("input", nullptr, DT_STRING, "Input file", "-");
  const char *args[] = { "-v", "draw", "a.png", "-w", "998" };
  ArgVector argVector(5, const_cast<char**>(args));
  auto verbose = parser.asBool("verbose");
  ASSERT_TRUE(verbose);
  ASSERT_TRUE(parser.parseAndCheck(argVector));
  if (parser.isMode("mode", "draw")) {
    auto input = parser.asString("input");
    ASSERT_STREQ("a.png", input);
    auto width = parser.asInt("width");
    ASSERT_EQ(998, width);
  }
  delete logger;
}

TEST(ArgumentParserTest, shortcuts) {
  FEW_TESTS();
  auto logger = buildMemoryLogger();
  ArgumentParser parser("My App", logger);
  parser.add("--verbose", "-v", DT_BOOL, "Print more info");
  parser.add("--verify", nullptr, DT_BOOL, "?");
  parser.addMode("mode", "What should be done", "draw,down");
  ArgumentParser drawParser("draw", logger);
  parser.addSubParser("mode", "draw", drawParser);
  drawParser.add("input", nullptr, DT_STRING, "Input file", "-");
  drawParser.add("--width", "-w", DT_NAT, "Drawing width in pixel", "-");
  drawParser.add("--wide", "-W", DT_NAT, "?", "-");
  ArgumentParser infoParser("down", logger);
  infoParser.add("input", nullptr, DT_STRING, "Input file", "-");
  const char *args[] = { "--verb", "dr", "a.png", "--widt", "998" };
  ArgVector argVector(5, const_cast<char**>(args));
  auto verbose = parser.asBool("verbose");
  ASSERT_TRUE(verbose);
  ASSERT_TRUE(parser.parseAndCheck(argVector));
  if (parser.isMode("mode", "drawn")) {
    auto input = parser.asString("input");
    ASSERT_STREQ("a.png", input);
    auto width = parser.asInt("width");
    ASSERT_EQ(998, width);
  }
  delete logger;
}

TEST(ArgumentParserTest, subHelp) {
  //FEW_TESTS();
  auto logger = buildMemoryLogger();
  ArgumentParser parser("myApp", logger, "Does the real needed things.");
  parser.add("--verbose", "-v", DT_BOOL, "Print more info");
  parser.add("--verify", nullptr, DT_BOOL, "?");
  parser.addMode("mode", "What should be done", "draw");
  ArgumentParser drawParser("draw", logger);
  parser.addSubParser("mode", "draw", drawParser);
  drawParser.add("input", nullptr, DT_STRING, "Input file", "-");
  drawParser.add("--width", "-w", DT_NAT, "Drawing width in pixel", "-");
  const char *args[] = { "--verb", "dr", "--help" };
  ArgVector argVector(3, const_cast<char**>(args));
  auto verbose = parser.asBool("verbose");
  ASSERT_TRUE(verbose);
  ASSERT_FALSE(parser.parseAndCheck(argVector));
  ASSERT_EQ(19,
      countCString(parser.usageMessage().c_str(), parser.usageMessage().size(),
          "\n"));
  delete logger;
}

TEST(ArgumentParserTest, help) {
  //FEW_TESTS();
  auto logger = buildMemoryLogger();
  ArgumentParser parser("myApp", logger, "Does the real needed things.");
  parser.add("--verbose", "-v", DT_BOOL, "Print more info");
  parser.addMode("mode", "What should be done", "draw,down");
  ArgumentParser drawParser("draw", logger, "Draws something.");
  parser.addSubParser("mode", "draw", drawParser);
  drawParser.add("input", nullptr, DT_STRING, "Input file", "-");
  drawParser.add("--width", "-w", DT_NAT, "Drawing width in pixel", "-");
  drawParser.add("--wide", "-W", DT_NAT, "?", "-");
  ArgumentParser infoParser("info", logger, "Informs about something.");
  infoParser.add("output", nullptr, DT_STRING, "Output file", "-");
  parser.addSubParser("mode", "info", infoParser);
  const char *args[] = { "--help" };
  ArgVector argVector(1, const_cast<char**>(args));
  auto verbose = parser.asBool("verbose");
  ASSERT_TRUE(verbose);
  ASSERT_FALSE(parser.parseAndCheck(argVector));
  ASSERT_EQ(28,
      countCString(parser.usageMessage().c_str(), parser.usageMessage().size(),
          "\n"));
  delete logger;
}

#ifdef FEATURE_TEST
class Person
{
public:
  const char *_name;
  int _year;
  int _id;
  static int _nextId;
  Person (const char *name, int year) :
  _name (name), _year (year), _id (_nextId++)
  {
    printf ("created: %d: %s\n", _id, _name);
  }
  Person (const Person &other) :
  _name (other._name), _year (other._year), _id (_nextId++)
  {
    printf ("copy %d from %d %s\n", _id, other._id, _name);
  }
  ~Person() {
    printf("deleted: %d %s\n", _id, _name);
  }
  Person& operator =(const Person& other) {
    _name = other._name;
    _year = other._year;
    _id = _nextId++;
    printf("assign %d from %d %s\n", _id, other._id, _name);
    return *this;
  }
};
int Person::_nextId = 1;

TEST(ArgumentParserTest, vector)
{
  if (true)
  {
    std::vector<Person> v;
    v.reserve(10);
    printf("push back:\n");
    printf("size: %lu capacity: %lu\n", v.size(), v.capacity());
    v.push_back (Person ("Adam", 33));
    printf("size: %lu capacity: %lu\n", v.size(), v.capacity());
    auto p0 = *v.begin();
    printf ("first: %d %s\n", p0._id, p0._name);
    v.push_back (Person ("Berta", 44));
    printf("size: %lu capacity: %lu\n", v.size(), v.capacity());
    auto p3 = *(v.begin()+1);
    printf ("second: %d %s\n", p3._id, p3._name);
    printf("Fetch\n");
    auto p1 = v[0];
    auto p2 = v[1];
    printf ("v[0]: %d %s\n", p1._id, p1._name);
    printf ("v[1]: %d %s\n", p2._id, p2._name);
    printf("leaving block\n");
  }
  printf("end\n");
}
TEST(ArgumentParserTest, vectorPtr)
{
  if (true)
  {
    std::vector<Person*> v;
    printf("push back:\n");
    auto pA = new Person ("Adam", 33);
    printf("size: %lu capacity: %lu\n", v.size(), v.capacity());
    v.push_back (pA);
    printf("size: %lu capacity: %lu\n", v.size(), v.capacity());
    auto p0 = *v.begin();
    printf ("first: %d %s\n", p0->_id, p0->_name);
    auto pB = new Person ("Berta", 44);
    v.push_back (pB);
    printf("size: %lu capacity: %lu\n", v.size(), v.capacity());
    auto p3 = *(v.begin()+1);
    printf ("second: %d %s\n", p3->_id, p3->_name);
    printf("Fetch\n");
    auto p1 = v[0];
    auto p2 = v[1];
    printf ("v[0]: %d %s\n", p1->_id, p1->_name);
    printf ("v[1]: %d %s\n", p2->_id, p2->_name);
    printf("leaving block\n");
    delete pA;
    delete pB;
  }
  printf("end\n");
}
#endif

TEST(ArgumentParserTest, parameterSetFormat) {
  FEW_TESTS();
  Parameter param("--status", "-s", DT_STRING, "Help");
  param.setFormat("(in)?active",
      "not a status: %v~ Use 'active' or 'inactive'");
  std::string value("active");
  Argument argument1(&param, value);
  argument1.validate();
  value = "abc";
  Argument argument2(&param, value);
  try {
    argument2.validate();
    FAIL();
  } catch (const ArgumentException &e) {
    ASSERT_STREQ(
        "status: wrong value (syntax): not a status: abc Use 'active' or 'inactive'",
        e.message().c_str());
  }
}
TEST(ArgumentParserTest, parameterRegExpr) {
  //FEW_TESTS();
  Parameter param("--search", "-s", DT_REGEXPR, "Help");
  std::string value("!abc|123!i");
  Argument argument1(&param, value);
  argument1.validate();
  auto expr1 = argument1.asRegExpr(0);
  ASSERT_TRUE(std::regex_match("abc", expr1));
  //std::string x = expr1.what();
  ASSERT_TRUE(std::regex_match("123", expr1));
  ASSERT_FALSE(std::regex_match("1ab3", expr1));
  value = "/abc";
  Argument argument2(&param, value);
  try {
    argument2.validate();
    FAIL();
  } catch (const ArgumentException &e) {
    ASSERT_STREQ("search: not 2 delimiter / for reg. expr.: /abc",
        e.message().c_str());
  }
  value = "/abc/x";
  Argument argument3(&param, value);
  try {
    argument3.validate();
    FAIL();
  } catch (const ArgumentException &e) {
    ASSERT_STREQ(
        "search: unknown flag(s) x for reg. expr.: /abc/x Use: i(gnore case) m(ultiline)",
        e.message().c_str());
  }
}
