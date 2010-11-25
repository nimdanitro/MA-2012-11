require 'fileutils'
require 'ipaddr'
require 'logger'

Log = Logger.new(STDOUT)
Log.level = Logger::DEBUG

# include the relevant things
require_relative '../../ext/connectivity/connectivity.so'

require_relative 'Connection'
require_relative 'Connections'

# Data Parser
require_relative 'DataParser'
require_relative 'NFDataParser'


# Extract Network Topology
require_relative 'ExtractTopology'

# Filter Bank
require_relative 'FilterIPv6'
require_relative 'FilterIPv4'
require_relative 'FilterBorder'
require_relative 'FilterInOut'
require_relative 'FilterPrefixBlacklist'

# Connection Matrix
require_relative 'ConnectionMatrixKey'
require_relative 'ConnectionMatrixEntry'
require_relative 'ConnectionMatrix'

# Analysers
require_relative 'AnalyserBGP'
require_relative 'Analyser'

# Process Monitor
require_relative 'ProcessMonitor'

# some general code
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


