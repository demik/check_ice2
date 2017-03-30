# check_ice2

It is a Nagios plugin to check if a ICE or Shoucast stream status. It started as a fork of [pozar/check_ice](https://github.com/pozar/check_ice) by Tim Pozar but ended with a full rewrite. designed as a drop-in replacement for check_ice, with some additionnal features :

* IPv6 support
* ICY name checks

More features in the future.

### Usage
Basic Usage:
~~~
> check_ice2 -H ducks.and.ponies.listen.bassjunkees.com -p 80 -m bjr_192.mp3   
Stream is up on bjr_192.mp3 (name: "Bassjunkees - Drum & Bass 24/7", type: audio/mpeg).
~~~
Help:
Use -h flag
~~~
check_ice2 -h
check_ice2 client version 1.0.2
check_ice2 [-H hostname] [-v] [-m mount]
	 -h --help	 this screen.
	 -H --host	 remote host.
	 -m --mount	 remote mount.
	 -p --port	 remote port.
	 -v --verbose	 verbose mode.
	 -V --version	 show version and exit.
	 -w --warning	 treat ICY issues as warnings.
~~~
