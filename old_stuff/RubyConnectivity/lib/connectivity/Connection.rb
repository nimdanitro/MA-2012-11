#------------------------------------------------------------------------------#
# Connection: A Connection object consits of a key and a body 
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class Connection

	def to_s
		return "invalid" if valid == false 
		key_s = sprintf('%16s::%8d -> %16s::%8d %3d ', addr_src_s, port_src , addr_dst_s, port_dst, protocol) 
		time_s = sprintf('%10d %10d',start_s, stop_s)
		routing_s = sprintf('%1d %2d %2d -> %16s', router, if_in, if_out, addr_next_s)
		network_s = sprintf('%1d %1d', direction, border)
		content_s = sprintf('P:%10d B:%10d', packets, bytes)
		return(key_s + ' | ' + time_s +  ' | ' + routing_s  + ' | ' + network_s + ' | ' + content_s)
	end

=begin
################################################################################
	Connection__Unpack_M_Connection = 'NNNNNNNnnnncc'
	def import_from_m_connection(bin)
		data = bin.unpack(Connection__Unpack_M_Connection)
		data_i = data.collect{|item| item.to_i}
		set_ip_src(data[0])
		set_ip_dst(data[1])
		set_ip_next(data[2])
		set_start_s(data[3])
		set_stop_s(data[4])
		set_packets(data[5])
		set_bytes(data[6])
		set_port_dst(data[7])
		set_port_src(data[8])
		set_if_in(data[9])
		set_if_out(data[10])
		set_protocol(data[11])
		set_router(data[12])
		set_direction(Connection::DirectionUnknown)
		set_border(Connection::BorderUnknown)
	end

################################################################################
	def to_unique_key
		if ip_src > ip_dst
			return(sprintf('%10d-%8d-%10d-%8d-%3d', ip_src, port_src, ip_dst, port_dst, protocol))
		elsif ip_src < ip_dst
			return(sprintf('%10d-%8d-%10d-%8d-%3d', ip_dst, port_dst, ip_src, port_src, protocol))
		else
			# ip_src == ip_dst 
			if port_src > port_dst
				return(sprintf('%10d-%8d-%10d-%8d-%3d', ip_src, port_src, ip_dst, port_dst, protocol))
			elsif port_src < port_dst
				return(sprintf('%10d-%8d-%10d-%8d-%3d', ip_dst, port_dst, ip_src, port_src, protocol))
			else
				# ip_dst == ip_src; port_src == port_dst ...
				return(sprintf('%10d-%8d-%10d-%8d-%3d', ip_src, port_src, ip_dst, port_dst, protocol))
			end
		end
		throw "WTF: This line should never be executed"
	end
=end

end
