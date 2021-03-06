represent - A format representer
=============================================
`represent` is a command line numeric display. It takes a number,
vector, or matrix and will display the result of simple expression in 
a number of different formats. This is particularly helpful in cases
where the bit-layout of a number is important, and for visualizing the 
results of homogenous transformations.

Syntax
--------------------------------------------
The basic syntax of the command line interface is

    represent [options] "expression"

A number may be one of the following formats. Any one of these may be 
preceded by a sign character, either '+' or '-', which designates the
sign of the number. For all vector and matrix operations that require an N 
to determine the size of the matrix or vector, if N is ommitted, then it is
assumed to be 4. Vector and matrix elements are stored as 32 bit floating
point values.

	Format         | Result 
	---------------+-------------------------
	0.123145       | If the double would start with '0.', the leading 0 may be omitted.
	0xABCDEF       | integer, parsed as hex. Values may also be in lowercase.
	0b1011110      | integer, parsed as binary.
	01234567       | integer, parsed as octal.
	123456         | integer, parsed as decimal.
	[0, 1, 2, 3]   | Vector of floats. A vector must have 4 elements. All vectors are row-major.
	q[0, 1, 2, 3]  | Quaternion of floats. The format is in w, x, y, z format.
	{T, T, T ...}  | Array of elements.
	zero()         | Zero vector. N may be 2, 3, or 4.
	unitX()        | Unit vector in the X direction
	unitY()        | Unit vector in the Y direction
	unitZ()        | Unit vector in the Z direction
	unitW()        | Unit vector in the W direction
	pi             | A high precision representation of pi
	e              | A high precision representation of e
	[v;v;v;v]      | Matrix. Each vector must have the same size. Row-major. May omit the []s for the vectors.
	ident()        | Identity matrix
	`string`       | A string. Valid escape characters are \`.wrq1w1

Allowed operators are:
	
	Operator       | Description
	---------------+-----------------------------
	+, -, *, /     | Basic math operators
	.+, .-, .*, .* | Component-wise operators.
	^              | Exponentiation.
	.^             | Component-wise exponentiation
	x              | Cross product. Only valid for vector x vector.
	.              | Dot product. Only valid for vector . vector.
	>>, <<         | Right and left shift. No sign extension.
	>>>, <<<       | Right and left rotate.
	s>>, s<<       | Right and left shift. Sign extended.
	'              | Transpose matrix. 
	~, &, |        | Binary operators.

There are also some functions that may be used to build up values.

	Function       | Description
	---------------+-------------------------------
	rotateN(v, n)  | Returns a NxN rotation matrix around the axis v, of n radians. 
	translateN(v)  | Returns a NxN translation matrix.
	strlen(s)      | Returns the length of a string.
	invert(m)      | Inverts a matrix, or tries to.
	qrotate(v, n)  | Returns a quaternion that represents a rotation of n radians around axis v.

Formats
---------------------------------------------
An expression may be displayed in a number of formats. By default, a numeric value
is displayed in binary and hexadecimal, little and big endian versions of both. 
Additional flags may add or remove formats. By default, the '+' prefix to a 
flag will add it to the output, while a - will remove it. Some options may
be prefixed with an endian descriptor, either 'little-' or 'big-'. If ommitted, 
the format will be displayed in big-endian format. The assumed byte size is 8 bits.

	Descriptor    | E | Format
	--------------+---+------------------------------
	b             | Y | Binary
	h             | Y | Hexadecimal
	o             | Y | Octal
	d             | Y | Decimal
	base64        | N | Base64.
	sf            | N | 32-bit floating point.
	df            | N | 64-bit floating point.
	str           | N | Takes the binary representation, turns it into bytes, and then into a UTF8 string.

To specify a format, use the -f option, and seperate different descriptors with a space.
Binary and Hexadecimal may be followed by a comma delimited list in paraentheses, which 
specify the locations to split the display, starting from 0 as the least significant bit.
If this is not specified, then the output is split at every 8 bits for binary, and 8 characters
for hexadecimal. 64 bits are displayed for Binary, Hex, and octal representations. With no format, 
objects are displayed in binary, hex and binary. If only one format is specified, then no format
descriptor is printed.

Functions
----------------------------------------------
Functions are defined in lua. The lua API is fairly simple, 

    function dotp(stack)
    	local va, vb = stack:pop('vector', 'vector')
    	return va.dot(vb)
    end

Output Examples
----------------------------------------------
Here follows some basic examples.

    > represent -f "b" 42
	00000000 0000000 00000000 00000000 00000000 00000000 00000000 00101010

	> represent -f "d" 0x12123
	74 019

	> represent "unitX4 .+ 4" 
	0 : 
	  DECIMAL |                                                                       4
	  HEX     |                                                                       4
      BINARY  | 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000101

	1 : 
	  DECIMAL |                                                                       0
	  HEX     |                                                                       0
	  BINARY  | 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

	2 : 
	  DECIMAL |                                                                       0
	  HEX     |                                                                       0
	  BINARY  | 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

	3 :
	  DECIMAL |                                                                       0
	  HEX     |                                                                       0
	  BINARY  | 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

	> represent "unitX4 * rotate4(unitZ4, pif/2)"