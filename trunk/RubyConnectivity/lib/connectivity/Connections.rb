#------------------------------------------------------------------------------#
# Connections: Bundle a number of 'Connection' Objects into a set 
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class Connections

	def initialize(capacity=10000)
		resize(capacity)
	end
	def get_first_unused()
		return(get_first_unused__)
	end
	def get_next_unused()
		return(get_next_unused__)
	end
	def resize(capacity)
			resize__(capacity)
	end
	def reset()
		reset__()
	end
	def used()
		return used__()
	end
	def capacity()
		return capacity__()
	end
	def each_used
		each_used__ do |con|
			yield(con)
		end
	end
	def each_all
		each_all__ do |con|
			yield(con)
		end
	end
	def to_s 
		"Connections: #{used} / #{capacity}"
	end
end




