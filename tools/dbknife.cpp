/*
 * textknife.cpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "dbknife.hpp"
#include "../os/os.hpp"
#include "../db/db.hpp"
#include "tools.hpp"

namespace cppknife {

bool dbknifeUnderTest = false;

void examples() {
  printf(
      R"""(# Create a mysql database "dbwiki" and the administrator user "jonny" with passwd "TopSecret":
dbknife create-db --driver=mysql --user=jonny:TopSecret --administrator dbwiki

# Describe the postgresql database "dbstock":
dbknife info --driver=postgres dbstock
# Describe the table "persons" in the mysql database "mycompany" with details:
dbknife info --details --driver=mysql mycompany.persons

# show all databases, the system databases too:
dbknife list-dbs --systems --driver=mysql
# show the databases matching the pattern "db*":
dbknife list-dbs --driver=mysql --pattern=db*

# Describe the usage:
dbknife --help
)""");
}


int daemon(ArgumentParser &parser, Logger &logger) {
  int rc = 0;
  return rc;
}
void _authentificate(ArgumentParser &parser, Logger &logger,
    VeilEngine &veilEngine) {
  std::string user;
  std::string code;
  if (!dbknifeUnderTest) {
    const char *current = nullptr;
    if ((current = parser.asString("user")) != nullptr) {
      user = current;
    } else {
      auto user2 = getenv("USER");
      if (user2 != nullptr) {
        user = user2;
      }
    }
    if ((current = parser.asString("code")) != nullptr) {
      code = current;
    } else {
      auto code2 = getenv("CODE");
      if (code2 != nullptr) {
        code = code2;
      }
    }
    if (user.empty()) {
      throw ArgumentException("authentification failed");
    }
    if (code.empty()) {
      char buffer[128];
      printf("Password for user %s\n", user.c_str());
      auto code2 = fgets(buffer, sizeof buffer, stdin);
      code = code2;
      trimString(code);
    }
    if (!veilEngine.verify(user.c_str(), code.c_str())) {
      throw ArgumentException("authentification failed");
    }
  }
}
int _veilOrUnveil(ArgumentParser &parser, Logger &logger, bool isVeil) {
  int rc = 0;
  auto application = parser.asString("application");
  auto input = parser.asString("input");
  std::string output = parser.asString("output");
  bool isStdIn = strcmp("~", input) == 0;
  bool isStdOut = "~" == output;
  bool inPlace = output.empty();
  if (inPlace) {
    output = input;
    output += "~";
  }
  VeilEngine engine(application, logger);
  _authentificate(parser, logger, engine);
  FILE *fpInput = isStdIn ? stdin : fopen(input, "rb");
  FILE *fpOutput = isStdOut ? stdin : fopen(output.c_str(), "wb");
  const char *line = nullptr;
  char buffer[0x10000];
  char buffer2[0x10000 + 5];
  int lineNo = 0;
  while ((line = fgets(buffer, sizeof buffer, fpInput)) != nullptr) {
    lineNo++;
    if (lineNo == 1) {
      if (isVeil) {
        if (!startsWith(line, 2, "# ")) {
          logger.say(LV_ERROR,
              formatCString(
                  "%s: not a clear text configuration file: missing '# '",
                  input));
          rc = 9;
          break;
        }
      } else {
        if (strlen(line) < 6) {
          logger.say(LV_ERROR,
              formatCString(
                  "%s: not an encrypted configuration file: first line to short.",
                  input));
          rc = 9;
          break;
        }
      }
    }
    engine.reset();
    if (isVeil) {
      engine.veil(line, buffer2, sizeof buffer2);
    } else {
      engine.unveil(line, buffer2, sizeof buffer2);
    }
    if (!isVeil && lineNo == 1) {
      if (!startsWith(buffer2, 2, "# ")) {
        logger.say(LV_ERROR,
            formatCString(
                "%s: not an encrypted configuration file: first line starts not with '# '.",
                input));
        rc = 9;
        break;
      }
    }
    fputs(buffer2, fpOutput);
  }
  if (!isStdIn) {
    fclose(fpInput);
  }
  if (!isStdOut) {
    fclose(fpOutput);
  }
  if (rc == 0 && inPlace && fileExists(output.c_str())) {
    unlink(input);
    rename(output.c_str(), input);
  }
  return rc;
}
SqlDriver* _getDriver(const char *driver, const char *database,
    SecretConfiguration &configuration, Logger &logger, bool readOnly = false) {
  SqlDriver *dbms = nullptr;
  if (driver == nullptr || strcmp(driver, "default") == 0) {
    driver = configuration.asString("default.driver");
  }
  LoginData login(driver, configuration, database);
#if ! defined IGNORE_MYSQL
  if (strcmp(driver, "mysql") == 0) {
    MySql *dbms2 = new MySql(database == nullptr ? "mysql" : database,
        login._user, login._code, logger, login._host, login._port);
    dbms2->connect();
    dbms = dbms2;
  }
#endif
#if ! defined IGNORE_POSTGRES
  if (strcmp(driver, "postgres") == 0) {
    Postgres *dbms2 = new Postgres(database == nullptr ? "postgres" : database,
        login._user, login._code, logger, login._host, login._port);
    dbms2->connect();
    dbms = dbms2;
  }
#endif
  if (dbms == nullptr) {
    throw InternalError("_getDriver: unknown driver ", driver);
  }
  return dbms;
}
std::vector<std::string> _getAllDbs(const char *driver, bool withSystemTables,
    SecretConfiguration &configuration, Logger &logger) {
  std::vector<std::string> rc;
  SqlDriver *dbms = _getDriver(driver, nullptr, configuration, logger);
  rc = dbms->allDbs(withSystemTables);
  delete dbms;
  return rc;
}
int createDb(ArgumentParser &parser, Logger &logger) {
  int rc = 0;
  SecretConfiguration configuration("/etc/cppknife/db.conf", "db", logger);

  auto driver = parser.asString("driver");
  auto dbName = parser.asString("database");
  auto user = parser.asString("user");
  auto readOnly = parser.asBool("read-only");
  auto isAdministrator = parser.asBool("administrator");
  SqlDriver *dbms = _getDriver(driver, nullptr, configuration, logger);
  const char *user2 = nullptr;
  const char *code2 = nullptr;
  std::vector<std::string> parts;
  if (user != nullptr) {
    parts = splitCString(user, ":", 2);
    if (parts.size() != 2) {
      throw ArgumentException(
          formatCString("create-db-and-user: missing ':' in user: %s", user));
    }
    user2 = parts[0].c_str();
    code2 = parts[1].c_str();
  }
  dbms->createDbAndUser(dbName, user2, code2, isAdministrator, readOnly);
  delete dbms;
  return rc;
}

void infoOfTable(const char *database, const char *table, const char *driver,
    bool withDetails, SecretConfiguration &configuration, Logger &logger) {
  SqlDriver *dbms = _getDriver(driver, database, configuration, logger);
  logger.say(LV_INFO, formatCString("= database %s", database));
  auto tables = dbms->allTables();
  if (table == nullptr) {
    table = "*";
  }
  auto toProcess = SimpleMatcher::filterMatches(table, tables);
  TableSummary summary;
  for (auto name : toProcess) {
    summary.clear();
    dbms->summaryOf(name.c_str(), summary);
    logger.say(LV_INFO,
        formatCString("%s: rows: %d columns: %d", name.c_str(),
            summary._rowCount, summary._colCount));
  }
}
int info(ArgumentParser &parser, Logger &logger) {
  int rc = 0;
  SecretConfiguration configuration("/etc/cppknife/db.conf", "db", logger);

  auto driver = parser.asString("driver");
  bool withDetails = parser.asBool("details");
  auto dbName = parser.asString("what");
  if (dbName == nullptr) {
    dbName = "*";
  }
  SqlDriver *dbms = _getDriver(driver, nullptr, configuration, logger);
  auto dbs = dbms->allDbs(false);
  if (strchr(dbName, '.') == nullptr) {
    auto dbs2 = SimpleMatcher::filterMatches(dbName, dbs);
    for (auto db : dbs2) {
      infoOfTable(db.c_str(), nullptr, driver, withDetails, configuration,
          logger);
    }
    delete dbms;
  } else {
    auto parts = splitCString(dbName, ".", 2);
    auto dbs2 = SimpleMatcher::filterMatches(parts[0].c_str(), dbs);
    for (auto db : dbs2) {
      infoOfTable(db.c_str(), parts[1].c_str(), driver, withDetails,
          configuration, logger);
    }
  }
  return rc;
}
int listDbs(ArgumentParser &parser, Logger &logger) {
  int rc = 0;
  SecretConfiguration configuration("/etc/cppknife/db.conf", "db", logger);

  auto driver = parser.asString("driver");
  auto pattern = parser.asString("pattern");
  auto withSystemTables = parser.asBool("system");
  auto dbs = _getAllDbs(driver, withSystemTables, configuration, logger);
  if (pattern != nullptr) {
    dbs = SimpleMatcher::filterMatches(pattern, dbs);
  }
  for (auto name : dbs) {
    logger.say(LV_INFO, name.c_str());
  }
  return rc;
}
int license(ArgumentParser &parser, Logger &logger) {
  auto rc = 0;
  std::string host = parser.asString("host");
  auto secret3 = parser.asString("secret3");
  auto code = getenv("CPPKNIFE");
  auto code2 = formatCString("%.2f", 3.14);
  extern bool dbknifeUnderTest;
  if (!dbknifeUnderTest
      && (code == nullptr || strcmp(code, code2.c_str()) != 0)) {
    logger.say(LV_ERROR, "precondition failed");
  } else {
    if (host == "*") {
      host = readAsString("/etc/hostname");
      trimString(host);
      logger.say(LV_DETAIL, formatCString("= host: %s", host).c_str());
    }
  }
  auto fn = temporaryFile("example.license");
  std::vector<std::string> additionalSecrets;
  std::string application1;
  for (size_t ix = 0; ix < parser.countValuesOf("additional-secret"); ix++) {
    auto arg = parser.asString("additional-secret", nullptr, ix);
    if (arg != nullptr) {
      if (strchr(arg, '=') == nullptr) {
        throw ArgumentException(
            "--additional-secret: wrong format, missing '='", arg);
      }
      if (strchr(arg, '.') == nullptr) {
        throw ArgumentException(
            "--additional-secret: wrong format, missing '.'", arg);
      }
      if (application1.empty() && startsWith(arg, -1, "application.")) {
        application1 = arg;
      } else {
        additionalSecrets.push_back(arg);
      }
    }
  }
  if (application1.empty()) {
    throw ArgumentException("missing at least one application entry");
  }
  auto parts = splitCString(application1.c_str(), "=", 2);
  VeilEngine::buildConfiguration(fn.c_str(), host.c_str(), parts[0].c_str(),
      parts[1].c_str(), secret3, logger, &additionalSecrets);
  logger.say(LV_SUMMARY, formatCString("= created: %s", fn.c_str()));
  return rc;
}
void mailerSend(const char *filename, Logger &logger) {

  auto contents = readAsString(filename, &logger);
  auto lines = splitCString(contents.c_str(), "\n");
// At least 4 lines: receiver options subject text1 ...
  if (lines.size() < 4) {
    logger.say(LV_ERROR,
        formatCString("%s: too few lines: %d", filename, lines.size()));
  } else {
    bool encrypted = !startsWith(lines[1].c_str(), lines[1].size(), "options:");
    if (encrypted) {
      PortableRandom random0;
      CharRandom trans(random0, nullptr, CC_ASCII95);
      char buffer[0x10000];
      for (size_t ix = 0; ix < lines.size(); ix++) {
        trans.reset();
        trans.unveil(lines[ix].c_str(), buffer, sizeof buffer);
        lines[ix] = buffer;
      }
    }
    auto address = lines[0];
    auto subject = lines[2];
    auto text = lines[3];
    for (size_t ix = 4; ix < lines.size(); ix++) {
      text += "\n";
      text += lines[ix];
    }
    subject = "-s" + subject;
    const char *argv[] = { "/usr/bin/mail", subject.c_str(), address.c_str(),
        nullptr };
    Process process(&logger);
    std::string output;
    process.execute(argv, text.c_str(), nullptr, &output);
    logger.say(LV_DETAIL,
        formatCString("= sent: %s %s", address.c_str(), subject.c_str()));
  };
}
int mailer(ArgumentParser &parser, Logger &logger) {
  auto rc = 0;
  auto taskDirectory = parser.asString("tasks");
  auto stopFile = joinPath(taskDirectory, "stop!");
  auto wait = parser.asInt("interval", 60);
  do {
    auto files = listFiles(taskDirectory, "^[0-9.]+", false, true);
    for (auto file : files) {
      mailerSend(file.c_str(), logger);
    }
    sleep(wait);
  } while (!fileExists(stopFile.c_str()));
  unlink(stopFile.c_str());
  return rc;
}
int unveil(ArgumentParser &parser, Logger &logger) {
  auto rc = _veilOrUnveil(parser, logger, false);
  return rc;
}
int veil(ArgumentParser &parser, Logger &logger) {
  auto rc = _veilOrUnveil(parser, logger, true);
  return rc;
}
int unlicense(ArgumentParser &parser, Logger &logger) {
  auto rc = 0;
  auto filename = parser.asString("file");
//auto token = parser.asString("token");
  auto code = getenv("CPPKNIFE");
  auto code2 = formatCString("%.2f", 3.14);
  extern bool dbknifeUnderTest;
  if (!dbknifeUnderTest
      && (code == nullptr || strcmp(code, code2.c_str()) != 0)) {
    logger.say(LV_ERROR, "precondition failed");
  } else {
    PortableRandom rand0;
    CharRandom rand1(rand0, nullptr, CC_ASCII95);
    rand1.setSecrets(VeilEngine::_defaultSecret1, VeilEngine::_defaultSecret2,
        VeilEngine::_defaultSecret3);
    auto lines = readAsList(filename, &logger);
    std::string cmd;
    cmd.reserve(64000);
    cmd = "dbknife license ";
    char buffer[1024];
    for (auto line : lines) {
      rand1.reset();
      rand1.unveil(line.c_str(), buffer, sizeof buffer);
      if (buffer[0] == '#') {
        logger.say(LV_INFO, buffer);
        continue;
      }
      if (strncmp(buffer, "secret3=", 8) == 0
          || strncmp(buffer, "host=", 8) == 0) {
        cmd += formatCString("--%s \\\n", buffer);
      } else if (strchr(buffer, '=') != nullptr) {
        cmd += formatCString("'--additional-secret=%s' \\\n", buffer);
      }
      logger.say(LV_INFO, buffer);
    }
    logger.say(LV_INFO, cmd.c_str());
  }
  return rc;
}
int dbknife(int argc, char **argv, Logger *loggerExtern) {
  auto logger =
      loggerExtern == nullptr ?
          buildMemoryLogger(200, LV_SUMMARY) : loggerExtern;
  double start = nowAsDouble();
  int rc = 0;
  ArgumentParser parser("dbknife", logger, "Database management and more");
  parser.add("--log-level", "-l", DT_NAT,
      "Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG",
      "5");
  parser.add("--verbose", "-v", DT_BOOL, "Show more information");
  parser.add("--examples", nullptr, DT_BOOL, "Show usage examples", "false");
  parser.addMode("mode", "What should be done:",
      "install,uninstall,daemon,veil,unveil");
  ArgumentParser veilParser("veil", logger, "Encrypt a file or stdin.");
  parser.addSubParser("mode", "veil", veilParser);
  veilParser.add("--user", "-u", DT_STRING,
      "The password of that user will be asked for authentification.");
  veilParser.add("--code", "-c", DT_STRING,
      "The password for authentification. If not given the password will be asked.");
  veilParser.add("application", nullptr, DT_STRING,
      "The application to which the configuration file belongs.");
  veilParser.add("input", nullptr, DT_FILE,
      "The file to encrypt. If '~' stdin is used.", "~");
  veilParser.add("output", nullptr, DT_STRING,
      "The encrypted file. If '~' stdout is used. If '~' (empty) the input file is changed.",
      "");
  ArgumentParser unveilParser("unveil", logger, "Decrypt a file or stdin.");
  parser.addSubParser("mode", "unveil", unveilParser);
  unveilParser.add("--user", "-u", DT_STRING,
      "The password of that user will be asked for authentification.");
  unveilParser.add("--code", "-c", DT_STRING,
      "The password for authentification. If not given the password will be asked.");
  unveilParser.add("application", nullptr, DT_STRING,
      "The application to which the configuration file belongs.");
  unveilParser.add("input", nullptr, DT_FILE,
      "The file to decrypt. If '~' stdin is used.", "~");
  unveilParser.add("output", nullptr, DT_STRING,
      "The decrypted file. If '~' stdout is used. If '' (empty) the input file is changed.",
      "");

  ArgumentParser mailerParser("mailer", logger,
      "A daemon sending emails defined by files.");
  parser.addSubParser("mode", "mailer", mailerParser);
  mailerParser.add("--tasks", "-t", DT_DIRECTORY,
      "The directory containing the task files.", "/var/spool/cppknife/m");
  mailerParser.add("--interval", "-i", DT_NAT,
      "The waiting interval in seconds between test for tasks.", "60");

  ArgumentParser licenseParser("license", logger, "Create a license.");
  parser.addSubParser("mode", "license", licenseParser);
  licenseParser.add("--host", "-h", DT_STRING, "The licensed host.", "*");
  licenseParser.add("--secret3", "-3", DT_STRING, "The 3rd secret.", "*");
  licenseParser.add("--additional-secret", "-a", DT_STRING,
      "Additional secret. Format: <scope>.<name>=<secret> Scopes: application user",
      "", "user.jonny=NobodyKnows!", true);

  ArgumentParser unlicenseParser("unlicense", logger, "Show the license.");
  parser.addSubParser("mode", "unlicense", unlicenseParser);
  unlicenseParser.add("--token", "-t", DT_STRING, "The token.", "*");
  unlicenseParser.add("file", nullptr, DT_FILE, "The license file to show.");

  ArgumentParser listDbsParser("list-dbs", logger, "List all databases.");
  parser.addSubParser("mode", "list-dbs", listDbsParser);
  listDbsParser.add("--system", "-s", DT_BOOL, "Lists the system database too.",
      "false");
  listDbsParser.add("--driver", "-d", DT_STRING, "The DBMS system: mysql",
      "default");
  listDbsParser.add("--pattern", "-p", DT_STRING,
      "Only dbs matching that pattern will be listed", nullptr, "*person*");

  ArgumentParser createDbParser("create-db", logger,
      "Create a database and a user");
  parser.addSubParser("mode", "create-db", createDbParser);
  createDbParser.add("--driver", "-d", DT_STRING, "The DBMS system: mysql",
      "default");
  createDbParser.add("--user", "-u", DT_STRING,
      "Create a user. Format: <username>:<password>", nullptr,
      "jonny:TopSecret");
  createDbParser.add("--administrator", "-a", DT_BOOL,
      "Sets the user rights as administrator.", "false");
  createDbParser.add("--read-only", "-r", DT_BOOL,
      "Sets the user rights for read only access.", "false");
  createDbParser.add("database", nullptr, DT_STRING, "Name of the database.");

  ArgumentParser infoParser("info", logger, "Create a database and a user");
  parser.addSubParser("mode", "info", infoParser);
  infoParser.add("--driver", "-d", DT_STRING, "The DBMS system: mysql",
      "default");
  infoParser.add("--detail", "-t", DT_BOOL, "Show more information", "false");
  infoParser.add("what", nullptr, DT_STRING,
      "Defines what should be described. Format: <db>[.<table>", nullptr,
      "dbwarehouse|dbwarehouse.users");

  auto verbose = parser.asBool("verbose");
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
  } else if (parser.asBool("examples")) {
    examples();
  } else {
    auto level = static_cast<LogLevel>(parser.asInt("log-level", LV_SUMMARY));
    logger->setLevel(level);
    try {
      if (parser.isMode("mode", "daemon")) {
        rc = daemon(parser, *logger);
      } else if (parser.isMode("mode", "veil")) {
        rc = veil(parser, *logger);
      } else if (parser.isMode("mode", "unveil")) {
        rc = unveil(parser, *logger);
      } else if (parser.isMode("mode", "mailer")) {
        rc = mailer(parser, *logger);
      } else if (parser.isMode("mode", "license")) {
        rc = license(parser, *logger);
      } else if (parser.isMode("mode", "unlicense")) {
        rc = unlicense(parser, *logger);
      } else if (parser.isMode("mode", "list-dbs")) {
        rc = listDbs(parser, *logger);
      } else if (parser.isMode("mode", "create-db")) {
        rc = createDb(parser, *logger);
      } else if (parser.isMode("mode", "info")) {
        rc = info(parser, *logger);
      } else {
        printf("%s\n", parser.usage("unknown mode", nullptr, false).c_str());
      }
    } catch (ArgumentException &e) {
      logger->say(LV_ERROR, e.message());
    }
  }
  if (verbose) {
    logger->say(LV_SUMMARY,
        timeDifferenceToString(nowAsDouble() - start,
            "= runtime: %hh%mm%s.%3s"));
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}

} /* namespace */
