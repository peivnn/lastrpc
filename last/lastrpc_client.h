/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Feng Qui  <peivnn@foxmail.com>                               |
  +----------------------------------------------------------------------+
*/
#ifndef PHP_LASTRPC_CLIENT_H
#define PHP_LASTRPC_CLIENT_H

#define LASTRPC_PACKAGER_JSON     "json"
#define	LASTRPC_PACKAGER_MSGPACK  "msgpack"


#define LASTRPC_PROTOCOL_HTTP 1
#define LASTRPC_PROTOCOL_TCP 2

void lastrpc_client_init(TSRMLS_D);

#endif /* PHP_LASTRPC_CONFIG_H */
