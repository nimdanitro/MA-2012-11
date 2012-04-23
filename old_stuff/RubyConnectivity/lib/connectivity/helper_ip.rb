IPV4_Reg = Regexp.new('^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})')
def get_ipv4_n(ip_s)
	if m = IPV4_Reg.match(ip_s)
		return(m[1].to_i << 24 | m[2].to_i << 16 | m[3].to_i << 8 | m[4].to_i)
	else
		raise "'#{ip_s}'is not a valide IPv4 string"
	end
end
def get_ipv4_s(ip_n)
	return(IPAddr.new(ip_n, Socket::AF_INET).to_s)
end
