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
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_lastrpc.h"

#include "last/lastrpc_client.h"


/* If you declare any globals in php_lastrpc.h uncomment this:*/
ZEND_DECLARE_MODULE_GLOBALS(lastrpc);


/* True global resources - no need for thread safety here */
// static int le_lastrpc;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini*/

PHP_INI_BEGIN()
#if ENABLE_MSGPACK == 1
    STD_PHP_INI_ENTRY("lastrpc.packager", LASTRPC_PACKAGER_MSGPACK, PHP_INI_SYSTEM, OnUpdateString, packager, zend_lastrpc_globals, lastrpc_globals)
#else
    STD_PHP_INI_ENTRY("lastrpc.packager", LASTRPC_PACKAGER_JSON, PHP_INI_SYSTEM, OnUpdateString, packager, zend_lastrpc_globals, lastrpc_globals)
#endif
    //连接超时时间(毫秒)
    STD_PHP_INI_ENTRY("lastrpc.cntimeout", "1000", PHP_INI_ALL, OnUpdateLong, cntimeout, zend_lastrpc_globals, lastrpc_globals)
    //请求响应超时时间
    STD_PHP_INI_ENTRY("lastrpc.rwtimeout", "3000", PHP_INI_ALL, OnUpdateLong, rwtimeout, zend_lastrpc_globals, lastrpc_globals)
PHP_INI_END()

/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_lastrpc_compiled(string arg)
   Return a string to confirm that the module is compiled in */

/**
 * 支持的数据传输格式列表
 */
PHP_FUNCTION(lastrpc_packagers)
{
    if (zend_parse_parameters_none() == FAILURE)
    {
        return;
    }

    char *dataf = LASTRPC_G(packager);

    array_init(return_value);

    add_next_index_string(return_value,LASTRPC_PACKAGER_JSON,1);
    if (ENABLE_MSGPACK)
    {
        add_next_index_string(return_value,LASTRPC_PACKAGER_MSGPACK,1);
    }
}


/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/* {{{ php_lastrpc_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_lastrpc_init_globals(zend_lastrpc_globals *lastrpc_globals)
{
	lastrpc_globals->dataformat = "json";
	lastrpc_globals->cntimeout = 0;
    lastrpc_globals->rwtimeout = 0;
}*/

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */

PHP_MINIT_FUNCTION(lastrpc)
{
	/* If you have INI entries, uncomment these lines*/
	REGISTER_INI_ENTRIES();

    // 定义扩展版本号
    REGISTER_STRINGL_CONSTANT("LASTRPC\\VERSION", PHP_LASTRPC_VERSION, sizeof(PHP_LASTRPC_VERSION)-1, CONST_PERSISTENT | CONST_CS);

    REGISTER_STRINGL_CONSTANT("LASTRPC\\PACKAGER_JSON", LASTRPC_PACKAGER_JSON, sizeof(LASTRPC_PACKAGER_JSON)-1, CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("LASTRPC\\PACKAGER_MSGPACK",LASTRPC_PACKAGER_MSGPACK, sizeof(LASTRPC_PACKAGER_MSGPACK)-1, CONST_PERSISTENT | CONST_CS);

    // 定义PHP的HTTP传输协议
    REGISTER_LONG_CONSTANT("LASTRPC\\PROTOCOL_HTTP",LASTRPC_PROTOCOL_HTTP,CONST_PERSISTENT | CONST_CS);
    // 定义PHP的TCP传输协议
    REGISTER_LONG_CONSTANT("LASTRPC\\ROTOCOL_TCP",LASTRPC_PROTOCOL_TCP,CONST_PERSISTENT | CONST_CS);

    // 初始化client
    lastrpc_client_init(TSRMLS_C);

    // 初始化server
    lastrpc_server_init(TSRMLS_C);

    // 初始exception
    lastrpc_exception_init(TSRMLS_C);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(lastrpc)
{
	/* uncomment this line if you have INI entries*/
	UNREGISTER_INI_ENTRIES();


	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(lastrpc)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(lastrpc)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(lastrpc)
{
	php_info_print_table_start();
    // php_info_print_table_header(2, "lastrpc support", "enabled");
	php_info_print_table_row(2, "lastrpc support", "enabled");
    php_info_print_table_row(2, "version", PHP_LASTRPC_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini*/
	DISPLAY_INI_ENTRIES();

}
/* }}} */



/* {{{ lastrpc_functions[]
 *
 * Every user visible function must have an entry in lastrpc_functions[].
 */
const zend_function_entry lastrpc_functions[] = {
	PHP_FE(lastrpc_packagers, NULL)
    //设置此函数别名：两种方法
    //ZEND_FALIAS(lastrpc_protocol_list,lastrpc_dataformats, NULL)
    ZEND_NAMED_FE(lastrpc_get_packagers,ZEND_FN(lastrpc_packagers),NULL)
	PHP_FE_END	/* Must be the last line in lastrpc_functions[] */
};
/* }}} */

/* {{{ lastrpc_module_entry
 */
zend_module_entry lastrpc_module_entry = {
	STANDARD_MODULE_HEADER,
	"lastrpc",
	lastrpc_functions,
	PHP_MINIT(lastrpc),
	NULL, // PHP_MSHUTDOWN(lastrpc),
	NULL, // PHP_RINIT(lastrpc),		/* Replace with NULL if there's nothing to do at request start */
	NULL, // PHP_RSHUTDOWN(lastrpc),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(lastrpc),
	PHP_LASTRPC_VERSION,
    PHP_MODULE_GLOBALS(lastrpc),
    NULL, // PHP_GINIT(lastrpc)
    NULL,
    NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_LASTRPC
ZEND_GET_MODULE(lastrpc)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
