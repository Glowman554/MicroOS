# Setting a ip

You can use the `ip` command to show and edit the ip configuration. The `ip` command supports the following sub-commands:

- `address <ip>`: Set the `address` field of a nic
- `subnet <ip>`: Set the `subnet` field of a nic
- `gateway <ip>`: Set the `gateway` field of a nic
- `dns <ip>`: Set the `dns` field of a nic
- `show`: Show the current configuration of a nic

You can use the `-i <nic>` flag to set the target nic. The default target nic is nic0.  

Example configuration:

```
ip address 10.0.2.100
ip subnet 255.255.255.0
ip gateway 10.0.2.2
ip dns 8.8.8.8
ip show
```

# Using DHCP

You can use the `dhcp` command to automatically configure an ip address. You can use the `-i <nic>` flag to set the target nic. The default target nic is nic0. You can use the `-n <hostname>` to set the hostname. The default hostname is `MicroOS`

# Testing internet access

You can test the internet access using `date -n`. This will test the dns and ip settings.
