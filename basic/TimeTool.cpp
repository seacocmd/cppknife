/*
 * TimeTool.cpp
 *
 *  Created on: 06.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "basic.hpp"

namespace cppknife {
double nowAsDouble() {
  double rc = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count() / 1E6;
  return rc;
}

std::string formatDate(bool sortable, const char *separator) {
  auto rc = formatDate(std::time(nullptr), sortable, separator);
  return rc;
}
std::string formatDate(std::time_t time, bool sortable, const char *separator) {
  std::tm tm = *std::localtime(&time);
  char format[32];
  if (sortable) {
    snprintf(format, sizeof format, "%%Y%s%%m%s%%d", separator, separator);
  } else {
    snprintf(format, sizeof format, "%%d%s%%m%s%%Y", separator, separator);
  }
  char buffer[64];
  std::strftime(buffer, sizeof buffer, format, &tm);
  return buffer;
}
std::string formatDateTime(bool sortable, const char *separator,
    bool withSeconds, const char *separatorDate, const char *separatorTime) {
  auto rc = formatDateTime(std::time(nullptr), sortable, separator, withSeconds,
      separatorDate, separatorTime);
  return rc;
}
std::string formatDateTime(std::time_t time, bool sortable,
    const char *separator, bool withSeconds, const char *separatorDate,
    const char *separatorTime) {
  std::tm tm = *std::localtime(&time);
  char format[32];
  if (sortable) {
    if (withSeconds) {
      snprintf(format, sizeof format, "%%Y%s%%m%s%%d%s%%H%s%%M%s%%S",
          separatorDate, separatorDate, separator, separatorTime,
          separatorTime);
    } else {
      snprintf(format, sizeof format, "%%Y%s%%m%s%%d%s%%H%s%%M", separatorDate,
          separatorDate, separator, separatorTime);
    }
  } else {
    if (withSeconds) {
      snprintf(format, sizeof format, "%%d%s%%m%s%%Y%s%%H%s%%M%s%%S",
          separatorDate, separatorDate, separator, separatorTime,
          separatorTime);
    } else {
      snprintf(format, sizeof format, "%%d%s%%m%s%%Y%s%%H%s%%M", separatorDate,
          separatorDate, separator, separatorTime);
    }
  }
  char buffer[64];
  std::strftime(buffer, sizeof buffer, format, &tm);
  return buffer;
}

std::string formatTime(bool withSeconds, const char *separator) {
  auto rc = formatTime(std::time(nullptr), withSeconds, separator);
  return rc;
}
std::string formatTime(std::time_t time, bool withSeconds,
    const char *separator) {
  std::tm tm = *std::localtime(&time);
  char format[32];
  if (withSeconds) {
    snprintf(format, sizeof format, "%%H%s%%M%s%%S", separator, separator);
  } else {
    snprintf(format, sizeof format, "%%H%s%%M", separator);
  }
  char buffer[64];
  std::strftime(buffer, sizeof buffer, format, &tm);
  return buffer;
}

std::string timeDifferenceToString(double diffTime, const char *format) {
  std::string rc(format);
  int seconds = static_cast<int>(floor(diffTime));
  std::string value;
  bool useDays = strstr(format, "%d") != nullptr;
  bool useHours = strstr(format, "%h") != nullptr;
  bool useMinutes = strstr(format, "%m") != nullptr;
  if (useDays) {
    replaceString(rc, "%d", appendInt(value, seconds / 86400));
    value.clear();
  }
  if (useHours) {
    int hours = useDays ? seconds / 3600 % 24 : seconds / 3600;
    replaceString(rc, "%h", appendInt(value, hours, "%02d"));
    value.clear();
  }
  if (useMinutes) {
    int minutes = useHours ? seconds % 3600 / 60 : seconds / 60;
    replaceString(rc, "%m", appendInt(value, minutes, "%02d"));
    value.clear();
  }
  int seconds2 = useMinutes ? seconds % 60 : seconds;
  replaceString(rc, "%s", appendInt(value, seconds2, "%02d"));
  value.clear();
  if (strstr(format, "%3") != nullptr) {
    replaceString(rc, "%3",
        appendInt(value, static_cast<int>((diffTime - seconds) * 1E3), "%03d"));
    value.clear();
  }
  if (strstr(format, "%6") != nullptr) {
    replaceString(rc, "%6",
        appendInt(value, static_cast<int>((diffTime - seconds) * 1000000),
            "%06d"));
  }
  return rc;
}

}

