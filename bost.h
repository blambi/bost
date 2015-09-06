#define VERSION "0.0.9"

/* global options */
int double_check;

/* functions */
void check_host( const char *host );
int is_ip( const char *host );
void check_ip( const char *ip );
void check_sub_ip( const char *ip );
void check_sub_host( const char *host );
void check_ip_range( const char *ip );
int is_ip_range( const char *iprange );
