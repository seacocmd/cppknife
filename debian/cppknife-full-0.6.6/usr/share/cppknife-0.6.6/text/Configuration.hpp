/*
 * Configuration.hpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_CONFIGURATION_HPP_
#define TEXT_CONFIGURATION_HPP_
/// Implements a reader for configuration in Java style. Each line contains a definition like <em>name = Jonny Walker</em>.
namespace cppknife {

/**
 * @brief Offers an exception for errors in configuration data.
 */
class ConfigurationException {
public:
  ConfigurationException(const char *message);
  ConfigurationException(const std::string &message);
public:
  const std::string& message() const {
    return _message;
  }
protected:
  std::string _message;
};
/**
 * @brief Offers access to a configuration file with Java format.
 *
 * This class is an interface and an implementation for memory based configuration at the same time.
 *
 * The format:
 * <ul><li>A list of lines.</li>
 * <li>A line can be an empty line, a comment or an variable definition.</li>
 * <li>A variable definition has the form: <em>&lt;name> = &lt;value></em></li>
 * </ul>
 */
class Configuration {
public:
  /**
   * Constructor.
   * @param logger The logging manager. If <em>nullptr</em> a memory logger will be used.
   */
  Configuration(Logger *logger = nullptr);
  virtual ~Configuration();
public:
  /**
   * Returns a boolean value of a variable.
   * @param name The variable name
   * @param defaultValue That value is returned if the variable does not exist or has not a boolean value.
   * @return <em>defaultValue</em> if the variable does not exist or has not a boolean value.
   *  Otherwise: the value of the variable <em>name</em>.
   */
  virtual int asBool(const char *name, int defaultValue = -1);
  /**
   * Returns a integer value of a variable.
   * @param name The variable name
   * @param defaultValue That value is returned if the variable does not exist or has not a int value.
   * @return <em>defaultValue</em> if the variable does not exist or has not a boolean value.
   *  Otherwise: the value of the variable <em>name</em>.
   */
  virtual int asInt(const char *name, int defaultValue = -1);
  /**
   * Returns a natural integer value (&gt;= 0) of a variable.
   * @param name The variable name
   * @param defaultValue That value is returned if the variable does not exist or has not a nat value.
   * @return <em>defaultValue</em> if the variable does not exist or has not a boolean value.
   *  Otherwise: the value of the variable <em>name</em>.
   */
  virtual size_t asNat(const char *name, size_t defaultValue =
      static_cast<size_t>(-1));
  /**
   * Returns a string of a variable.
   * @param name The variable name
   * @param defaultValue That value is returned if the variable does not exist or has not a boolean value.
   * @return <em>defaultValue</em> if the variable does not exist or has not a boolean value.
   *  Otherwise: the value of the variable <em>name</em>.
   */
  virtual const char* asString(const char *name, const char *defaultValue =
      nullptr);
  /**
   * Returns the names of all variables.
   */
  virtual std::vector<std::string> names(const char *included = nullptr,
      const char *excluded = nullptr) const;
  /**
   * Populates the map from a string containing variables definitions.
   * Note: the map is not cleared at first, the info is appended.
   * @param lines A string with lines separated by "\n" and the format "name = value".
   */
  void populate(const char *lines);
protected:
  std::map<std::string, std::string> _map;
  Logger *_logger;
  bool _internalLogger;
};

/**
 * @brief Implementation of <em>Configuration</em> for files.
 */
class SimpleConfiguration: public Configuration {
public:
  SimpleConfiguration(const char *filename = nullptr, Logger *logger = nullptr);
  virtual ~SimpleConfiguration();
public:
  bool readFromFile(const char *filename = nullptr);
protected:
  std::string _filename;
};
} /* namespace cppknife */

#endif /* TEXT_CONFIGURATION_HPP_ */
