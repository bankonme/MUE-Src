// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 Bitcoin Developers
// Copyright (c) 2014-2015 MonetaryUnit Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef _MONETARYUNITRPC_CLIENT_H_
#define _MONETARYUNITRPC_CLIENT_H_ 1

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_writer_template.h"

int CommandLineRPC(int argc, char *argv[]);

json_spirit::Array RPCConvertValues(const std::string &strMethod, const std::vector<std::string> &strParams);

/** Show help message for monetaryunit-cli.
 * The mainProgram argument is used to determine whether to show this message as main program
 * (and include some common options) or as sub-header of another help message.
 *
 * @note the argument can be removed once monetaryunit-cli functionality is removed from monetaryunitd
 */
std::string HelpMessageCli(bool mainProgram);

#endif
