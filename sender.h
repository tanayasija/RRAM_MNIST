#ifndef sender_H
#define sender_H

#include <systemc>
#include <iostream>
#include <fstream>
#include "RRAM_MNIST.h"
using namespace std;
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(sender)
{
	RRAM_MNIST<float,32,784> n;
	
	sc_clock clk;	
	sc_signal<bool> cs_lc;
	sc_signal<sc_lv<32>, SC_MANY_WRITERS > io;

	ifstream f;
	
	sc_lv<32> high_impedance;
	SC_CTOR(sender):
		n("n",1,8),
		clk("clk",20,SC_NS,0.5,10,SC_NS,true),
		high_impedance("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ")
	{
		cout << "Initializing RRAM channels" << endl;	
	
		n.clk_p(clk);
		n.cs_p(cs_lc);
		n.io_p(io);

		cs_lc.write(true);
		io.write(high_impedance);
		
		SC_THREAD(transciever);	

		cout << "Initialized sender block" << endl;
	}

	void transciever(void);
};
#endif
