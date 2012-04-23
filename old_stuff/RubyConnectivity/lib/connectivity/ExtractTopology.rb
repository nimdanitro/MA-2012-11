#------------------------------------------------------------------------------#
# The Extract Topology trys to extract the topology of the network
# This data is then used for the 'FilterBorder'
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class ExtractTopology

	def initialize(
		output,
		stat_interval_s = 300
	)
		# result path
		@result_p = output
		FileUtils::mkdir_p(@result_p)

		# layout
		@layout_p = "#{@result_p}/layout"
		FileUtils::mkdir_p(@layout_p)
	end
################################################################################
### process ####################################################################
################################################################################
	def process(cons)
		process__(cons, @layout_p)
	end
################################################################################
### debug  #####################################################################
################################################################################
	def to_s
		s =  "ExtractTopology (#{Version}) \n"
		s += "Layout Folder: #{@layout_p} \n"
	end
end


