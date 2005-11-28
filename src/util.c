/** \file 
 */

#include <openpgpsdk/util.h>
#include <openpgpsdk/packet-parse.h>
#include <openpgpsdk/crypto.h>
#include <openpgpsdk/create.h>
#include <openpgpsdk/errors.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

/**
 * Searches the given map for the given type.
 * Returns a human-readable descriptive string if found,
 * returns NULL if not found
 *
 * It is the responsibility of the calling function to handle the
 * error case sensibly (i.e. don't just print out the return string.
 * 
 */
static char *str_from_map_or_null(int type, ops_map_t *map)
    {
    ops_map_t *row;

    for ( row=map; row->string != NULL; row++ )
	if (row->type == type)
	    return row->string;
    return NULL;
    }

/**
 * \ingroup Utils
 *
 * Searches the given map for the given type.
 * Returns a readable string if found, "Unknown" if not.
 */

char *ops_str_from_map(int type, ops_map_t *map)
    {
    char *str;
    str=str_from_map_or_null(type,map);
    if (str)
	return(str);
    else
	return("Unknown");
    }

void hexdump(const unsigned char *src,size_t length)
    {
    while(length--)
	printf("%02X",*src++);
    }

/**
 * \ingroup Utils
 *
 * Initialise OpenPGP:SDK. This <b>must</b> be called before any other
 * OpenPGP:SDK function is used.
 */

void ops_init(void)
    {
    ops_crypto_init();
    }

/**
 * \ingroup Utils
 *
 * Close down OpenPGP:SDK, release any resources under the control of
 * the library. No OpenPGP:SDK function other than ops_init() should
 * be called after this function.
 */

void ops_finish(void)
    {
    ops_crypto_finish();
    }

/**
 * \ingroup Parse
 *
 * ops_reader_fd() attempts to read up to "plength" bytes from the file 
 * descriptor in "parse_info" into the buffer starting at "dest" using the
 * rules contained in "flags"
 *
 * \param	dest	Pointer to previously allocated buffer
 * \param	plength Number of bytes to try to read
 * \param	flags	Rules about reading to use
 * \param	parse_info	Gets cast to ops_reader_fd_arg_t
 *
 * \return	OPS_R_EOF 	if no bytes were read
 * \return	OPS_R_PARTIAL_READ	if not enough bytes were read, and OPS_RETURN_LENGTH is set in "flags"
 * \return	OPS_R_EARLY_EOF	if not enough bytes were read, and OPS_RETURN_LENGTH was not set in "flags"
 * \return	OPS_R_OK	if expected length was read
 * \return 	OPS_R_ERROR	if cannot read
 *
 * OPS_R_EARLY_EOF and OPS_R_ERROR push errors on the stack
 *
 * \sa enum opt_reader_ret_t
 *
 * \todo change arg_ to typesafe? 
 */
ops_reader_ret_t ops_reader_fd(unsigned char *dest,unsigned *plength,
			       ops_reader_flags_t flags,
			       ops_parse_info_t *parse_info)
    {
    ops_reader_fd_arg_t *arg=parse_info->reader_arg;
    int n=read(arg->fd,dest,*plength);

    if(n == 0)
	return OPS_R_EOF;

    if(n == -1)
	{
	ops_system_error_1(&parse_info->errors,OPS_E_R_READ_FAILED,"read",
			   "file descriptor %d",arg->fd);
	return OPS_R_ERROR;
	}

    if((unsigned)n != *plength)
	{
	if(flags&OPS_RETURN_LENGTH)
	    {
	    *plength=n;
	    return OPS_R_PARTIAL_READ;
	    }
	else
	    {
	    ops_error_1(&parse_info->errors,OPS_E_R_EARLY_EOF,
			       "file descriptor %d",arg->fd);
	    return OPS_R_EARLY_EOF;
	    }
	}
#if 0
    printf("[read 0x%x: ",length);
    hexdump(dest,length);
    putchar(']');
#endif
    return OPS_R_OK;
    }

void *ops_mallocz(size_t n)
    {
    void *m=malloc(n);

    memset(m,'\0',n);

    return m;
    }
