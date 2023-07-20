/*
 * netknife.cpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */
#include "netknife.hpp"
#include "../net/net.hpp"
#include "tools.hpp"

namespace cppknife {

bool netknifeUnterTest = false;

void examples() {
  printf(
      R"""(# Starts the secrets server using unix sockets:
netknife secrets --address=/run/netknife.secrets
# Starts the secrets server using tcp
netknife secrets --address=0.0.0.0:12345

# converts the string "basic" into a 8-byte int:
netknife to-hex basic
)""");
}

/**
 * Manages the "secrets" sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 * @return 0: success Otherwise: the exit code.
 */
int secrets(ArgumentParser &parser, Logger &logger) {
  int rc = 9;
  return rc;
}

/**
 * Manages the "secrets" sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 * @return 0: success Otherwise: the exit code.
 */
int toHex(ArgumentParser &parser, Logger &logger) {
  int rc = 0;
  auto count = parser.countValuesOf("data");
  for (size_t ix = 0; ix < count; ix++) {
    auto data = parser.asString("data", nullptr, ix);
    union {
      char _string[8];
      uint64_t _int;
    } transformer;
    memset((void*) transformer._string, ' ', sizeof transformer._string);
    size_t length = strlen(data);
    length = min(sizeof transformer._string, length);
    for (size_t ix = 0; ix < length; ix++) {
      transformer._string[7 - ix] = data[ix];
    }
    printf("0x%016lx %s\n", transformer._int, data);
  }
  return rc;
}
int netknife(int argc, char **argv, Logger *loggerExtern) {
  auto logger =
      loggerExtern == nullptr ?
          buildMemoryLogger(200, LV_SUMMARY) : loggerExtern;
  int rc = 0;
  ArgumentParser parser("netknife", logger, "network services");
  parser.add("--log-level", "-l", DT_NAT,
      "Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG",
      "5");
  parser.add("--verbose", "-v", DT_BOOL, "Show more information");
  parser.add("--examples", nullptr, DT_BOOL, "Show usage examples", "false");
  parser.addMode("mode", "What should be done:", "to-gps,from-gps");
  ArgumentParser secretsParser("secrets", logger,
      "A daemon storing and offering secrets");
  parser.addSubParser("mode", "secrets", secretsParser);
  secretsParser.add("address", nullptr, DT_STRING,
      "<ip>:<port or <path_socket>", "/run/netknife.secrets",
      "localhost:58777|/run/netknife.secrets");
  ArgumentParser toHexParser("to-hex", logger,
      "Converts a string into a hex digit");
  parser.addSubParser("mode", "to-hex", toHexParser);
  toHexParser.add("data", nullptr, DT_STRING, "That string will be converted",
      nullptr, "hello|general", true);
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
  } else if (parser.asBool("examples")) {
    examples();
  } else {
    auto level = static_cast<LogLevel>(parser.asInt("log-level", LV_SUMMARY));
    logger->setLevel(level);
    try {
      if (parser.isMode("mode", "secrets")) {
        rc = secrets(parser, *logger);
      } else if (parser.isMode("mode", "to-hex")) {
        rc = toHex(parser, *logger);
      } else {
        printf("%s\n", parser.usage("unknown mode", nullptr, false).c_str());
      }
    } catch (ArgumentException &e) {
      logger->say(LV_ERROR, e.message());
    }
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}

} /* namespace */
