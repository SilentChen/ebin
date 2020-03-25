/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ebin.h"

/* If you declare any globals in php_ebin.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ebin)
*/

/* True global resources - no need for thread safety here */
static int le_ebin;

/* {{{ ebin_functions[]
 *
 * Every user visible function must have an entry in ebin_functions[].
 */
const zend_function_entry ebin_functions[] = {
	PHP_FE(ebin_decode,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in ebin_functions[] */
};
/* }}} */

/* {{{ ebin_module_entry
 */
zend_module_entry ebin_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ebin",
	ebin_functions,
	PHP_MINIT(ebin),
	PHP_MSHUTDOWN(ebin),
	PHP_RINIT(ebin),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(ebin),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(ebin),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_EBIN_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EBIN
ZEND_GET_MODULE(ebin)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ebin.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ebin_globals, ebin_globals)
    STD_PHP_INI_ENTRY("ebin.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ebin_globals, ebin_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_ebin_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ebin_init_globals(zend_ebin_globals *ebin_globals)
{
	ebin_globals->global_value = 0;
	ebin_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ebin)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ebin)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ebin)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ebin)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ebin)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ebin support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

int _ebinary_decode( ei_x_buff *ebinary ,zval *htable){
    zval *z;
    int type;
    int size;
    char *buff;
    long len;
    long long_value;
    double double_value;
    int i;
    ALLOC_INIT_ZVAL(z);
    ei_get_type( ebinary->buff, &ebinary->index, & type, & size );
    switch( type ) {
        case ERL_ATOM_EXT:
            buff = emalloc( size + 1 );
            ei_decode_atom( ebinary->buff, &ebinary->index, buff );
            buff[ size ] = '\0';
            ZVAL_STRING(z, buff, 0);
            add_next_index_zval( htable, z);
            break;
        case ERL_STRING_EXT:
            buff = emalloc( size + 1 );
            ei_decode_string( ebinary->buff, &ebinary->index, buff );
            buff[ size ] = '\0';
            ZVAL_STRING(z, buff, 0);
            add_next_index_zval( htable, z);								
            break;
        case ERL_BINARY_EXT:
            buff = emalloc( size );
            ei_decode_binary( ebinary->buff, &ebinary->index, buff, & len );
            ZVAL_STRINGL(z, buff, size, 0);
            add_next_index_zval( htable, z);								
            break;
        case ERL_SMALL_BIG_EXT:
        case ERL_SMALL_INTEGER_EXT:		 
        case ERL_INTEGER_EXT:
            ei_decode_long( ebinary->buff, &ebinary->index, & long_value );
            ZVAL_LONG(z, long_value);
            add_next_index_zval( htable, z);								
            break;
        case ERL_FLOAT_EXT:
            ei_decode_double( ebinary->buff, &ebinary->index, & double_value );
            ZVAL_DOUBLE(z, double_value);
            add_next_index_zval( htable, z);								
            break;
        case ERL_SMALL_TUPLE_EXT:
        case ERL_LARGE_TUPLE_EXT:
            array_init( z );
            ei_decode_tuple_header( ebinary->buff, &ebinary->index, & size );
            for( i = 1; i <= size; i++ ) {
                if(_ebinary_decode(ebinary,z)!=SUCCESS) { 
                    return FAILURE; 
                }
            }
            add_next_index_zval( htable, z );
            break;
        case ERL_NIL_EXT:
        case ERL_LIST_EXT:
            array_init( z );
            ei_decode_list_header( ebinary->buff, &ebinary->index, & size );
            while( size > 0 ) {
                for( i = 1; i <= size; i++ ) {
                    if(_ebinary_decode(ebinary,z)!=SUCCESS) { 
                        return FAILURE; 
                    }
                }
                ei_decode_list_header( ebinary->buff, &ebinary->index, & size );
            }
            add_next_index_zval( htable, z );				 
            break;
        default:
            // php_error( E_ERROR, "Unsupported Data Type %d", type );
            return FAILURE;
    }
    return SUCCESS;
}

/**
 *@php's param: erlang's binary string
 *@php's return: array or bool false
 */
PHP_FUNCTION(ebin_decode)
{
	int v, ret, arg_len;
	zval *arg, *htable;
	ei_x_buff  *ebinary;

	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE){
		RETURN_FALSE;
	}

	// php_printf("ARG:%s\n", arg);
	ebinary =emalloc(sizeof(ei_x_buff));

	ebinary->buff   = arg;
	ebinary->buffsz = arg_len;
	ebinary->index	= 0;
	ei_decode_version(ebinary->buff, &ebinary->index, &v);

	ALLOC_INIT_ZVAL(htable);
	array_init(htable);

	ret = _ebinary_decode(ebinary ,htable);
	// ei_x_free(ebinary); there is no need to double free, cause zend will do it after using the emalloc

	if(ret==SUCCESS) {
		*return_value = *htable;		
	}else{
		RETURN_FALSE;
	}
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
