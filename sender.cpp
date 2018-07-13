#include "sender.h"

void sender::transciever(void)
{
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cout << "Writing CS low" << endl;
	cs_lc.write(false);
	wait(clk.posedge_event());
	sc_lv<32> write_enable = "00000000000000000000000000000110";
	io.write(write_enable);
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(true);

	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(false);
	wait(clk.posedge_event());
	sc_lv<32> ins = "00000000000000000000000000010101";
	io.write(ins); 
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(true);

	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(false);
	ins = "00000000000000000000000000000101";
	io.write(ins);
	wait(clk.negedge_event());
	wait(SC_ZERO_TIME);
	ins = io.read();
	while (ins[0] != SC_LOGIC_0)
	{
		wait(clk.negedge_event());
		wait(SC_ZERO_TIME);
		ins = io.read();
	}
	cout << "Weights have been written at time " << sc_time_stamp()  << endl; 
	wait(5,SC_NS);
	cs_lc.write(true);

	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(false);
	wait(clk.posedge_event());
	ins = "00000000000000000000000000000011";
	io.write(ins);
	sc_lv<32> address = "00000000000000000000000000000000";
	wait(clk.posedge_event());
	io.write(address);
	for(int i=0;i<10;i++)
	{
		wait(clk.negedge_event());
		wait(SC_ZERO_TIME);
	
		sc_lv<32> weight = io.read();
		cout << "Weight read as in bit format " << weight << endl;
		sc_int<32> weight_sc_int = weight;
		long weight_int = weight_sc_int;
		float *weight_p = (float *)&weight_int;
		float weight_float = *weight_p;
		cout << "Weight " << i+1 << " read as " << weight_float << " at time " << sc_time_stamp() << endl;  
	}
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(true);

	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(false);
	wait(clk.posedge_event());
	ins = "00000000000000000000000000000110";
	io.write(ins);
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(true);
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(false);
	wait(clk.posedge_event());
	ins = "00000000000000000000000000010100";
	io.write(ins);
	f.open("pixels.txt");
	int num_images = 0;
	f >> num_images;
	for(int i=0;i<num_images;i++)
	{
		for(int j=0;j<784;j++)
		{
			float pix_float = 0.0;
			f >> pix_float;
			long *pix_pointer = (long *)&pix_float;
			sc_int<32> pix_sc_int = *pix_pointer;
			sc_lv<32> pix  = pix_sc_int;
			wait(clk.posedge_event());
			io.write(pix);
		}
	}
	wait(clk.posedge_event());
	wait(5,SC_NS);
	cs_lc.write(true);
}
