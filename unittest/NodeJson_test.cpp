/*
 * JsonReader_test.cpp
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../text/text.hpp"

#include "google_test.hpp"
namespace cppknife {

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return
#ifndef TEST
#define TEST(a, b) void a::b()
#endif

TEST(NodeJsonTest, basic) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.json", "unittest", true);
  writeText(fnSource.c_str(),
      R"""({
"number": 10.5,
"string": "hello\tWorld",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz\tabc\n" }
}
)""");
  FileLinesStream stream(fnSource.c_str(), *logger);
  JsonReader reader(*logger);
  auto root = reader.parse(stream);
  ASSERT_TRUE(root != nullptr);
  ASSERT_EQ(10.5, root->byAttribute("number")->asDouble());
  ASSERT_STREQ("hello\tWorld", root->byAttribute("string")->asString());
  ASSERT_TRUE(root->byAttribute("bool")->asBool());
  ASSERT_EQ(1, root->byAttribute("array")->byIndexConst(0)->asInt());
  ASSERT_EQ(2, root->byAttribute("array")->byIndexConst(1)->asInt());
  ASSERT_EQ(3, root->byAttribute("array")->byIndexConst(2)->asInt());
  ASSERT_EQ(47, root->byAttribute("map")->byAttribute("a")->asInt());
  ASSERT_STREQ("xyz\tabc\n",
      root->byAttribute("map")->byAttribute("b")->asString());
  delete root;
  delete logger;
}

TEST(NodeJsonTest, root) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));

  auto fnSource = temporaryFile("test1.json", "unittest", true);
  writeText(fnSource.c_str(),
      R"""({
"number": 10.5,
"string": "hello",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz" }
}
)""");
  FileLinesStream stream(fnSource.c_str(), *logger);
  JsonReader reader(*logger);
  auto root = reader.parse(stream);
  ASSERT_EQ(JNT_MAP, root->type());
  delete root;
  delete logger;
}
TEST(NodeJsonTest, decodeEncode) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  const char *data =
      R"""({
"number": 10.5,
"string": "hello\n",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz", "array2": [ 0 ], "map2": { "key": null} }
}
)""";
  std::string error;
  auto root = NodeJson::encode(data, error, *logger);
  auto data2 = NodeJson::decode(root, 2);
  auto data3 = NodeJson::decode(root, 0);
  auto fnTarget = temporaryFile("1.json");
  writeText(fnTarget.c_str(), data2.c_str());
  ASSERT_EQ(data2,
      R"""({
"array": [
  1,
  2,
  3
  ],
"bool": true,
"map": {
  "a": 47,
  "array2": [
    0
    ],
  "b": "xyz",
  "map2": {
    "key": null
    }
  },
"number": 10.5,
"string": "hello\n"
})""");
  writeText(fnTarget.c_str(), data3.c_str());
  ASSERT_STREQ(data3.c_str(),
      R"""({"array":[1,2,3],"bool":true,"map":{"a":47,"array2":[0],"b":"xyz","map2":{"key":null}},"number":10.5,"string":"hello\n"})""");
  delete root;
  delete logger;
}

TEST(NodeJsonTest, checkStructure) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));

  auto data =
      R"""({
"number": 10.5,
"string": "hello",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz" }
}
)""";
  std::string error;
  auto root = NodeJson::encode(data, error, *logger);
  NameAndType mandantory[] = { { "number", JDT_FLOAT },
      { "string", JDT_STRING }, { "array", JDT_ARRAY }, { "map", JDT_MAP }, {
          nullptr, JDT_UNDEFINED } };
  NameAndType optional[] = { { "bool", JDT_BOOL }, { "missing", JDT_INT }, {
      nullptr, JDT_UNDEFINED } };
  ASSERT_STREQ(root->checkStructure(mandantory, optional, true).c_str(), "");
  NameAndType mandantory2[] = { { "number", JDT_STRING }, { "string", JDT_INT },
      { "array", JDT_ARRAY }, { "map", JDT_ARRAY }, { "array", JDT_MAP }, {
          "string", JDT_FLOAT }, { "string", JDT_FLOAT_LIST }, { "dummy",
          JDT_FLOAT_LIST }, { nullptr, JDT_UNDEFINED } };
  ASSERT_STREQ(root->checkStructure(mandantory2, optional, true).c_str(),
      R"""(
number is not a string: 10.5
string is not an int: hello
map is not an array: <map>
array is not a map: <array>
string is not a float: hello
string is not a float list: hello
missing attribute dummy)""");
  NameAndType mandantory3[] = { { "number", JDT_FLOAT },
      { "string", JDT_STRING }, { nullptr, JDT_UNDEFINED } };
  ASSERT_STREQ(root->checkStructure(mandantory3, optional, true).c_str(),
      R"""(
unknown attribute: array
unknown attribute: map)""");
  delete root;
  delete logger;
}

TEST(NodeJsonTest, nodeByPath) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));

  auto data =
      R"""({
"number": 10.5,
"string": "hello",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz" }
}
)""";
  std::string error;
  auto root = NodeJson::encode(data, error, *logger);
  const char *path[] = { "map", "a", nullptr };
  auto node = root->nodeByPath(path);
  ASSERT_TRUE(nullptr != node);
  node = root->nodeByPath(path, JDT_INT);
  ASSERT_TRUE(nullptr != node);
  node = root->nodeByPath(path, JDT_BOOL);
  ASSERT_TRUE(nullptr == node);
  delete root;
  delete logger;
}

TEST(NodeJsonTest, nodeByPathExceptions) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));

  auto data =
      R"""({
"number": 10.5,
"string": "hello",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz" }
}
)""";
  std::string error;
  NodeJson &root = *NodeJson::encode(data, error, *logger);
  try {
    const char *path[] = { "[1]", nullptr };
    root.nodeByPath(path, JDT_STRING, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "[1] not an array: <map>");
  }
  try {
    const char *path[] = { "abc", nullptr };
    root["number"].nodeByPath(path, JDT_STRING, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "abc not a map: 10.5");
  }
  try {
    const char *path[] = { "abc", nullptr };
    root.nodeByPath(path, JDT_STRING, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "abc missing attribute: abc");
  }
  try {
    const char *path[] = { "bool", nullptr };
    root.nodeByPath(path, JDT_STRING, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "bool unexpected type: bool / string");
  }
  delete &root;
  delete logger;
}

TEST(NodeJsonTest, encodeByFile) {
  //FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));
  auto data =
      R"""({
"array": [
  1,
  2,
  3
  ],
"bool": true,
"map": {
  "a": 47,
  "array2": [
    0
    ],
  "b": "xyz",
  "map2": {
    "key": null
    }
  },
"number": 10.5,
"string": "hello"
})""";
  std::string error;
  auto fnSource = temporaryFile("test1.json", "unittest", true);
  writeText(fnSource.c_str(), data);
  auto root = NodeJson::encodeFromFile(fnSource.c_str(), error, *logger);
  ASSERT_TRUE(root != nullptr);
  std::string data2;
  root->addAsString(data2, 2, 0);
  ASSERT_STREQ(data, data2.c_str());
  delete root;
  delete logger;
}

TEST(NodeJsonTest, isNull) {
  //FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));
  std::string error;
  NodeJson &root = *NodeJson::encode(
      R"""({"Null": null, "One": 1, "Str": "x", "Array": [] })""", error,
      *logger);
  ASSERT_TRUE(root["Null"].isNull());
  ASSERT_FALSE(root["One"].isNull());
  ASSERT_FALSE(root["Array"].isNull());
  delete &root;
  delete logger;
}

TEST(NodeJsonTest, operatorBracket) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));
  std::string error;
  MapJson &root =
      dynamic_cast<MapJson&>(*NodeJson::encode(
          R"""({
"array": [
  1,
  2,
  3
  ],
"bool": true,
"map": {
  "a": 47,
  "array2": [
    0
    ],
  "b": "xyz",
  "map2": {
    "key": null
    }
  },
"number": 10.5,
"string": "hello"
})""",
          error, *logger));

  ASSERT_EQ(root["array"][2].asInt(), 3);
  ASSERT_EQ(root["bool"].asBool(), true);
  ASSERT_EQ(root["map"]["array2"][(int) 0].asInt(), 0);
  ASSERT_EQ(root["map"]["map2"]["key"].isNull(), true);
  delete &root;
  delete logger;
}
TEST(NodeJsonTest, exceptions) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));
  std::string error;
  MapJson &root =
      dynamic_cast<MapJson&>(*NodeJson::encode(
          R"""({
"array": [ 1,  2,  3 ],
"bool": true,
"map": {
  "a": 47,
  "array2": [ 0 ],
  "b": "xyz",
  "map2": {
    "key": null
    },
  "null": null
  },
"number": 10.5,
"string": "hello"
})""",
          error, *logger));
  try {
    root["map"][1];
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonMap has no index access");
  }
  try {
    root["bool"]["value"];
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonValue has attribute value");
  }
  try {
    root["map"].asBool();
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonMap has no bool value");
  }
  try {
    root["map"].asDouble();
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonMap has no double value");
  }
  try {
    root["array"].asInt();
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonArray has no int value");
  }
  try {
    root["map"].asString();
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonMap has no string value");
  }
  try {
    root.byAttribute("dummy", true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "unknown attribute: dummy");
  }
  try {
    root.byAttributeConst("dummy", true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "unknown attribute: dummy");
  }
  try {
    root["array"].byAttribute("dummy", true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(),
        "node with type JsonArray has no attribute dummy");
  }
  try {
    root["array"].byAttributeConst("dummy", true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(),
        "node with type JsonArray has no attribute dummy");
  }
  try {
    root["map"].byIndex(1, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonMap has no index 1");
  }
  try {
    root["map"].byIndexConst(1, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node with type JsonMap has no index 1");
  }
  try {
    root["array"].byIndexConst(22, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "no entry at [22]: [0..3]");
  }
  try {
    root["array"].byIndex(9, true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "no entry at [9]: [0..3]");
  }
  try {
    root["array"][-1];
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "no entry at [-1]: [0..3]");
  }
  try {
    root["array"].map();
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "the node <array> has no map");
  }
  try {
    root["map"].array(true);
    ASSERT_TRUE(false);
  } catch (const JsonError &exc) {
    ASSERT_STREQ(exc.message(), "node <map> has no array");
  }
  delete &root;
  delete logger;
}
TEST(NodeJsonTest, addBlanks2) {
  FEW_TESTS();
  std::string item("x");
  NodeJson::addBlanks(200, item);
  ASSERT_EQ(item.size(), 201);
  ASSERT_EQ(item[0], 'x');
  ASSERT_EQ(item[1], ' ');
  ASSERT_EQ(item[200], ' ');
}
TEST(NodeJsonTest, erase) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));
  std::string error;
  MapJson &root = dynamic_cast<MapJson&>(*NodeJson::encode(
      R"""({
"array": [ 1,  2,  3 ],
"bool": true,
})""", error, *logger));
  ASSERT_TRUE(root.hasAttribute("bool"));
  root.erase("bool", true);
  ASSERT_FALSE(root.hasAttribute("bool"));
  delete &root;
  delete logger;
}
TEST(NodeJsonTest, addMaskedLabel) {
  FEW_TESTS();
  std::string item("x");
  NodeJson::addBlanks(200, item);
  ASSERT_EQ(item.size(), 201);
  ASSERT_EQ(item[0], 'x');
  ASSERT_EQ(item[1], ' ');
  ASSERT_EQ(item[200], ' ');
}
TEST(NodeJsonTest, addLabelWithMetaChar) {
  FEW_TESTS();
  auto logger(buildMemoryLogger(100, LV_DEBUG));
  std::string error;
  MapJson &root = dynamic_cast<MapJson&>(*NodeJson::encode(R"""({
})""", error,
      *logger));
  auto node = new ValueJson(JDT_NULL, nullptr);
  root.add("\n", node);
  ASSERT_TRUE(root.hasAttribute("\\n"));
  delete &root;
  delete logger;
}
TEST(NodeJsonTest, change) {
  FEW_TESTS();
  auto node = new ValueJson(JDT_STRING, "Hello");
  node->change("Hi");
  ASSERT_STREQ(node->asString(), "Hi");
  delete node;
}
}
