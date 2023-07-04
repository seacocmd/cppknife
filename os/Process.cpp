/*
 * Process.cpp
 *
 *  Created on: 18.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"

namespace cppknife {

Process::Process(Logger *logger) :
    _returnCode(0), _logger(logger), _isInternalLogger(false) {
  if (logger == nullptr) {
    logger = buildMemoryLogger();
    _isInternalLogger = true;
  }
}
#define PIPE_READ 0
#define PIPE_WRITE 1
#define READ_END 0
#define WRITE_END 1

Process::~Process() {
  if (_isInternalLogger) {
    delete _logger;
    _logger = nullptr;
  }
}

void Process::readIntoFile(int fhInput, const char *fileOutput) {
  char buffer[0x10000];
  std::string rc;
  ssize_t bytes = 0;
  FILE *fpOut = fopen(fileOutput, "w");
  while ((bytes = read(fhInput, buffer, sizeof buffer - 1)) > 0) {
    //buffer[bytes] = '\0';
    //rc += buffer;
    if (fwrite(buffer, 1, bytes, fpOut) != static_cast<size_t>(bytes)) {
      auto errNo = errno;
      _logger->say(LV_ERROR,
          formatCString("cannot write to %s: (%d) %s", fileOutput, errNo,
              strerror(errNo)));
    }
    if (bytes < static_cast<ssize_t>(sizeof buffer) - 1) {
      break;
    }
  }
  if (bytes < 0) {
    auto errNo = errno;
    _logger->say(LV_ERROR,
        formatCString("cannot read: (%d) %s", errNo, strerror(errNo)));
  }
  writeText(fileOutput, rc.c_str(), rc.size(), _logger);
  fclose(fpOut);
}
std::string Process::readIntoString(int fhInput) {
  char buffer[0x10000];
  std::string rc;
  ssize_t bytes;
  while ((bytes = read(fhInput, buffer, sizeof buffer - 1)) > 0) {
    buffer[bytes] = '\0';
    rc += buffer;
    if (bytes < static_cast<ssize_t>(sizeof buffer) - 1) {
      break;
    }
  }
  if (bytes < 0) {
    auto errNo = errno;
    _logger->say(LV_ERROR,
        formatCString("cannot read: (%d) %s", errNo, strerror(errNo)));
  }
  return rc;

}

void Process::childProc(const char *argv[], int pipes[], const char *inputFile,
    const char *inputString) {
  int filein = 0;

  printf("childProc(): %s\n", argv[0]);
  printf("childProc(): dup2-1 %d %d\n", pipes[1], pipes[0]);
  if (dup2(pipes[1], STDOUT_FILENO) == -1) {
    _logger->say(LV_ERROR, "dup2 failed1");
  }
  if (inputFile != nullptr) {
    filein = open(inputFile, O_RDONLY, 0777);
    printf("childProc(): %d\n", filein);
    if (filein == -1) {
      _logger->say(LV_ERROR, "open filein in child failed");
    }
    printf("childProc(): dup2-2 %d\n", filein);
    if (dup2(filein, STDIN_FILENO) == -1) {
      _logger->say(LV_ERROR, "dup2 failed2");
    }
  }
  close(pipes[0]);
  close(pipes[1]);
  close(filein);
  printf("childProc(): start execv\n");
  execv(argv[0], (char* const*) &argv[1]);
}

void Process::parentProc(const char *argv[], int pipes[],
    const char *outputFile) {
  int fileout = -1;
  printf("parenProc: %s\n", outputFile == nullptr ? "null" : outputFile);
  if (outputFile != nullptr) {
    fileout = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fileout == -1) {
      _logger->say(LV_ERROR, "open fileout in parent failed");
    }
  }
  if (dup2(pipes[0], STDIN_FILENO) == -1) {
    _logger->say(LV_ERROR, "dup2 failed3");
  }
  if (outputFile != nullptr) {
    if (dup2(fileout, STDOUT_FILENO) == -1) {
      _logger->say(LV_ERROR, "dup2 failed4");
    }
  }
  close(pipes[1]);
  close(pipes[0]);
  if (fileout >= 0) {
    close(fileout);
  }
  printf("parenProc: stariing execv %s\n", argv[0]);
  execv(argv[0], (char* const*) &argv[1]);
}

int Process::executeDeprecated(const char *argv[], const char *inputString,
    const char *inputFile, std::string *outputString, const char *outputFile) {
  int pipes[2];
  int pid = 0;
  if (pipe(pipes) == -1) {
    _logger->say(LV_ERROR, "execute(): pipe failed");
  }
  pid = fork();
  if (pid == -1) {
    _logger->say(LV_ERROR, "execute(): fork failed");
  }
  if (pid == 0) {
    childProc(nullptr, pipes, inputFile,
        inputString == nullptr ? nullptr : inputString);
  } else {
    parentProc(argv, pipes, outputFile);
    close(pipes[0]);
    close(pipes[1]);
  }
  return (0);
}

int Process::execute(const char *argv[], const char *inputString,
    const char *inputFile, std::string *outputString, const char *outputFile) {
  int rc = 0;
  int pipes1[2];
  int pipes2[2];
  int pid = 0;
  if (pipe(pipes1) < 0) {
    auto errNo = errno;
    _logger->say(LV_ERROR,
        formatCString("execute(): cannot create pipe(): (%d) %s", errNo,
            strerror(errNo)));
  }
  if ((pid = fork()) < 0) {
    auto errNo = errno;
    _logger->say(LV_ERROR,
        formatCString("execute(): fork(): (%d) %s", errNo, strerror(errNo)));
  } else if (pid == 0) {
    //child , write to pipe
    //if (input != nullptr) {
    //  dup2(pipe_fd[PIPE_READ], STDIN_FILENO);
    //}
    /* Close the read end of the pipe in child process. We have a copy of it in file descriptor 0*/
    close(pipes1[PIPE_READ]);
    /* Close the write end of the pipe in child process.It is not used */
    close(pipes1[PIPE_WRITE]);
    if (execv(argv[0], (char* const*) &argv[1]) < 0) {
      auto errNo = errno;
      _logger->say(LV_ERROR,
          formatCString("execute(): execv() failed: %s (%d) %s", argv[0], errNo,
              strerror(errNo)));
      ;
    }
    // we never should arrive here:
    _logger->say(LV_ERROR, "execute(): behind execv");
    exit(6);
  } else /* pid > 0 */{
    // parent:
    // Write to child:
    if (inputString != nullptr) {
      write(STDIN_FILENO, inputString, strlen(inputString));
    }
    // Send EOF:
    //close(pipe_fd[PIPE_WRITE]);
    bool hasRead = false;
    if (outputFile != nullptr) {
      int fileout = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fileout == -1) {
        _logger->say(LV_ERROR, "open fileout in parent failed");
      }
      dup2(fileout, STDOUT_FILENO);
      hasRead = true;
    } else if (outputString != nullptr) {
      if (pipe(pipes2) < 0) {
        _logger->say(LV_ERROR, "pipe()-2 failed");
      } else {
        dup2(pipes2[0], STDOUT_FILENO);
        close(pipes2[0]);
        close(pipes2[1]);
      }
      hasRead = true;
    }
    if (outputString != nullptr) {
      *outputString = readIntoString(pipes2[0]);
    }

    if (close(pipes1[PIPE_READ]) != 0) {
      auto errNo = errno;
      _logger->say(LV_ERROR,
          formatCString(
              "execute(): cannot close pipe outputString file: (%d) %s", errNo,
              strerror(errNo)));
    }
    if (!hasRead && waitpid(pid, NULL, 0) < 0) {
      auto errNo = errno;
      _logger->say(LV_ERROR,
          formatCString("execute(): wait() failed: (%d) %s", errNo,
              strerror(errNo)));
    }
  }
  return rc;
}
int Process::executeAndPipe(const char **argv1, const char **argv2,
    const char *inputFile, const char *inputString, std::string *outputString,
    const char *outputFile) {
  int pipes[2];
  int pid = 0;

  if (pipe(pipes) == -1) {
    _logger->say(LV_ERROR, "pipe failed");
  }
  pid = fork();
  if (pid == -1) {
    _logger->say(LV_ERROR, "fork failed");
  }
  if (pid == 0) {
    childProc(argv2, pipes, inputFile, inputString);
  } else {
    parentProc(argv1, pipes);
    close(pipes[0]);
    close(pipes[1]);
  }
  return (0);
}

