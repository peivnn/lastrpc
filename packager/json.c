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

#include "php.h"
#include "php_lastrpc.h"
#include "ext/json/php_json.h"
#include "last/lastrpc_packager.h"


int php_lastrpc_packager_json_encode(zval *pzval, smart_str *buf TSRMLS_DC)
{
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3))
	php_json_encode(buf, pzval TSRMLS_CC);
#else
	php_json_encode(buf, pzval, 0 TSRMLS_CC);
#endif

    return 1;
}

zval * php_lastrpc_packager_json_decode(char *text, size_t text_len TSRMLS_DC)
{
	zval *return_value;
	MAKE_STD_ZVAL(return_value);

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3))
	php_json_decode(return_value, text, text_len, 1 TSRMLS_CC);
#else
	php_json_decode(return_value, text, text_len, 1, 512 TSRMLS_CC);
#endif
	return return_value;
}

lastrpc_data_t lastrpc_packager_json = {
	php_lastrpc_packager_json_encode,
    php_lastrpc_packager_json_decode
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
