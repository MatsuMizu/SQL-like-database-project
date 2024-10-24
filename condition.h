# include <stdio.h>
# include <iostream>
# include <stdlib.h>
# include <time.h>
# include <string>
#include <cstring>
#include <memory>
#include <math.h>

#define ERROR_OPEN (-1)
#define ERROR_READ (-2)
#define ERROR_INIT (-3)
#define ERROR_MEMORY (-4)
#define ERROR_INPUT (-5)
#define ERROR_CONFIG (-6)
#define SUCCESS (0)
#define LEN (1234)
#define C_LEN (10)
#define ONE_SYMBOL (-1)
#define ACII_LEN (256)
#define max_items (3)
#define max_commands (3)
#define max_symbol_code (256)
#define groups_amount (1000)

#define debug_helpers (0)

# ifndef ordering_H
# define ordering_H
enum class ordering
{
	none, // not specified
	name, // print name
	phone, // print phone
	group, // print group
};
# endif

# ifndef node_child
# define node_child
enum class child {
	none,
	left,
	right,
};
# endif

# ifndef bit_collections
# define bit_collections
enum class search_directions
{
	none = 0,
	here = 1,
	left = 2,
	right = 4,
	max = none | here | left | right
};
class search_directions_utils
{
public:
	static search_directions bit_or(search_directions a, search_directions b)
	{
		return static_cast<search_directions>(static_cast<int>(a) | static_cast<int>(b));
	}
	static search_directions bit_and(search_directions a, search_directions b)
	{
		return static_cast<search_directions>(static_cast<int>(a) & static_cast<int>(b));
	}
};

enum class field_shouldnt_satisfy
{
	none = 0,
	group = 1,
	phone = 2,
	name = 4,
	all = none | group | phone | name
};
class field_shouldnt_satisfy_utils
{
public:
	static field_shouldnt_satisfy bit_or(field_shouldnt_satisfy a, field_shouldnt_satisfy b)
	{
		return static_cast<field_shouldnt_satisfy>(static_cast<int>(a) | static_cast<int>(b));
	}
	static field_shouldnt_satisfy bit_and(field_shouldnt_satisfy a, field_shouldnt_satisfy b)
	{
		return static_cast<field_shouldnt_satisfy>(static_cast<int>(a) & static_cast<int>(b));
	}
};
# endif

# ifndef operation_H
# define operation_H
enum class operation
{
	none, // not specified
	land, // logical and
	lor, // logical or
};
# endif

# ifndef condition_H
# define condition_H
enum class condition
{
	none,
	eq,
	ne,
	lt,
	gt,
	le,
	ge,
	like,
	nlike,
};
# endif

# ifndef command_type_H
# define command_type_H
enum class command_type
{
	none, // uninitialized
	quit, // end of input stream
	insert, // add record
	select, // find by conditions specified
	del, // delete record
};
# endif