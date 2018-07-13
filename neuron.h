#ifndef neuron_H
#define neuron_H

#include <systemc>
#include <iostream>
using namespace std;
using namespace sc_core;
using namespace sc_dt;

template <class dtype,int precision,int NUM_PIXELS> 
SC_MODULE(neuron)
{
	sc_time time_neuron_reset;
	sc_lv<10*precision> weight;
	sc_lv<precision> pixel;

	sc_in<bool> en_p;
	sc_in<bool> reset_p;
	sc_in<bool> valid_p;
	sc_in<sc_lv<precision*10> > weight_p;
	sc_fifo_in<sc_lv<precision> > pixel_p;

	public:
		sc_lv<precision> activation[11];

	SC_HAS_PROCESS(neuron);
	neuron(sc_module_name name):
		sc_module(name),
		time_neuron_reset(100,SC_NS)
	{
		SC_THREAD(read_reset);
		SC_THREAD(add_update);
	}

	void read_reset(void);
	void add_update(void);
};

template <class dtype,int precision,int NUM_PIXELS> 
void neuron<dtype,precision,NUM_PIXELS>::read_reset(void)
{
	for(;;)
	{
		wait(reset_p->negedge_event());
		cout << "Starting to reset at " << sc_time_stamp() << endl;
		cout << "Reset bit set high at " << sc_time_stamp() << endl;
		activation[10][6] = SC_LOGIC_1;
		cout << "Reset bit is " << activation[10][6] << " at time " <<  sc_time_stamp() << endl; 
		for(int i=0;i<10;i++)
		{
			sc_int<precision> zero_sc_int = 0;
			sc_lv<precision> zero = zero_sc_int;
			activation[i] = zero;
			
		}
		activation[10].range(3,0) = "0000";
		wait(time_neuron_reset);
		activation[10][6] = SC_LOGIC_0;
		cout << "Reset complete at " << sc_time_stamp() << endl;
		wait(reset_p->posedge_event());
	}
}

template <class dtype,int precision,int NUM_PIXELS>
void neuron<dtype,precision,NUM_PIXELS>::add_update(void)
{
	for(;;)
	{
		wait(en_p->negedge_event());
		
		cout << "Starting the inference process in the neuron" << sc_time_stamp() << endl;
		activation[10][5] = SC_LOGIC_1;

		for(int i=0;i<NUM_PIXELS;i++)
		{
			cout  << "Waiting for valid to go high in the neuron at time " << sc_time_stamp() << endl;
			wait(valid_p->posedge_event());
			weight = weight_p->read();
			cout  << "Weight read by neuron at time " << sc_time_stamp() << endl;
			if (pixel_p->num_available())
			{
				pixel = pixel_p->read();
				cout << "Pixel value read by neuron as " << pixel << " at time " << sc_time_stamp() << endl;  
			}
			else
			{
				wait(pixel_p->data_written_event() | en_p->default_event());
				cout << "Waiting for pixel value to be written at time " << sc_time_stamp() << endl;
				if(en_p->event())
				{
					break;
				}
				pixel = pixel_p->read();
				cout << "Pixel value read by neuron from fifo as " << pixel  <<  " at time " << sc_time_stamp() << endl;
				
			}
			
			sc_int<precision> pixel_sc_int = pixel;
			long pixel_long = pixel_sc_int;
			dtype *pixel_pointer = (dtype *)&pixel_long;
			dtype pixel_dtype = *pixel_pointer;

			int num = 0;
			for(int j=10*precision-1;j>=precision-1;j-=precision)
			{
				sc_lv<precision> weight_j = weight(j,j-precision+1);
				sc_int<precision> weight_sc_int = weight_j;
				long weight_long = weight_sc_int;
				dtype *weight_pointer = (dtype *)&weight_long;
				dtype weight_dtype = *weight_pointer;

				sc_lv<precision> activate = activation[num];
				sc_int<precision> activate_sc_int = activate;
				long activate_long = activate_sc_int;
				dtype *activate_pointer =(dtype *)&activate_long;
				dtype activate_dtype = *activate_pointer;

				activate_dtype += weight_dtype*pixel_dtype;

				long *update_pointer = (long *)&activate_dtype;
				sc_int<precision> update_sc_int = *update_pointer;
				sc_lv<precision> update = update_sc_int;
				activation[num] = update;

				num++;
			}
		}

		sc_lv<precision> activate = activation[0];
		sc_int<precision> activate_sc_int = activate;
		long activate_long = activate_sc_int;
		dtype *activate_pointer =(dtype *)&activate_long;
		dtype activate_dtype = *activate_pointer;
		int pred_int = 0;
		dtype max_val = activate_dtype;
		for(int i=1;i<10;i++)
		{
			sc_lv<precision> activate = activation[i];
			sc_int<precision> activate_sc_int = activate;
			long activate_long = activate_sc_int;
			dtype *activate_pointer =(dtype *)&activate_long;
			dtype activate_dtype = *activate_pointer;

			if(activate_dtype>max_val)
			{
				max_val = activate_dtype;
				pred_int = i;
			}
		}
		cout << "Predicted class is " << pred_int << endl;
		sc_int<4> pred_sc_int = pred_int;
		sc_lv<4> pred = pred_sc_int;
		activation[10].range(3,0) = pred;
		activation[10][5] = SC_LOGIC_0;	
	}
}
#endif
