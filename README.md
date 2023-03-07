Converts the JSON 1-line string to TLV records and dictionaries.
For example given JSON:

{"key1":true, "key2":234,  "key3":"sdfdgv"}
{"key4":false,"key5":45634,"key6":"sdfgxb"}
{"key7":true, "key8":-55,  "key9":"sdfgsdg"}

Convertion will handle each line and produce the binary files: record_0, record_1, record_2 and
dict_0, dict_1, dict_2. In a human view records and dicts will looking like (for 1st JSON line)

record:
	{1:true, 2:234, 3:sdfdgv}
dict:
	{"key1":1, "key2":2, "key3":3}

TLV convertion rules are described in sources.

This project consists from:
/TLV 		- library with basic TLV encoder implementation.
/JsonToTLV 	- console application. Gains the filePath to JSON file we want to convert.
/TestTLV	- google test covering - mainly for internal TLV encoding.