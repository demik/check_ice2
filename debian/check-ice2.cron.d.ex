#
# Regular cron jobs for the check-ice2 package
#
0 4	* * *	root	[ -x /usr/bin/check-ice2_maintenance ] && /usr/bin/check-ice2_maintenance
