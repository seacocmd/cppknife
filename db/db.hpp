/*
 * db.hpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef DB_DB_HPP_
#define DB_DB_HPP_
#ifndef CORE_HPP_
#include "../core/core.hpp"
#include "../text/text.hpp"
#endif
#ifndef OS_HPP_
#include "../os/os.hpp"
#endif
#include "SqlDriver.hpp"
#if ! defined IGNORE_MYSQL
#include "MySql.hpp"
#endif
#if ! defined IGNORE_POSTGRES
#include "Postgres.hpp"
#endif
#endif /* DB_DB_HPP_ */
