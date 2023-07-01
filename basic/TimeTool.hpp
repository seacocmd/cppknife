/*
 * TimeTool.h
 *
 *  Created on: 06.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef BASIC_TIMETOOL_HPP_
#define BASIC_TIMETOOL_HPP_

namespace cppknife {
/**
 * Returns the current date as a date string.
 * @param sortable <em>true</em>: the result is sortable: year first.
 * @param separator The separator between the year, month and day.
 * @return The formated date.
 */
std::string formatDate(bool sortable = true, const char *separator = ".");
/**
 * Returns the given timestamp as a date string.
 * @param time The timestamp (seconds since epoch) to convert, e.g. <em>std::time()</em>
 * @param sortable <em>true</em>: the result is sortable: year first.
 * @param separator The separator between the year, month and day.
 * @return The formated date.
 */
std::string formatDate(std::time_t time, bool sortable = true,
    const char *separator = ".");
/**
 * Returns the given timestamp as a date string.
 * @param time The timestamp (seconds since epoch) to convert, e.g. <em>std::time()</em>
 * @param sortable <em>true</em>: the result is sortable: year first.
 * @param separator The separator between the date and time.
 * @param withSeconds <em>true</em>: the time will have seconds.
 * @param separatorDate The separator used for the date.
 * @param separatorTime The separator used for the time.
 * @return The formated date.
 */
std::string formatDateTime(std::time_t time, bool sortable = true,
    const char *separator = " ", bool withSeconds = true,
    const char *separatorDate = ".", const char *separatorTime = ":");
/**
 * Returns the current time as a date/time string.
 * @param sortable <em>true</em>: the result is sortable: year first.
 * @param separator The separator between the date and time.
 * @param withSeconds <em>true</em>: the time will have seconds.
 * @param separatorDate The separator used for the date.
 * @param separatorTime The separator used for the time.
 * @return The formated date.
 */
std::string formatDateTime(bool sortable = true, const char *separator = " ",
    bool withSeconds = true, const char *separatorDate = ".",
    const char *separatorTime = ":");

/**
 * Returns the given timestamp as a time string.
 * @param withSeconds <em>true</em>: the result contains the seconds.
 * @param separator The separator between hours, minutes and seconds.
 * @return The formated time.
 */
std::string formatTime(bool withSeconds = true, const char *separator = ":");
/**
 * Returns the current time as a time string.
 * @param time The timestamp (seconds since epoch) to convert, e.g. <em>std::time()</em>
 * @param withSeconds <em>true</em>: the result contains the seconds.
 * @param separator The separator between hours, minutes and seconds.
 * @return The formated time.
 */
std::string formatTime(std::time_t time, bool withSeconds = true,
    const char *separator = ":");
/**
 * Returns the seconds (as double) since epoch.
 */
double nowAsDouble();
/**
 * Formats a time difference.
 * @param diffTime The time difference in seconds as double.
 * @param format The format with that placeholders: %d(ays), %h(ours), %m(inutes), %s(econds) %3 (milliseconds) %6 (microseconds).
 *    Example: "%d:%h:%m:%s.%3"
 */
std::string timeDifferenceToString(double diffTime, const char *format =
    "%d:%h:%m:%s.%3");
}
#endif /* BASIC_TIMETOOL_HPP_ */
