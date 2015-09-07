#include <json-c/json.h>

/*printing the value corresponding to boolean, double, integer and strings*/
void print_json_value(json_object *jobj);

void json_parse_array( json_object *jobj, char *key);

/*Parsing the json object*/
void json_parse(json_object * jobj);
