#!/usr/bin/ruby1.9
require_relative '../lib/connectivity/helper.rb'
require_relative './connectivity_start.rb'
require 'daemons'

Daemons.run('connectivity_start.rb', )
