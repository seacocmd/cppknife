/*
 * textknife.cpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "textknife.hpp"
namespace cppknife {

void examples() {
  printf(
      R"""(# Executes a script "build_summary.ses" with 2 parameters: "source" and "target":
sesknife build_summary.ses -Dsource=data/year2023.csv -Dtarget=/tmp/result.doc
# Describe the usage:
sesknife --help
)""");
}

int processScript(const ArgumentParser &parser, SearchEngine &engine,
    Logger &logger, const char *input = nullptr) {
  auto scriptFile = parser.asString("script");
  auto scriptName = basename(scriptFile);
  int rc = 0;
  if (scriptName.empty()) {
    logger.error("missing script");
    rc = 3;
  } else {
    engine.loadScript(scriptName.c_str(), scriptFile);
    engine.selectScript(scriptName.c_str());
    if (input != nullptr) {
      auto lines = readAsList(input, &logger);
      auto buffer = engine.getBuffer("_main");
      buffer->setLines(lines);
      buffer->setFilename(input);
    }
    auto variableCount = parser.countValuesOf("define");
    for (size_t ix = 0; ix < variableCount; ix++) {
      std::vector<std::string> parts = splitCString(
          parser.asString("define", nullptr, ix), "=", 2);
      engine.defineVariable(parts[0].c_str(),
          parts.size() <= 1 ? "" : parts[1].c_str());
    }
    rc = engine.testAndRun(scriptName.c_str());
  }
  return rc;
}

int searchEngineScriptKnife(int argc, char **argv, Logger *loggerExtern) {
  auto logger =
      loggerExtern == nullptr ?
          buildMemoryLogger(200, LV_SUMMARY) : loggerExtern;
  double start = nowAsDouble();
  int rc = 0;
  ArgumentParser parser("sesknife", logger,
      "A search engine script processor.");
  parser.add("--log-level", "-l", DT_NAT,
      "Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG",
      "5");
  parser.add("--verbose", "-v", DT_BOOL, "Show more information");
  parser.add("--trace", "-t", DT_BOOL, "Log each statement of the script.",
      "false");
  parser.add("--define", "-D", DT_STRING,
      "Defines a variable/parameter. Can be used multiple times", nullptr,
      "path=/usr/local/bin|count=10", true);
  parser.add("--examples", nullptr, DT_BOOL, "Show usage examples", "false");
  parser.add("script", nullptr, DT_FILE, "The script to process.");
  parser.add("input", nullptr, DT_STRING,
      "The files to process. Use '' for no files.", nullptr, "", true);

  auto verbose = parser.asBool("verbose");
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
  } else if (parser.asBool("examples")) {
    examples();
  } else {
    auto level = static_cast<LogLevel>(parser.asInt("log-level", LV_SUMMARY));
    logger->setLevel(level);
    SearchEngine engine(*logger);
    if (parser.asBool("trace")) {
      engine.setTrace("-", false);
    }
    auto inputFileCount = parser.countValuesOf("input");
    if (inputFileCount == 0) {
      rc = processScript(parser, engine, *logger);
    } else {
      for (size_t ix = 0; ix < inputFileCount; ix++) {
        processScript(parser, engine, *logger,
            parser.asString("input", nullptr, ix));
      }
    }
    if (verbose) {
      logger->say(LV_SUMMARY,
          timeDifferenceToString(nowAsDouble() - start,
              "= runtime: %hh%mm%s.%3s"));
    }
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}

}
/* namespace */
