--initalization script. Uses global 'ctx' variable.
ctx:define("dotp", function(stack, ctx, args) 
	ctx:checkArity(args, 2);
	ctx:checkType(stack, "vector", "vector");

	local a, b = stack.pop(2);

	local res = a.at(1) * b.at(1) + 
	a[2] * b[2] + 
	a[3] * b[3] + 
	a[4] * b[4]; 

	stack:push(res);
end)

ctx:define("stddev", function(stack, ctx, args) 
	ctx:checkArity(args, 1);
	ctx:checkType(stack, "array");

	local arr = stack.pop(1);
	local avg = arr:accumulate(Value(0), "+") / #arr
	arr:map(function(a)
		return (a - avg) * (a - avg)
	end)

	return arr:accumulate(Value(0), "+") / #arr
end)
