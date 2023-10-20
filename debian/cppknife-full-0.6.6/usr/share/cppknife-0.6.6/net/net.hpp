/*
 * net.hpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#ifndef NET_NET_HPP_
#define NET_NET_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#if defined __linux__
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/socket.h>
#endif
#ifndef BASIC_HPP_
#include "../basic/basic.hpp"
#endif
#include "../net/SocketServer.hpp"
#include "../net/Agents.hpp"
#include "../net/SocketClient.hpp"
#endif /* NET_NET_HPP_ */
