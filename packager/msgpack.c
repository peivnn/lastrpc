/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Feng Qiu  <peivnn@foxmail.com>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#if ENABLE_MSGPACK

#include "php.h"
#include "php_lastrpc.h"
#include "last/lastrpc_packager.h"

extern void php_msgpack_serialize(smart_str *buf, zval *pzval TSRMLS_DC);
extern void php_msgpack_unserialize(zval *return_val, char *str, size_t str_len TSRMLS_DC);

int php_lastrpc_packager_msgpack_encode(zval *pzval, smart_str *buf TSRMLS_DC)
{
    php_msgpack_serialize(buf, pzval TSRMLS_CC);
	return 1;
}

zval * php_lastrpc_packager_msgpack_decode(char *text, size_t text_len TSRMLS_DC)
{
    zval *return_val;
	MAKE_STD_ZVAL(return_val);
	ZVAL_NULL(return_val);
	php_msgpack_unserialize(return_val, text, text_len TSRMLS_CC);
	return return_val;
}

lastrpc_data_t lastrpc_packager_msgpack = {
	php_lastrpc_packager_msgpack_encode,
    php_lastrpc_packager_msgpack_decode
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
