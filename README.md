# ebtables 

# ebtables commmand

* -t, --table

ebtables command has three tables(filter, nat, broute).

	* broute: "broute" is a interface which has function, bridge and router.

* -o, --out-interface (interface name)

In my case, output interface is tap device.

	* how to make tap device by "openvpn" command.

``` openvpn –mktun –dev tap –user (username) ```

	* how to be active tap device by "ip" command.

``` ip link set tap up ```


* -i, --in-interface (interface name)



### what is ulog? 

* --ulog

* --ulog-prefix text

* --ulog-nlgroup group

[https://linuxjm.osdn.jp/html/LDP_man-pages/man7/netlink.7.html](refarence document1)

[http://ebtables.netfilter.org/examples/basic.html#ex_ulog](refarence document2)

## what is netlink?

* nl_group is a bit mask with every bit representing a netlink group number.

[http://man7.org/linux/man-pages/man7/netlink.7.html](refarence document for netlink)


