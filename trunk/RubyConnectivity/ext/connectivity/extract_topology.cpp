/**
 * File: extract_topology.cpp
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "extract_topology.h"
Extract_Topology::Extract_Topology()
{
	id_next = 1;
	interfaces = new RouterInterface[EXTRACT_TOPOLOGY_ROUTER_INTERFACES_MAX];
	interface_id_aa = new int*[EXTRACT_TOPOLOGY_ROUTER_MAX];
	for(int i = 0; i < EXTRACT_TOPOLOGY_ROUTER_MAX; i++)
	{
		interface_id_aa[i] = new int[EXTRACT_TOPOLOGY_INTERFACE_MAX];
		for(int j = 0; j < EXTRACT_TOPOLOGY_INTERFACE_MAX; j++)
		{
			interface_id_aa[i][j] = 0;
		}
	}
	time_s = 0;
	stat_next_export_s = 0;
};
Extract_Topology::~Extract_Topology()
{
	delete [] interfaces;

	for(int i = 0; i < EXTRACT_TOPOLOGY_ROUTER_MAX; i++)
	{
		delete [] interface_id_aa[i];
	}
	delete [] interface_id_aa;
};
void Extract_Topology::process(Connections& cons, string& path)
{
	Connection* con; 
	int32_t iterf;
	for(int i = 0; i < cons.used; i++)
	{
		// cast
		con = cons.connections_a + i;

		// export					
		if (con->start_s > stat_next_export_s)
		{
			// update the time
			time_s = con->start_s;

			if(stat_next_export_s == 0)
				stat_next_export_s = (con->start_s/900)*900;
			else
			{
				stringstream file_ss;
				file_ss <<  path <<  "/" <<  stat_next_export_s << ".csv";
				ofstream out(file_ss.str().c_str());
				string buf;
				for(uint32_t i = 1; i < id_next; i++)
				{
					interfaces[i].to_s(buf);
					out << buf;
					interfaces[i].reset();
				}
				out.close();

				stat_next_export_s += 900;
			}
		}

		// we have only next hop ipv4 infos ...
		if(con->addr_length != 4)
			continue;

		// infrastructre information
		if(
			con->router > EXTRACT_TOPOLOGY_ROUTER_MAX or
			con->if_in > EXTRACT_TOPOLOGY_INTERFACE_MAX or
			con->if_out > EXTRACT_TOPOLOGY_INTERFACE_MAX
		)
		{
			cout << "Extract_Topology::process OUT OF BOUND INTERFACE:: ";
			cout << (int) con->router << ", ";
			cout << (int) con->if_in << ", ";
			cout << (int) con->if_out << ", ";
			string buf;
			Connection::to_s(buf, con->addr_next, 4);
			cout << buf << ", ";
			cout << endl;
			continue;
		}
		// in interface
		iterf = interface_id_aa[con->router][con->if_in];
		if(iterf == 0)
		{
			interface_id_aa[con->router][con->if_in] = id_next;
			iterf = id_next;
			id_next ++;
			interfaces[iterf].router = con->router;
			interfaces[iterf].iterf = con->if_in;

			cout << "ADD: " ;
			cout << (int) con->router << ", ";
			cout << (int) con->if_in << ", ";
			cout << endl;
			if(id_next >= EXTRACT_TOPOLOGY_ROUTER_INTERFACES_MAX)
			{
				cout << "Extract_Topology::process OUT OF BOUND ROUTER_INTERFACES_MAX";
				exit(-1);
			}
		}
		interfaces[iterf].add_in();
		// out interface
		iterf = interface_id_aa[con->router][con->if_out];
		if(iterf == 0)
		{
			interface_id_aa[con->router][con->if_out] = id_next;
			iterf = id_next;
			id_next ++;
			interfaces[iterf].router = con->router;
			interfaces[iterf].iterf = con->if_out;

			cout << "ADD: " ;
			cout << (int) con->router << ", ";
			cout << (int) con->if_out << ", ";
			cout << endl;
			if(id_next >= EXTRACT_TOPOLOGY_ROUTER_INTERFACES_MAX)
			{
				cout << "Extract_Topology::process OUT OF BOUND ROUTER_INTERFACES_MAX";
				exit(-1);
			}
		}
		interfaces[iterf].add_out(con->addr_next);
	}
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_extract_topology_free(void *p)
{
	delete (Extract_Topology*) p;
	p = NULL;
};
VALUE rb_extract_topology_alloc(VALUE klass)
{
	Extract_Topology* p = NULL;
	VALUE obj;
	p = new Extract_Topology();
	if (p == NULL)
		rb_raise(rb_path2class("ExtractTopologyError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_extract_topology_free, p);
	return(obj);
};

VALUE rb_extract_topology_process(VALUE self, VALUE cons, VALUE path)
{
	RB_EXTRACT_TOPOLOGY_UNWRAP
	Connections* cons_;
	Data_Get_Struct(cons, Connections, cons_);
	string path_ps = RSTRING_PTR(path);
	exttopo->process(*cons_, path_ps);
	return(self);
}

