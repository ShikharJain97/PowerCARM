# PowerCARM
A open-source tool to generate the POWERCARM model [as mentioned in the paper "Beyond the Roofline: Cache-Aware Power and Energy-Efficiency Modeling for Multi-Cores" by Alexandar illic etal.]  which eventually helps to determine total power roof of the underlying architecture. 
This tool is derived or is an extension of already available open-source tool Locality aware roofline tool by NicolasDenoyelle , which generates CARM for the underlying microarchitecture.
If this tool is helpful in any way then please site both NicolasDenoyelle and ShikharJain97.
Using this tool I have generated necessary data for multiple intel's micro-architecture: Broadwell,Rocketlake.
If you require this data for research purposes , can sync up via cs22mtech02002@iith.ac.in/shikharjainman@gmail.com
Currently i am changing this tool to support arm cortex A-72 { Raspberry-pi4 [LARM] } and A-53 { Nvidia Jetson , it has internal powermeter [Both LARM & POWERCARM]}.
