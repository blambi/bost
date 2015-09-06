## What is bost?
   A very simple "host" implementation by Avraham Lembke <blambi@chebab.com>
   It is not meant to be a full host replacement etc just translate hostname
   to a IP.
   
   One good think on how its implemented is that it follows all the
   normal libc lookup's etc for your system so you can use it to test
   NSS or other configuration that changes your DNS lookups.

## Where to get it or contact the author?
   you can get it at http://blambi.chebab.com/software/bost/
   And I'm contactable at blambi@chebab.com

## License?
   "This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version."

## Note on ranges
   By choice the ranges are limited to a maximum of /24

## Usage?

   For a single host

   `$ bost host`

   For an IP address:
   `$ bost 10.0.0.1`

   or
   
   `$ bost 2001:6b0:17:f0a0::b3`

   For N hosts
   
   `$ bost host1 ... hostN`

   For a subnet/range
   
   `$ bost 192.168.0.0/24`
  
   Another range
   
   `$ bost 192.168.0.0/32`

   Its simple..
   But there is currently no range support for IPv6 addresses.

    current options are:
      -h, --help     Show help
      -d, --double   enable double check
     
      for example "bost -d foo" would output both the ip(s) of foo
      and the hostname(s) for the ip(s).
     
## Thanks!  
   Just want to give big thanks to all people who have
   contacted me with bug reports and/or feedback.

   Erik Mathis
   Jeszenszky András

   And many more.

