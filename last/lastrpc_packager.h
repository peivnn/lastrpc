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
#ifndef PHP_LASTRPC_PACKAGER_H
#define PHP_LASTRPC_PACKAGER_H

#include "ext/standard/php_smart_str.h"

typedef struct _lastrpc_data
{
    int (*encode) (zval *pzval, smart_str *buf TSRMLS_DC);
    zval * (*decode) (char *text, size_t text_len TSRMLS_DC);
} lastrpc_data_t;

lastrpc_data_t * php_lastrpc_packager_get(char *packager TSRMLS_DC);

extern lastrpc_data_t lastrpc_packager_json;

#if ENABLE_MSGPACK
extern lastrpc_data_t lastrpc_packager_msgpack;
#endif

#endif /* PHP_LASTRPC_CONFIG_H */
