/*
  bost is a DNS resolving program, similar to the 'host' program(s).
  Copyright (C) 2006-2015  Avraham Lembke
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "bost.h"

int double_check;
int ip_range_check;

int main( int argc, char **argv )
{
    int args;
    int opt_index;
    int choice;

    static struct option long_options[] =
    {
        { "help", no_argument, 0, 'h' },
        { "double", no_argument, 0, 'd' },
        { 0, 0, 0, 0 }
    };

    /* defaults */
    double_check = false;
    ip_range_check = false;

    printf( "bost v%s\n", VERSION );
    printf( "Copyright (C) 2006-2015  Avraham Lembke.\n" );
    printf( "This is free software with ABSOLUTELY NO WARRANTY.\n\n" );

    if( argc -1 == 0 )
    {
        /* Abort if no hostnames was supplied */
        printf( "Error: no hostname[s] supplied...\n" );
        printf( "Try `bost -h' for more information\n" );
        exit( EXIT_FAILURE );
    }
    
    /* Argument parser */
    while( 1 )
    {
        opt_index = 0;
        choice = getopt_long( argc, argv, "hd", long_options, &opt_index );
        
        if( choice == -1 ) /* no arguments left */
            break;
        
        switch( choice )
        {
        case 'h':
            printf( "Usage: [OPTIONS] HOST/IP ...\n\n" );
            printf( "Option\t\tMeaning\n" );
            printf( " -h, --help\tShow this message\n" );
            printf( " -d, --double\tEnable double checking\n" );
            printf( "\n" );
            exit( EXIT_SUCCESS );
            break;
        case 'd':
#ifdef DEBUG
            printf( "double_check enabled\n" );
#endif
            double_check = true;
            break;
        default:
            break;
        }   
    }

    /* hosts and ip's */
    for( args = 1; args < argc; args++ )
    {
        if( !( argv[args][0] == '-' ))
        {
            if( is_ip( argv[args] ))
                check_ip( argv[args] );
            else if( is_ip_range( argv[args] ))
            {
                ip_range_check = true;
                check_ip_range( argv[args] );
                ip_range_check = false;
            }
            else
            {
                check_host( argv[args] );
            }
            printf( "\n" );
        }
    }
    
    exit( EXIT_SUCCESS );
}

void check_host( const char *host )
{
    struct addrinfo hints, *hostinfo, *iter;
    int hosts;
    char addr[50]; /* big enought for both IPv6: 46, IPv4: 16 */

    hosts = 0;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC; /* So we can use this function for
                                    both types */
    hints.ai_socktype = SOCK_STREAM; /* Not sure why */
    
    if( !ip_range_check )
        printf( "%s:\n", host );

    if( getaddrinfo( host, NULL, &hints, &hostinfo ) != 0 )
    {
        if( !ip_range_check )
            printf( "\tLookup failed\n" );
    }
    else
    {
        for( iter = hostinfo; iter != NULL; iter = iter->ai_next )
        {
            switch( iter->ai_family )
            {
            case AF_INET: /* IPv4 */
                inet_ntop( AF_INET,
                           &(( struct sockaddr_in *) iter->ai_addr)->sin_addr,
                           addr, INET_ADDRSTRLEN );
                break;
            case AF_INET6: /* IPv6 */
                inet_ntop( AF_INET6,
                           &(( struct sockaddr_in6 *) iter->ai_addr)->sin6_addr,
                           addr, INET6_ADDRSTRLEN );
                break;
            default: /* Unknown */
                strncpy( addr, "unknown network family\0", 50 );
                break;
            }

            printf( "\t%d: %s\n", hosts, addr );

            if( double_check )
            {
                if( strncmp( "unknown", addr, 7 ) != 0 )
                    check_sub_ip( addr );
            }

            hosts++;
        }
        
        freeaddrinfo( hostinfo ); /* don't be a memory glutton */
    }

    if( ip_range_check )
        printf( "\n" ); 
}

