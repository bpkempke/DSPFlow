#include <flowGraph.h>
#include <stdio.h>
#include <string>

using namespace std;

int main(){
	//This main function isn't going to do a whole lot right now...
	//Right now it's just instantiating a flowGraph with a default XML input
	string xml_desc = 
		"<flowgraph>"
		"<block>" //This is the file input block
		"<id>file_input_block</id>"
		"<function>generic/file</function>"
		"<arg>file_name=input_file.dat</arg>"
		"<output prim_type=\"int8\">file_input_out</output>"
		"</block>"
		"<block>" //This is a constant block which specifies how much to add to each byte
		"<id>constant_block</id>"
		"<function>generic/constant</function>"
		"<arg>value=1</arg>"
		"<output prim_type=\"int8\">constant_out</output>"
		"</block>"
		"<block>" //This is the addition block
		"<id>add_block</id>"
		"<function>generic/add</function>"
		"<input>file_input_out</input>"
		"<input>constant_out</input>"
		"<output prim_type=\"int8\">add_out</output>"
		"</block>"
		"<block>" //This is the file output block
		"<id>file_output_block</id>"
		"<function>generic/file</function>"
		"<arg>file_name=output_file.dat</arg>"
		"<input>add_out</input>"
		"</block>"
		"</flowgraph>"
	;

	//Now create the flowgraph and run it....
	printf("Constructing the flowGraph...\n");
	flowGraph main_flowgraph(xml_desc);
	printf("Running the flowGraph...\n");
	main_flowgraph.run();
	usleep(1000000);
	printf("flowGraph complete...\n");
	main_flowgraph.stop();

	//What to do here? Wait for thread closure or will it block in main_flowgraph.run()?
	return 0;
}

