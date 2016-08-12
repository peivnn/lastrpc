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
#ifndef PHP_LASTRPC_EXCEPTION_H
#define PHP_LASTRPC_EXCEPTION_H

/**
 * #include <zend_exceptions.h>
 * #include "lastrpc_exception.h"
 *
 * zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "Failed calling %s::jsonSerialize()", ce->name);
 */
extern zend_class_entry *lastrpc_exception_zce;

void lastrpc_exception_init(TSRMLS_D);

#endif /* PHP_LASTRPC_CONFIG_H */
