require_relative 'helper.rb'
class ConnectionMatrixEntry
=begin
	def initialize(connection)

		throw "IN_OUT flag not set" if connection.routing.in_out == nil

		# set 5 tuple
		if connection.routing.in_out
			@ip_in = connection.key.ip_src
			@port_in = connection.key.port_src
			@ip_out = connection.key.ip_dst
			@port_out = connection.key.port_dst
		else
			@ip_in = connection.key.ip_dst
			@port_in = connection.key.port_dst
			@ip_out = connection.key.ip_src
			@port_out = connection.key.port_src
		end
		@protocol = connection.key.protocol

		# first-last activity
		@first_s = 0;
		@last_s  = 0;

		# outgoing flows
		@outgoing_first_s = 0;
		@outgoing_last_s  = 0;

		# incoming flows
		@incoming_first_s = 0;
		@incoming_last_s  = 0;
	end

	def add(connection)
			@first_s = connection.body.start_s if @first_s == 0 or connection.body.start_s < @first_s
			@last_s  = connection.body.stop_s if connection.body.stop_s > @last_s

			if connection.routing.in_out
				@outgoing_first_s = connection.body.start_s if @outgoing_first_s == 0 or connection.body.start_s < @outgoing_first_s
				@outgoing_last_s  = connection.body.stop_s if connection.body.stop_s > @outgoing_last_s
			else
				@incoming_first_s = connection.body.start_s if @incoming_first_s == 0 or connection.body.start_s < @incoming_first_s
				@incoming_last_s  = connection.body.stop_s if connection.body.stop_s > @incoming_last_s
			end
	end
	def aging(time_s)
		# drop all information older than 'time_s'
		# return 'true' if this item can be deleted
	end
	def to_s
		s = sprintf('%10d %8d %10d %8d %3d', @ip_in, @port_in, @ip_out, @port_out, @protocol)
		s = sprintf(' | %10d %10d ', @first_s, @last_s)
	end
=end
end
