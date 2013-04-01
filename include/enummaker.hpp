#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/stringize.hpp> 
#include <boost/preprocessor/control/if.hpp> 
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

/*
	This header creates enumermations based on boost sequences, and also binds them to lua.
*/


#pragma once

/*
	This is a helper macro that generates enumerations.
	The first enumeration is 
		element = starting index
	Following elements are preceded by a comma and are displayed as literals.
*/
#define ENUM_MAKER_HELPER(r, startindex, index, element)				\
	BOOST_PP_COMMA_IF(index)											\
	BOOST_PP_IF( BOOST_PP_NOT(index), element = startindex, element)

/*
	This helper macro is used to start an open enumeration.
	It is identical to ENUM_MAKER_HELPER
*/
#define ENUM_MAKER_HELPER_START(r, startindex, index, element)		\
	BOOST_PP_COMMA_IF(index)											\
	BOOST_PP_IF( BOOST_PP_NOT(index), element = startindex, element)

/*
	This macro is used to continue an enumeration. Since the elements
	here will always be inside another enmeration, the preceding comma will
	always be present.
*/
#define ENUM_MAKER_HELPER_CONTINUE(r, startindex, index, element)		\
	,BOOST_PP_IF( BOOST_PP_NOT(index), element = startindex, element)

/*
	Creates a fully formed enumeration with the identifier name, starting at the value index,
	and continuing on with sequence.
	This macro must be followed by a semicolon.

	Example: MAKE_FULL_ENUM(blah, 0, (A)(B)(C)) results in 
	enum blah { A = 0, B, C };
*/
#define MAKE_FULL_ENUM(name, start, sequence) \
	enum name													\
	{															\
		BOOST_PP_SEQ_FOR_EACH_I(ENUM_MAKER_HELPER, start, sequence)	\
	}

/*
	Generates a function body that will convert an enumeration into a string.
*/

#define TOWIDE_LITERAL2(quote) L##quote
#define TOWIDE_LITERAL(quote) TOWIDE_LITERAL2 quote
#define TOSTRING_HELPER(r, name, element) \
	case name :: element:		\
		{													\
			return BOOST_PP_CAT(TOWIDE_LITERAL, ((BOOST_PP_STRINGIZE(element))));			\
		}

#define CONVERT_TO_STRING(namesp, arg, sequence) \
	switch (arg)											\
	{														\
		BOOST_PP_SEQ_FOR_EACH(TOSTRING_HELPER, namesp, sequence)	\
	default:												\
		return L"Failed to find a match in the enumerated sequence."; \
	}														\

#define TOASTRING_HELPER(r, name, element) \
	case name :: element:		\
		{													\
			return BOOST_PP_STRINGIZE(element);				\
		}

#define CONVERT_TO_NARROW_STRING(namesp, arg, sequence) \
	switch (arg)											\
	{														\
		BOOST_PP_SEQ_FOR_EACH(TOASTRING_HELPER, namesp, sequence)	\
	default:												\
		return "Failed to find a match in the enumerated sequence."; \
	}		

/*
	Creates the beginning portion of the enumeration, witih the identifier name, starting at the value index,
	and continuing on with sequence.
	This macro cannot be followed by a semicolon.
	Instead, this macro should be directly followed by one or more CONTINUE_ENUMs, or a CLOSE_ENUM.
*/
#define MAKE_OPEN_ENUM(name, start, sequence) \
	enum name														\
	{																\
		BOOST_PP_SEQ_FOR_EACH_I(ENUM_MAKER_HELPER_START, start, sequence)

/*
	This expands the enumeration with the input sequence, starting at index start.
	This macro should follow MAKE_OPEN_ENUM
*/
#define CONTINUE_ENUM(start, sequence) \
		BOOST_PP_SEQ_FOR_EACH_I(ENUM_MAKER_HELPER_CONTINUE, start, sequence)

/*
	This closes an open enumeration.
	This macro should follow either CONTINUE_ENUM or a MAKE_OPEN_ENUM
	This macro must be followed by a semicolon.
*/
#define CLOSE_ENUM() \
		}

/*	
	This macro expands to a list of luabind::values, that register the
	literal form of the enumeration, along with its value.
*/
#define LUA_MAKER(r, data, index, element)						\
	BOOST_PP_COMMA_IF(index)									\
	luabind::value(												\
		BOOST_PP_STRINGIZE( element )							\
		, BOOST_PP_CAT(BOOST_PP_CAT(data, ::), element) )

/*
	Binds an enumerated sequence to lua. The enumeration must be located within
	namespace space, and be generated from the sequence seq.
*/
#define BIND_SEQ_TO_LUA(space, seq) \
	BOOST_PP_SEQ_FOR_EACH_I(LUA_MAKER, space, seq)


#define PARSER_MAKER(r, prefix, index, element)					\
	(BOOST_PP_STRINGIZE(prefix) "." BOOST_PP_STRINGIZE(element), element)

/*
	The prefix is an identifier, not a string.
*/
#define CREATE_ENUM_PARSER(prefix, name, result, seq)													\
	struct BOOST_PP_CAT(prefix, Parser) : public boost::spirit::qi::symbols<char, name>					\
	{ BOOST_PP_CAT(prefix, Parser) () { add BOOST_PP_SEQ_FOR_EACH_I(PARSER_MAKER, prefix, seq); } };	\
	BOOST_PP_CAT(prefix, Parser) result