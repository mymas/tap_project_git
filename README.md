# ebtables 

# ebtables commmand

* -t, --table

ebtables command has three tables(filter, nat, broute).

	* broute: "broute" is a interface which has function, bridge and router.

* -D, --delete
	* usage ex: ebtables -D FORWARD 1
In usage ex, delete an rule, rule num 1 in "FORWARD" table.

if you want to know one in detail, you should look ebtables -h.

* -L, --list

	* usage ex: ebtables -L --Ln

In usage ex, show all chain and all rule.

* -o, --out-interface (interface name)

In my case, output interface is tap device.

	* how to make tap device by "openvpn" command.

``` openvpn –mktun –dev tap –user (username) ```

	* how to be active tap device by "ip" command.

``` ip link set tap up ```


* -i, --in-interface (interface name)

* --log

log watcher write description of flame to syslog(/var/log/syslog).

default is log-leve= info , log-prefix="",no ip log, no arp log.


### what is ulog? 

* --ulog

* --ulog-prefix text

* --ulog-nlgroup group

[refarence document1](https://linuxjm.osdn.jp/html/LDP_man-pages/man7/netlink.7.html)

[refarence document2](http://ebtables.netfilter.org/examples/basic.html#ex_ulog)

## what is netlink?

* nl_group is a bit mask with every bit representing a netlink group number.

[refarence document for netlink](http://man7.org/linux/man-pages/man7/netlink.7.html)


