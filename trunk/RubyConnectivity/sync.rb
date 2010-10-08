#!/usr/bin/ruby1.9
################################################################################
# Synchronisation of code between your workstation and the cluster             #
################################################################################
# Copyright 2010 by D.Schatzmann, all rights reserved
################################################################################
#
# ONE-WAY Synchronisation: Push local data to the server
#
# Attentions: this script creates problems if multiple hosts are used ...


def sync_folder(folder, local_base, remote_base, exclude)
	puts "Sync:: #{folder}"
	cmd = "ssh asdaniel@cluster 'mkdir -p #{remote_base}/#{folder}'"
	system(cmd)
	cmd = "rsync -avz #{exclude} #{local_base}/#{folder}/*  -e ssh asdaniel@cluster:/#{remote_base}/#{folder}";
	system(cmd)
end

def sync_files(file_regex, local_base, remote_base, exclude)
	folder =  File.dirname(file_regex)
	cmd = "ssh asdaniel@cluster 'mkdir -p #{remote_base}/#{folder}'"
	system(cmd)
	cmd = "rsync -avz #{exclude} #{local_base}/#{file_regex}  -e ssh asdaniel@cluster:/#{remote_base}/#{folder}";
	system(cmd)
end

################################################################################
# MAIN 
################################################################################

local_base  = "/home/asdaniel/debug_inter_net/trunk"
remote_base = "/home/asdaniel/RubyConnectivity"

exclude = " --exclude '*.o' --exclude '*.so' --exclude 'Makefile' "
exclude = "#{exclude} --exclude '*.a' --exclude '*.svn' --exclude '.*' "
exclude = "#{exclude} --exclude '*~' --exclude 'test*' "
sync_folder("RubyConnectivity", local_base, remote_base, exclude)
