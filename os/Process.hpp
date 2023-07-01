/*
 * Process.hpp
 *
 *  Created on: 18.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_PROCESS_HPP_
#define OS_PROCESS_HPP_
namespace cppknife {

class Process {
protected:
  int _returnCode;
  Logger *_logger;
  bool _isInternalLogger;
public:
  Process(Logger *logger = nullptr);
  virtual ~Process();
public:
  void childProc(const char *argv[], int pipes[], const char *inputFile =
      nullptr, const char *inputString = nullptr);
  void parentProc(const char *argv[], int pipes[], const char *outputFile =
      nullptr);
  int execute(const char *argv[], const char *inputString = nullptr,
      const char *inputFile = nullptr, std::string *outputString = nullptr,
      const char *outputFile = nullptr);
  int executeDeprecated(const char *argv[], const char *input = nullptr,
      const char *inputFile = nullptr, std::string *outputString = nullptr,
      const char *outputFile = nullptr);
  int executeAndPipe(const char *argv1[], const char *argv2[],
      const char *inputFile = nullptr, const char *inputString = nullptr,
      std::string *outputString = nullptr, const char *outputFile = nullptr);
  int executeAndPipeDeprecated(const char **argv1, const char **argv2,
      const char *input = nullptr, std::string *outputString = nullptr,
      const char *outputFile = nullptr);
  void readIntoFile(int fhInput, const char *fileOutput);
  std::string readIntoString(int fhInput);
};
} /* cppknife */
#endif /* OS_PROCESS_HPP_ */
