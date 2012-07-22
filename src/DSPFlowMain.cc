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
		"<output>out</output>"
		"</block>"
		"<block>" //This is the addition block
		"<id>add_block</id>"
		"<function>generic/add</function>"
		"<input>cool_block.out</input>"
		"<output>out</output>"
		"</block>"
		"<block>" //This is the file output block
		"<id>file_output-bock</id>"
		"<function>generic/file</function>"
		"<input>add_block.out</input>"
		"</block>"
		"</flowgraph>"
	;

	//Now create the flowgraph and run it....
	printf("Constructing the flowGraph...\n");
	flowGraph main_flowgraph(xml_desc);
	printf("Running the flowGraph...\n");
	main_flowgraph.run();
	printf("flowGraph complete...\n");

	//What to do here? Wait for thread closure or will it block in main_flowgraph.run()?
	return 0;
}