#define OUTPUT_END 0  // OUTPUT_END means where the pipe produces outputString
#define INPUT_END 1   // INPUT_END means where the pipe takes input
int Process::executeAndPipeDeprecated(const char **argv1, const char **argv2,
    const char *input, std::string *outputString, const char *outputFile) {
  int rc = 0;
  pid_t pid1 = 0;
  pid_t pid2 = 0;
  int fd1[2];
  int fd2[2];
// cat | grep | cut
// we now have 4 fds:
// fd1[0] = read end of cat->grep pipe (read by process 2)
// fd1[1] = write end of cat->grep pipe (written by process 1)
// fd2[0] = read end of grep->cut pipe (read by process 3)
// fd2[1] = write end of grep->cut pipe (written by process 2)
// PIPE_READ 0
// PIPE_WRITE 1
  pipe(fd1);
  pipe(fd2);
  pid1 = fork();
  if (pid1 == 0) {
    // child1
    // replace process1 stdout with write part of 1st pipe
    dup2(fd1[INPUT_END], STDOUT_FILENO);
    // Important: close all:
    close(fd1[INPUT_END]);
    close(fd1[OUTPUT_END]);
    close(fd2[INPUT_END]);
    close(fd2[OUTPUT_END]);
    execv(argv2[0], (char* const*) &argv2[1]);
  } else {
    // parent2:
    if (input != nullptr) {
      write(fd1[PIPE_WRITE], input, strlen(input));
    }
    // Send EOF:
    close(fd1[PIPE_WRITE]);

    pid2 = fork();
    if (pid2 == 0) {
      // child2:
      // I am going to be the sender process (i.e. producing output to the pipe)
      dup2(fd2[INPUT_END], STDOUT_FILENO); // [STDIN -> terminal_input, STDOUT -> pipe_input, fd[0] -> pipe_input] (of the ls process)
      dup2(fd2[OUTPUT_END], STDIN_FILENO); // [STOUT -> terminal_input, STDOUT -> pipe_input, fd[0] -> pipe_input] (of the ls process)
      close(fd2[OUTPUT_END]); // [STDIN -> terminal_input, STDOUT -> terminal_output, fd[0] -> pipe_input] (of the sender process)
      close(fd2[INPUT_END]); // [STDIN -> terminal_input, STDOUT -> pipe_input] (of the ls process)
      execv(argv1[0], (char* const*) &argv1[1]);
    } else {
      // parent2:
      if (outputFile != nullptr) {
        readIntoFile(STDOUT_FILENO, outputFile);
      } else if (outputString != nullptr) {
        *outputString = readIntoString(fd2[INPUT_END]);
      }
    }
    close(fd1[OUTPUT_END]);
    close(fd1[INPUT_END]);
    close(fd2[OUTPUT_END]);
    close(fd2[INPUT_END]);
    waitpid(-1, NULL, 0); // As the parent process - we wait for a process to die (-1) means I don't care which one - it could be either ls or wc
    waitpid(-1, NULL, 0); // As the parent process - we wait for the another process to die.
    // At this point we can safely assume both process are completed
  }
  return rc;
}
}/* cppknife */
