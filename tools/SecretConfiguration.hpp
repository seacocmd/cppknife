/*
 * SecretConfiguration.hpp
 *
 *  Created on: 16.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_SECRET_CONFIGURATION_HPP_
#define TEXT_SECRET_CONFIGURATION_HPP_

namespace cppknife {

class SecretConfiguration: public Configuration {
protected:
  std::string _filename;
  VeilEngine _engine;
public:
  /**
   * Constructor.
   * @param filename The name of the configuration file.
   * @param application The name of the application.
   * @param logger The logging manager. If <em>nullptr</em> a memory logger will be used.
   */
  SecretConfiguration(const char *filename, const char *application,
      Logger &logger);
  virtual ~SecretConfiguration();
public:
  void read(const char *filename);
  bool veil(const char *filename, const std::vector<std::string> &lines);
};
} /* namespace cppknife */

#endif /* TEXT_SECRET_CONFIGURATION_HPP_ */
