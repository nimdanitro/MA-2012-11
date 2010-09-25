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
	cmd = "ssh schadomi@cluster 'mkdir -p #{remote_base}/#{folder}'"
	system(cmd)
	cmd = "rsync -avz #{exclude} #{local_base}/#{folder}/*  -e ssh schadomi@cluster:/#{remote_base}/#{folder}";
	system(cmd)
end

def sync_files(file_regex, local_base, remote_base, exclude)
	folder =  File.dirname(file_regex)
	cmd = "ssh schadomi@cluster 'mkdir -p #{remote_base}/#{folder}'"
	system(cmd)
	cmd = "rsync -avz #{exclude} #{local_base}/#{file_regex}  -e ssh schadomi@cluster:/#{remote_base}/#{folder}";
	system(cmd)
end

################################################################################
# MAIN 
################################################################################

local_base  = "/home/schadomi/eth/svn_sa_debug_internet/trunk/RubyConnectivity"
remote_base = "/home/schadomi/programming/RubyConnectivity"

exclude = " --exclude '*.o' --exclude '*.so' --exclude 'Makefile' "
exclude = "#{exclude} --exclude '*.a' --exclude '*.svn' --exclude '.*' "
exclude = "#{exclude} --exclude '*~' --exclude 'test*' "
sync_folder("RubyConnectivity2", local_base, remote_base, exclude_)