int is_ip( const char *ip )
{
    struct in6_addr test_ip;
  
    if( inet_pton( AF_INET, ip, &test_ip ) != false )
        return true; /* Its a IP */
    else if( inet_pton( AF_INET6, ip, &test_ip ) != false )
        return true; /* Its a IP */
    else
        return false; /* Its not */
}

int ip_type( const char *ip )
{
    int where;
    
    where = 0;

    while( ip[where] != '\0' )
    {
        if( ip[where] == '.' )
            return AF_INET;
        else if( ip[where] == ':' )
            return AF_INET6;
        
        where++;
    }

    return AF_UNSPEC;
}

void check_ip( const char *ip )
{
    struct hostent *hostinfo;
    struct in_addr in_ipv4;
    struct in6_addr in_ipv6;

    int hosts; /* host counter */
    int type;

    /* Find out type */
    type = ip_type( ip );

    switch( type )
    {
    case AF_INET:
        inet_pton( AF_INET, ip, &in_ipv4 );
        hostinfo = gethostbyaddr( &in_ipv4, sizeof( in_ipv4 ), AF_INET );
        break;
    case AF_INET6:
        inet_pton( AF_INET6, ip, &in_ipv6 );
        hostinfo = gethostbyaddr( &in_ipv6, sizeof( in_ipv6 ), AF_INET6 );
        break;
    default:
        printf( "Error: %s is not a recogniced IP type\n", ip );
        exit( 1 );
        break;
    }

    hosts = 0;
  
    if( !ip_range_check )
        printf( "%s:\n", ip );

#ifdef DEBUG
    printf( "sizeof( ip ) = %d\n", sizeof( ip ));
    printf( "h_errno = %d\n", h_errno );
#endif

    if( hostinfo == NULL )
    {
        if( !ip_range_check )
            printf( "\tLookup failed\n" );
    }
    else
    {
        if( ip_range_check )
            printf( "%s:\n", ip );
        printf( "\t%d: %s\n", hosts, hostinfo -> h_name );
        if( double_check )
            check_sub_host( hostinfo -> h_name );
        
        while( hostinfo -> h_aliases[hosts] != NULL )
        {
            printf( "\t%d: %s\n", hosts +1, hostinfo -> h_aliases[hosts] );
            if( double_check )
                check_sub_host( hostinfo -> h_aliases[hosts] );
            hosts++;
        }
        if( ip_range_check )
            printf( "\n" );
    }
}

void check_sub_ip( const char *ip )
{
    /* Used to check a sub ip.
       (used if -d is specfied) */

    struct hostent *hostinfo;
    struct in_addr in_ipv4;
    struct in6_addr in_ipv6;

    int hosts; /* host counter */
    int type;

    /* Find out type */
    type = ip_type( ip );

    switch( type )
    {
    case AF_INET:
        inet_pton( AF_INET, ip, &in_ipv4 );
        hostinfo = gethostbyaddr( &in_ipv4, sizeof( in_ipv4 ), AF_INET );
        break;
    case AF_INET6:
        inet_pton( AF_INET6, ip, &in_ipv6 );
        hostinfo = gethostbyaddr( &in_ipv6, sizeof( in_ipv6 ), AF_INET6 );
        break;
    default:
        printf( "Error: %s is not a recogniced IP type\n", ip );
        exit( 1 );
        break;
    }

    hosts = 0;

    if( hostinfo == NULL )
    {
        if( !ip_range_check )
            printf( "\t\tLookup failed\n" );
    }
    else
    {
        printf( "\t\t%d: %s\n", hosts, hostinfo -> h_name );
        
        while( hostinfo -> h_aliases[hosts] != NULL )
        {
            printf( "\t\t%d: %s\n", hosts +1, hostinfo -> h_aliases[hosts] );
            hosts++;
        }
    }
}

