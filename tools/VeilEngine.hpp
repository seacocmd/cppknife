/*
 * VeilEngine.hpp
 *
 *  Created on: 17.06.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_VEILENGINE_HPP_
#define CORE_VEILENGINE_HPP_

namespace cppknife {

/**
 * @brief Defines an engine for light encryption/decryption.
 */
class VeilEngine: public CharRandom {
public:
  static const char *_defaultSecret1;
  static const char *_defaultSecret2;
  static const char *_defaultSecret3;
  static const char *_introConfiguration;
protected:
  Logger &_logger;
  std::string _configuration;
  std::string _application;
  std::map<std::string, std::string> _secrets;
public:
  VeilEngine(const char *application, Logger &logger,
      const char *configuration = nullptr);
  virtual ~VeilEngine();
public:
  static void buildConfiguration(const char *filename, const char *host,
      const char *applicationName, const char *applicationValue,
      const char *secret3, Logger &logger,
      const std::vector<std::string> *additionalSecrets = nullptr);
public:
  /**
   * Reads the configuration data from a file.
   */
  bool readConfiguration();
  /**
   * Tests a user / password combination.
   * @param name The user's name.
   * @param code The password.
   * @return true: the user / password is correct.
   */
  bool verify(const char *name, const char *code);
};

} /* namespace cppknife */

#endif /* CORE_VEILENGINE_HPP_ */
