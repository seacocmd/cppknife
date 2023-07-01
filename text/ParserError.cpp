/*
 * ParserError.cpp
 *
 *  Created on: 03.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {

ParserError::ParserError(const char *message, const Parser &parser) :
    _message(parser.prepareError(message)) {
}

ParserError::ParserError(const std::string &message, const Parser &parser) :
    _message(parser.prepareError(message.c_str())) {
}

ParserError::~ParserError() {
}

}
/* cppknife */