void check_sub_host( const char *host )
{
    /* Used similarly to check_sub_ip */
        struct addrinfo hints, *hostinfo, *iter;
    int hosts;
    char addr[50]; /* big enought for both IPv6: 46, IPv4: 16 */

    hosts = 0;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC; /* So we can use this function for
                                    both types */
    hints.ai_socktype = SOCK_STREAM; /* Not sure why */

    if( getaddrinfo( host, NULL, &hints, &hostinfo ) != 0 )
    {
        if( !ip_range_check )
            printf( "\t\tLookup failed\n" );
    }
    else
    {
        for( iter = hostinfo; iter != NULL; iter = iter->ai_next )
        {
            switch( iter->ai_family )
            {
            case AF_INET: /* IPv4 */
                inet_ntop( AF_INET,
                           &(( struct sockaddr_in *) iter->ai_addr)->sin_addr,
                           addr, INET_ADDRSTRLEN );
                break;
            case AF_INET6: /* IPv6 */
                inet_ntop( AF_INET6,
                           &(( struct sockaddr_in6 *) iter->ai_addr)->sin6_addr,
                           addr, INET6_ADDRSTRLEN );
                break;
            default: /* Unknown */
                strncpy( addr, "unknown network family\0", 50 );
                break;
            }

            printf( "\t\t%d: %s\n", hosts, addr );
            hosts++;
        }
    }

    freeaddrinfo( hostinfo ); /* don't be a memory glutton */

    if( ip_range_check )
        printf( "\n" ); 
}

void check_ip_range( const char *iprange ) /* NO plans for IPv6 support here */
{
    /*
      Checks each IP in a range
      Only supports scanning one C net.
      (MAX /24 ~= 256 hosts)
    */
    char ip[16];
    char buf_mask[4];
    char buff[16];
    int slash_found, x, y, length, mask, hosts, start;

    slash_found = false;
    x = y = 0;

    while( iprange[x] != '\0' )
    {
        if( iprange[x] == '/' )
        {
            ip[x] = '\0';
            slash_found = true;
        }
        else
        {
            if( slash_found )
            {
                buf_mask[y] = iprange[x];
                y++;
            }
            else
                ip[x] = iprange[x];
        }
        x++;
    }
    buf_mask[y] = '\0';

    mask = atoi( buf_mask ); /* FIXME: libc says im obsolete */

    y = 32 - mask; /* get bits */
    hosts = (int)pow( (double)2, (double)y );

    /* get start ip */
    length = strlen( ip );
    
    for( x = length; x >= 0; x-- )
        if( ip[x] == '.' )
            break;
    
    for( y = 0; y < length; y++ )
        buff[y] = ip[y + ( x +1 )];
    buff[y] = '\0';   
    ip[x +1] = '\0'; /* remove start from ip */

    start = atoi( buff );
    
#ifdef DEBUG    
    printf( "ip: '%s', mask: '%d'\n", ip, mask );
    printf( "start: '%d'\n", start );
    printf( "hosts: %d\n", hosts );
    printf( "len: %d\n", length );
#endif

    if( mask < 24 )
    {
        printf( "I'm sorry I will not scan %d hosts, ", hosts );
        printf( "do it with a script.. :/\n" );
    }
    else
    {
        for( x = start; x < hosts + start; x++ )
        {
            if( x > 255 ) /* bugfix: so /24 doesn't try to scan 256... */
                break;

            sprintf( buff, "%s%d", ip, x );
            check_ip( buff );
        }
    }
}

int is_ip_range( const char *iprange )
{
    /* Checks if ip-range is a range
       Currently no plans for IPv6 support here */
    int x, c;
    x = 0;

#ifdef DEBUG
    printf( "testing if '%s' is a ip range\n", iprange );
#endif

    while( iprange[x] != '\0' )
    {
        c = iprange[x];
        if( !( isdigit( c ) || c == '.' || c == '/' ))
            return false;
        x++;
    }
    return true;
}
